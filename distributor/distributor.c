#include "distributor.h"

int main( int argc, char* argv[] )
{
    char* path = NULL;
    int sig_num;
    unsigned ad_number;
    if( argc != 6 )
    {
        printf( "You need to specify correct parameters!\n" );
        exit( EXIT_FAILURE );
    }
    getArgs( &sig_num, &ad_number, &path, argc, argv );
    printf( "sig_num = %d \nad_number = %d\npath = %s \n", sig_num, ad_number, path );
    return 0;
}

void getArgs( int* sig_num, unsigned* ad_number, char** path, int argc, char* argv[] )
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

//    if( optind >= argc )
//    {
//        printf("You need to specify configuration file path!\n");
//        exit(0);
//    }
    *path = (argv)[ optind ];
}