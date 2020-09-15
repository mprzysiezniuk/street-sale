#include "seller.h"

int isSold[MAX_PRODUCT_AMOUNT];
int counter = 0;

int main(int argc, char *argv[])
{
    if (argc != 4)
    {
        printf("You need to specify correct parameters!\n");
        exit(EXIT_FAILURE);
    }

    char *path = NULL;
    int sig_num;
    memset(isSold, 0, MAX_PRODUCT_AMOUNT*sizeof(int));
    getArgs(&sig_num, &path, argc, argv);

    printf("Wczytalem parametry.\n\n");

    Product** products = malloc( sizeof( Product ) * MAX_PRODUCT_AMOUNT );
    if( !products )
    {
        perror("malloc: Error allocating products");
        exit( EXIT_FAILURE );
    }

    srand( time(NULL));

    struct sigaction sa;

    memset(&sa, '\0', sizeof(sa));

    sa.sa_sigaction = handler;
    sa.sa_flags = SA_SIGINFO;

    if ( sigemptyset( &sa.sa_mask ) )
        perror("sigemptyset");
    if (sigaction( sig_num, &sa, NULL ) == -1 )
        perror( "sigaction error" );

// ========================
//    struct sigaction sa1;
//
//    memset(&sa1, '\0', sizeof(sa1));
//    sa1.sa_sigaction = handler1;
//    sa1.sa_flags = SA_SIGINFO;
//    if (sigemptyset(&sa.sa_mask))
//        perror("sigemptyset");
//    if (sigaction(SIGUSR1, &sa1, NULL) == -1)
//        perror("sigaction error");
// ========================

    char **fifo_paths = malloc(CONF_FILE_SIZE * sizeof(char *));
    if( !fifo_paths )
    {
        perror("Malloc: Error allocating fifo_paths");
        exit( EXIT_FAILURE );
    }

    printf("Wczytuje plik konfiguracyjny.\n\n");
    loadConfigFile( fifo_paths, path );
    printf("Wczytalem plik konfiguracyjny\n\n");

    printf("Otwieram fifo po kolei z listy\n\n");

    int arr_len = -1;
    while ( ( fifo_paths[ ++arr_len ] ) != NULL );

    int i = 0;
    int fd = 0;

    Product *ptr = *products;
    for(EVER)
    {
        sleep(2);

        if ( i == arr_len - 1 )
        {
            i = 0;
        }
        printf("Otwieram fifo: %s\n\n", fifo_paths[i]);
        int status = openFifo( &fd, fifo_paths[i] );
        if(  status == ENXIO )
        {
            printf("W fifo: %s nikogo nie ma, jade dalej\n\n", fifo_paths[i]);
            i++;
        } else if( status == FIFO_ERR )
        {
            printf("Podany plik to nie fifo.\n\n");
            i++;
        }else
        {
            if( !isFifoEmpty( fd ) )
            {
                printf("Kanal dystrybucji jest zajety\n\n");
                i++;
            } else
            {
                ptr = malloc( sizeof( Product ) );
                if( !ptr )
                {
                    perror("Malloc: Error allocating ptr");
                    exit( EXIT_FAILURE );
                }
                createProduct( ptr, counter, sig_num );
                printf( "Na kanale dystrybucji %s jest klient, wysylam produkt\n\n", fifo_paths[ i ] );
                sendProduct( &fd, *ptr++ );
                //1 - produkt wyslany, ale nie oplacony
                isSold[ counter++ ] = 1;
                i++;
            }
        }
    }
    return 0;
}

void handler( int sig, siginfo_t *si, void *uap )
{
    printf( "Otrzymalem zaplate za towar o ID: %d.\n\n", si->si_value.sival_int );
    //2 - produkt zostal oplacony
    isSold[si->si_value.sival_int] = 2;
}

void handler1()
{
    int amount = 0;
    printf("========= RAPORT =========\n");
    printf( "Products released: %d\n\n", counter );
    for( int i = 0; i < counter; i++ )
    {
        if( isSold[ i ] != 2 )
        {
            amount++;
        }
        else
        {
            printf( "Product number: %d is unpaid\n\n", i );
        }
    }
    printf( "Unpaid products number: %d\n\n", amount );
}

void sendProduct( int *fd, Product product )
{
    if ( write( *fd, &product, sizeof( Product ) ) < 0 )
    {
        perror("sendProduct: Error writing to file" );
        exit( EXIT_FAILURE );
    }
}

void createProduct( Product *product, int id, int sig_num )
{
    product->product_id = id;
    product->sig_num = sig_num;
    product->pid = getpid();

    int fd = open( "../pan_tadeusz.txt", O_RDONLY );
    if ( fd < 0 )
    {
        perror( "createProduct: Error opening file" );
        exit( EXIT_FAILURE );
    }

    if ( read( fd, product->towar, TAB_SIZE - 1 ) < 0 )
    {
        perror( "createProduct: Error reading file" );
        exit( EXIT_FAILURE );
    }
    product->towar[ TAB_SIZE - 1 ] = '\0';

    if ( close( fd ) < 0 )
    {
        perror( "createProduct: Error closing file" );
        exit( EXIT_FAILURE );
    }
}

int isFifoEmpty( int fd )
{
    int sz = 0;
    ioctl( fd, FIONREAD, &sz );
    return !sz;
}

int openFifo( int *fd, char *fifo )
{
    struct stat stat_p;
    if( stat( fifo, &stat_p ) < 0 )
    {
        return FIFO_ERR;

    } else if( !S_ISFIFO(stat_p.st_mode) )
    {
        return FIFO_ERR;
    } else
    {
        errno = 0;
        if ( ( *fd = open( fifo, O_WRONLY | O_NONBLOCK ) ) < 0 )
        {
            if ( errno != ENXIO )
            {
                perror( "openFile: Error opening file" );
                exit( EXIT_FAILURE );
            }
        }
        if ( errno == ENXIO )
        {
            return errno;
        }
        errno = 0;
        return 0;
    }
}

void loadConfigFile( char** fifo_paths, char* path_to_conf_file )
{
    printf( "wchodze do openFile\n" );
    int fd_conf = open( path_to_conf_file, O_RDONLY );
    printf( "otworzony konfiguracyjny\n" );
    if ( fd_conf < 0 )
    {
        perror("loadConfigFile: Error opening file" );
        exit( EXIT_FAILURE );
    }
    *fifo_paths = ( char* ) malloc( 256 );
    while ( readLine( fd_conf, *fifo_paths++ ) > 0 )
    {
        *fifo_paths = ( char* ) malloc( 256 );
    }
}

int readLine( int fd, char* file )
{
    char* buff = file;
    char c;
    int status;
    for(EVER)
    {
        status = read(fd, &c, 1);
        if (status < 0)
        {
            perror( "readLine: Error reading file" );
            exit( EXIT_FAILURE );
        }
        if ( c == '\n' || EOFILE )
        {
            break;
        }
        *buff = c;
        buff++;
    }
    return status;
}

void getArgs( int *sig_num, char **path, int argc, char *argv[] )
{
    int opt;
    *sig_num = -1;
    while ( ( opt = getopt( argc, argv, "s:" ) ) != -1 )
    {
        switch ( opt )
        {
            case 's':
                *sig_num = strtol( optarg, NULL, 10 );
                break;
            default:
                printf( "Wrong parameters! Usage: -s <int> path \n" );
                exit( EXIT_FAILURE );
        }
    }

    if ( *sig_num == -1 )
    {
        printf( "You need to specify -s parameter!\n" );
        exit( EXIT_FAILURE );
    }

    *path = ( argv )[ optind ];
}