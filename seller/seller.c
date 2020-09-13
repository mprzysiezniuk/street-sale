#include "seller.h"

int main( int argc, char* argv[] )
{
    if( argc != 4 )
    {
        printf( "You need to specify correct parameters!\n" );
        exit( EXIT_FAILURE );
    }

    char* path = NULL;
    //char isSold[MAX_PRODUCT_AMOUNT];
    int sig_num;
    Product* product = malloc(sizeof(Product));
//    product->towar = malloc(sizeof(char)*TAB_SIZE);
    getArgs( &sig_num, &path, argc, argv);
    printf("Wczytalem parametry.\n\n");
    printf("sig_num = %d \npath = %s \n\n", sig_num, path);
    createProduct(product, 1, sig_num);
    printf("Stworzylem produkt.\n\n");
    printf("id: %d, pid: %d\nsig_num: %d\ntowar: %s\n\n", product->product_id, product->pid, product->sig_num, product->towar);

    int fd = 0;
    char** fifo_paths = malloc(CONF_FILE_SIZE * sizeof(char*));
    printf("Wczytuje plik konfiguracyjny.\n\n");
    readConfigurationFile( fifo_paths, path, &fd );
    printf("Wczytalem plik konfiguracyjny\n\n");
    printf("Otwieram fifo\n\n");

    //openFifo( &fd, fifo_paths[0]);
    printf("Przesylam produkt klientowi.\n\n");

    free( product );
    return 0;
}

void createProduct( Product* product, int id, int sig_num )
{
    product->product_id = id;
    product->sig_num = sig_num;
    product->pid = getpid();

    int fd = open( "../pan_tadeusz.txt", O_RDONLY );
    if( fd < 0)
    {
        printf("createProduct: Could not open file\n");
        exit( EXIT_FAILURE );
    }

    if( read( fd, product->towar, TAB_SIZE - 1 ) < 0 )
    {
        printf("createProduct: Could not read from file\n");
        exit( EXIT_FAILURE );
    }
    product->towar[TAB_SIZE - 1] = '\0';

    if( close( fd ) < 0 )
    {
        printf("createProduct: Could not close file\n");
        exit( EXIT_FAILURE );
    }
}

void sendProduct( int* fd, Product product)
{
    if( write( *fd, &product, sizeof(Product) ) < 0 )
    {
        perror("sendProduct: Could not write to fifo: \n");
        exit(EXIT_FAILURE);
    }
}

void openFifo( int* fd, char* fifo)
{
        if( ( *fd = open( fifo, O_WRONLY | O_NONBLOCK ) ) < 0 )
    {
        if ( errno != ENXIO)
        {
            perror("openFile: Could not open file\n");
            exit(EXIT_FAILURE);
        }
    }
}

void readConfigurationFile( char** fifo_paths, char* path_to_conf_file, int* fd )
{
    printf("wchodze do openFile\n");
    int fd_conf = open( path_to_conf_file, O_RDONLY );
    printf("otworzony konfiguracyjny\n");
    if ( fd_conf < 0 )
    {
        perror("Chujowo\n");
        exit( EXIT_FAILURE );
    }
    *fifo_paths = (char*)malloc(256);
    while( readLine( fd_conf, *fifo_paths++) > 0 )
    {
        *fifo_paths = (char*)malloc(256);
    }
}

int readLine( int fd, char* file )
{
    char* buff = file;
    char c;
    int status;
    while( 1 )
    {
        status = read( fd, &c, 1 );
        if( status < 0 )
        {
            perror("readLine: read error!\n");
            exit(0);
        }
        if( c == '\n' || EOFILE )
        {
            break;
        }
        *buff = c;
        buff++;
    }
    return status;
}

void getArgs( int* sig_num, char** path, int argc, char* argv[] )
{
    int opt;
    *sig_num = -1;
    while( ( opt = getopt( argc, argv, "s:" ) ) != -1 )
    {
        switch( opt )
        {
            case 's':
                *sig_num = strtol( optarg, NULL, 10 );
                break;
            default:
                printf( "Wrong parameters! Usage: -s <int> path \n" );
                exit( EXIT_FAILURE );
        }
    }

    if( *sig_num == -1 )
    {
        printf( "You need to specify -s parameter!\n" );
        exit( EXIT_FAILURE );
    }

//    if( optind >= argc )
//    {
//        printf("You need to specify configuration file path!\n");
//        exit(0);
//    }

    *path = (argv)[ optind ];
}