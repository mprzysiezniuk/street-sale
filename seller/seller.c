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
    printf("sig_num = %d \npath = %s \n", sig_num, path);
    createProduct(product, 1, sig_num);
    printf("id: %d, pid: %d\nsig_num: %d\ntowar: %s\n", product->product_id, product->pid, product->sig_num, product->towar);

    int fd = 0;
    openFile( path, &fd );

    write( fd, product, sizeof(Product) );
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

void openFile( char* path, int* fd )
{
    char file[256];
    printf("wchodze do openFile\n");
    int fd_conf = open( path, O_RDONLY );
    printf("otworzony konfiguracyjny\n");
    if ( fd_conf < 0 )
    {
        perror("Chujowo\n");
        exit( EXIT_FAILURE );
    }
    readLine( fd_conf, file);
    printf("file = %s\n", file);
    //printf("path = %s\n", fifo_path);
    if( ( *fd = open( file, O_WRONLY | O_NONBLOCK ) ) < 0 )
    {
        if ( errno != ENXIO)
        {
            perror("openFile: Could not open file\n");
            exit(EXIT_FAILURE);
        }
    }
}

void readLine( int fd, char* file )
{
    char* buff = file;
    char c;
    while(1)
    {
        if(read(fd, &c, 1) < 0)
        {
            perror("readLine: read error!\n");
            exit(0);
        }
        if(c == '\n')
        {
            break;
        }
        *buff = c;
        buff++;
    }
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