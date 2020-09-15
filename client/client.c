#include "client.h"


int main( int argc, char* argv[] )
{
    srand( time( NULL ) );

    if( argc < 2 || argc > 4 )
    {
        printf("Wrong number of parameters. Correct: [-k <int>] <path>\n" );
        exit( EXIT_FAILURE );
    }

    int products_amount = 1;
    char* path = NULL;

    getArgs( &products_amount, &path, argc, argv );

    char** fifo_paths = malloc( CONF_FILE_SIZE * sizeof( char* ) );
    if( !fifo_paths )
    {
        perror( "Malloc: Error allocating fifo_paths" );
        exit( EXIT_FAILURE );
    }

    //printf( "Wczytuje plik konfiguracyjny.\n\n" );
    loadConfigFile( fifo_paths, path );
    //printf( "Wczytalem plik konfiguracyjny\n\n" );

    int fd[ products_amount ];

    int i = 0;
    char* fifo = malloc( sizeof( char ) * 256 );

    Item* item = malloc(sizeof(Item));
    for( i = 0; i < products_amount; i++ )
    {
        //sleep(5);
        fifo = pickFifo( fifo_paths );
        printf( "Wybrane fifo: %s\n\n", fifo );

        printf( "Otwieram wybrane fifo w trybie READ.\n\n" );
        item = collectItem( &fd[ i ], fifo );
        if( item->product_id < 1000000 )
        {
            printf("Natrafilem na towar, czyszcze fifo.\n\n");
            sleep(4);
            flushFifo( &fd[ i ], fifo );
        }
        else
        {
            printf("Natrafilem na ulotke, zamykam fifo.\n\n");
            close( fd[ i ] );
            continue;
        }
        sendSignal( item );
        close( fd[ i ] );

    }

    return 0;
}

Item* collectItem( int* fd, char* fifo_path )
{
    Item* item = malloc( sizeof( Item ) );
    if( !item )
    {
        perror("collectItem: Error allocating item" );
        exit( EXIT_FAILURE );
    }
    openFifo( fd, fifo_path );
    sleep(2);
    read( *fd, item, sizeof( Item ) );

    return item;
}

void flushFifo( int* fd, char* fifo_path )
{
    Item** item = malloc( sizeof( Item ) * 6 );
    int i = 0;
    while( !isFifoEmpty( *fd ) )
    {
        *item = malloc( sizeof( Item ) );
        read( *fd, *item, sizeof( Item ) );
        if( !i )
        {
            printf("Wysylam sygnal ulotce przy czyszczeniu\n");
            sendSignal( *item);
        }
        item++;
        i = !i;
    }
    close( *fd );
}

void sendSignal( Item* item )
{
    union sigval sig;
    sig.sival_int = item->product_id;
    //printf( "Item ID: %d\n\n", item->product_id );
    //printf( "Towar: %s\n\n", item->product_name );
    if ( sigqueue( item->pid, item->sig_num, sig ) < 0 )
    {
        perror( "receiveItem: failed to send signal: " );
        exit( EXIT_FAILURE );
    }
    printf("Wyslalem sygnal.\n\n");

}

int isFifoEmpty( int fd )
{
    int sz = 0;
    ioctl( fd, FIONREAD, &sz );
    return !sz;
}

void openFifo( int* fd, char* fifo_path )
{
    struct stat stat_p;
    if( stat( fifo_path, &stat_p ) < 0 )
    {
        if( mkfifo( fifo_path, 0600 ) < 0 )
        {
            perror("Error creating fifo");
            exit( EXIT_FAILURE );
        }
    }

    if( ( *fd = open( fifo_path, O_RDONLY ) ) < 0 )
    {
        perror("openFifo: error opening fifo.\n");
        exit( EXIT_FAILURE );
    }
}

char* pickFifo( char** fifo_paths )
{
    int arr_len = -1;
    while( ( fifo_paths[ ++arr_len ] ) != NULL );
    //printf("rozmiar: %d", arr_len);
    int i = rand() % ( arr_len - 2 );
    printf("nr wybranego el: %d\n", i);
    return fifo_paths[ i ];
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

void getArgs( int* number_of_products, char** path, int argc, char* argv[] )
{
    int opt;
    while( ( opt = getopt( argc, argv,"k:" ) ) != -1 )
    {
        switch( opt )
        {
            case 'k':
                *number_of_products=strtol( optarg, NULL, 10 );
                break;
            default:
                printf( "Wrong parameters. Correct: [-k <int>] <path>\n" );
                exit( EXIT_FAILURE );
        }
    }

    *path=( argv )[ optind ];
}
