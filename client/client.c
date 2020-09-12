#include "client.h"

int main( int argc, char* argv[] )
{
    char* path = NULL;
    unsigned amount;
    if( argc < 2 || argc > 4 )
    {
        printf( "You need to specify correct parameters!\n");
        exit( 0 );
    }
    getArgs( &amount, &path, argc, argv);
    printf("amount = %d \npath = %s \n", amount, path);

    return 0;
}

void getArgs( unsigned* amount, char** path, int argc, char* argv[] )
{
    int opt;
    *amount = 1;
    while( ( opt = getopt( argc, argv, "k:" ) ) != -1 )
    {
        switch( opt )
        {
            case 'k':
                *amount = strtol( optarg, NULL, 10 );
                break;
            default:
                printf( "Wrong parameters! Usage: -k <int> path \n" );
                exit(0);
        }
    }

    if( optind >= argc )
    {
        printf("You need to specify configuration file path!\n");
        exit(0);
    }
    *path = (argv)[ optind ];
}