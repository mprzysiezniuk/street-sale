#include "seller.h"

int main( int argc, char* argv[] )
{
    char* path = NULL;
    unsigned sig_num;
    if( argc != 4 )
    {
        printf( "You need to specify correct parameters!\n" );
        exit( 0 );
    }
    getArgs( &sig_num, &path, argc, argv);
    printf("sig_num = %d \npath = %s \n", sig_num, path);
    return 0;
}

void getArgs( unsigned* sig_num, char** path, int argc, char* argv[] )
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
                exit( 0 );
        }
    }

    if( *sig_num == -1 )
    {
        printf( "You need to specify -s parameter!\n" );
        exit( 0 );
    }

    if( optind >= argc )
    {
        printf("You need to specify configuration file path!\n");
        exit(0);
    }

    *path = (argv)[ optind ];
}