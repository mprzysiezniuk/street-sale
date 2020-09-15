#include "covid.h"

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

int isFifoEmpty( int fd )
{
    int sz = 0;
    ioctl( fd, FIONREAD, &sz );
    return !sz;
}