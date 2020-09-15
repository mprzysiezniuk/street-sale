#include "distributor.h"

int main( int argc, char* argv[] )
{
    srand( time( NULL ) );

    char* path = NULL;
    int sig_num;
    int ads_number;

    if( argc != 6 )
    {
        printf( "You need to specify correct parameters!\n" );
        exit( EXIT_FAILURE );
    }
    getArgs( &sig_num, &ads_number, &path, argc, argv );
    printf( "sig_num = %d \nads_number = %d\npath = %s \n", sig_num, ads_number, path );

    Flyer** flyers = malloc( sizeof( Flyer ) * ads_number );
    if( !flyers )
    {
        perror("Malloc: Error allocating flyers" );
        exit( EXIT_FAILURE );
    }

    for( int i = 0; i < ads_number; i++ )
    {
        flyers[ i ] = malloc( sizeof( Flyer ) );
        if( !flyers[ i ] )
        {
            perror("Malloc: Error allocating flyers" );
            exit( EXIT_FAILURE );
        }
        createFlyer( flyers[ i ], sig_num, i );
    }
// ========================

    struct sigaction sa;

    memset(&sa, '\0', sizeof(sa));
    sa.sa_sigaction = handler;
    sa.sa_flags = SA_SIGINFO;
    if (sigemptyset(&sa.sa_mask))
        perror("sigemptyset");
    if (sigaction(sig_num, &sa, NULL) == -1)
        perror("sigaction error");

// ========================
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

//    printf("Posiadane ulotki: \n\n");
//    for( int i = 0; i < ads_number; i++ )
//    {
//        printf("Ulotka %d: %d\n\n", i, flyers[i]->ad_id);
//    }

    int fd = 0;
    char** fifo_paths = malloc( CONF_FILE_SIZE * sizeof( char* ) );
    printf( "Wczytuje plik konfiguracyjny.\n\n" );
    loadConfigFile( fifo_paths, path );
    printf( "Wczytalem plik konfiguracyjny\n\n" );

    int arr_len = -1;
    while ( ( fifo_paths[ ++arr_len ] ) != NULL );

    int i = 0;
    for(EVER)
    {
//        if ( nanosleep( &time, &time2 ) < 0 )
//        {
//            perror("Nanosleep error: \n");
//            exit(EXIT_FAILURE);
//        }
        sleep(2);

        if ( i == arr_len - 1 )
        {
            i = 0;
        }
        printf( "Otwieram fifo: %s\n\n", fifo_paths[ i ]);
        int status = openFifo( &fd, fifo_paths[i] );
        if ( status == ENXIO )
        {
            printf("W fifo: %s nikogo nie ma, jade dalej\n\n", fifo_paths[i]);
            i++;
            //close(fd);
            continue;
        }
        else if( status == 999 || status == 998 )
        {
            printf( "Podany plik to nie fifo.\n\n" );
            i++;
        }
        else {
            if ( isFifoEmpty( fd ) )
            {
                printf( "fifo jest puste\n\n" );
                i++;
                continue;
            } else
                {

                printf( "Na kanale dystrybucji %s jest klient, wysylam ulotke\n\n", fifo_paths[ i ] );
                sendFlyer( &fd, pickFlyer( flyers, ads_number ) );
                i++;
            }
        }
    }
    return 0;
}

void handler( int sig, siginfo_t *si, void *uap )
{
    printf( "Ulotka o ID: %d zostala odczytana.\n\n", si->si_value.sival_int );
}

Flyer pickFlyer( Flyer** flyers, int ads_number )
{
    return *flyers[ rand() % +( ads_number - 1 ) ];
}

void sendFlyer( int* fd, Flyer flyer )
{
    if (write( *fd, &flyer, sizeof( flyer )) < 0 )
    {
        perror( "sendFlyer: Error writing to file" );
        exit( EXIT_FAILURE );
    }
}

void createFlyer( Flyer* flyer, int signal_number, int ads_number )
{
    flyer->pid = getpid();
    flyer->sig_num = signal_number;
    flyer->ad_id = 1000000 + ads_number;

    int fd = open( "../pan_tadeusz.txt", O_RDONLY );
    if ( fd < 0 )
    {
        perror( "createFlyer: Error opening file" );
        exit( EXIT_FAILURE );
    }

    if ( read( fd, flyer->ad_name, TAB_SIZE - 1 ) == -1 )
    {
        perror( "createFlyer: Error reading file" );
        exit( EXIT_FAILURE );
    }

    flyer->ad_name[ TAB_SIZE - 1 ] = '\0';

    if ( close( fd ) < 0 )
    {
        perror("createFlyer: Error closing file" );
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
        return 999;

    } else if( !S_ISFIFO( stat_p.st_mode ) )
    {
        return 998;
    } else {
        errno = 0;
        if ( ( *fd = open( fifo, O_WRONLY | O_NONBLOCK ) ) < 0 )
        {
            if ( errno != ENXIO )
            {
                perror("openFile: Error opening file" );
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

void getArgs( int* sig_num, int* ad_number, char** path, int argc, char* argv[] )
{
    int opt;
    *sig_num = -1;
    *ad_number = -1;
    while( ( opt = getopt( argc, argv, "s:r:" ) ) != -1 )
    {
        switch( opt )
        {
            case 's':
                *sig_num = strtol( optarg, NULL, 10 );
                break;

            case 'r':
                *ad_number = strtol( optarg, NULL, 10);
                break;

            default:
                printf( "Wrong parameters! Usage: -s <int> -r <int> path \n" );
                exit( EXIT_FAILURE );
        }
    }

    if( *sig_num == -1 || *ad_number == -1 )
    {
        printf( "You need to specify -s and -r parameters!\n" );
        exit( EXIT_FAILURE );
    }
    *path = ( argv )[ optind ];
}