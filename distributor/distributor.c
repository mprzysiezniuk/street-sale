#include "distributor.h"

int main( int argc, char* argv[] )
{
    srand(time(NULL));
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

    //Leaflet leaflet[ads_number];
    Leaflet** leaflets = malloc(sizeof(Leaflet)*MAX_LEAFLET_AMOUNT);
    for( int i = 0; i < ads_number; i++ )
    {
        leaflets[i] = malloc(sizeof(Leaflet));
        createLeaflet(leaflets[i], sig_num, i);
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

//    printf("Posiadane ulotki: \n\n");
//    for( int i = 0; i < ads_number; i++ )
//    {
//        printf("Ulotka %d: %d\n\n", i, leaflets[i]->ad_id);
//    }

    int fd = 0;
    char** fifo_paths = malloc(CONF_FILE_SIZE * sizeof(char*));
    printf("Wczytuje plik konfiguracyjny.\n\n");
    readConfigurationFile(fifo_paths, path, &fd);
    printf("Wczytalem plik konfiguracyjny\n\n");

    int arr_len = -1;
    while ((fifo_paths[++arr_len]) != NULL);

//    for (int i = 0; i < arr_len; i++)
//    {
//        if (isFifoEmpty(fd, fifo_paths[i]) == 1)
//        {
//            printf("Fifo: %s jest puste\n\n", fifo_paths[i]);
//            continue;
//        }
//        printf("Otwieram fifo: %s\n\n", fifo_paths[i]);
//        openFifo(&fd, fifo_paths[i]);
//        printf("Przesylam ulotke klientowi.\n\n");
//        sendLeaflet(&fd, *leaflet);
//    }
//
//    free(leaflet);
    int i = 0;
    while( 1 ) {
//        if ( nanosleep( &time, &time2 ) < 0 )
//        {
//            perror("Nanosleep error: \n");
//            exit(EXIT_FAILURE);
//        }
        sleep(5);

        if (i == arr_len - 1) {
            i = 0;
        }
        printf("Otwieram fifo: %s\n\n", fifo_paths[i]);
        if (openFifo(&fd, fifo_paths[i]) == ENXIO)
        {
            printf("W fifo: %s nikogo nie ma, jade dalej\n\n", fifo_paths[i]);
            i++;
            //close(fd);
            continue;
        } else {
            if (isFifoEmpty(fd, fifo_paths[i])) {
                //close(fd);
                //printf("fifo nie jest puste\n\n");
                i++;
                continue;
            } else {

                printf("Na kanale dystrybucji %s jest klient, wysylam ulotke\n\n", fifo_paths[i]);
                sendLeaflet(&fd, pickLeaflet(leaflets, ads_number));
                i++;
            }
        }
    }

    return 0;
}

void handler(int sig, siginfo_t *si, void *uap)
{
    printf("Ulotka o ID: %d zostala odczytana.\n\n", si->si_value.sival_int);
}

Leaflet pickLeaflet(Leaflet** leaflets, int ads_number)
{
    return *leaflets[rand()%+(ads_number-1)];
}

void readConfigurationFile(char** fifo_paths, char* path_to_conf_file, int* fd)
{
    printf("wchodze do openFile\n");
    int fd_conf = open(path_to_conf_file, O_RDONLY);
    printf("otworzony konfiguracyjny\n");
    if (fd_conf < 0)
    {
        perror("Chujowo\n");
        exit(-1);
    }
    *fifo_paths = (char*) malloc(256);
    while (readLine(fd_conf, *fifo_paths++) > 0)
    {
        *fifo_paths = (char*) malloc(256);
    }
}

int openFifo(int *fd, char *fifo)
{
    errno = 0;
    if ((*fd = open(fifo, O_WRONLY | O_NONBLOCK)) < 0) {
        if (errno != ENXIO) {
            perror("openFile: Could not open file\n");
            exit(EXIT_FAILURE);
        }
    }
    if (errno == ENXIO) {
        //close(*fd);
        return errno;
    }
    errno = 0;
    return 0;
}


int isFifoEmpty(int fd, char* fifo)
{
    int curr_location = lseek( fd, 0, SEEK_CUR );
    if( !lseek(fd, 0, SEEK_END ) )
        return 1;
    else
    {
        lseek( fd, curr_location, SEEK_SET );
        return 0;
    }
}

void createLeaflet(Leaflet* leaflet, int signal_number, int ads_number)
{
    leaflet->pid = getpid();
    leaflet->sig_num = signal_number;
    leaflet->ad_id = 1000000 + ads_number;

    int fd = open("../pan_tadeusz.txt", O_RDONLY);
    if (fd == -1)
    {
        printf("createLeaflet: File open failed\n");
        exit(-1);
    }

    if (read(fd, leaflet->ad_name, TAB_SIZE - 1) == -1)
    {
        printf("createLeaflet: Read from file failed\n");
        exit(-1);
    }

    leaflet->ad_name[TAB_SIZE - 1] = '\0';

    if (close(fd) < 0)
    {
        printf("createLeaflet: Could not close file\n");
        exit(EXIT_FAILURE);
    }
}

void sendLeaflet(int* fd, Leaflet leaflet)
{
    if (write(*fd, &leaflet, sizeof(Leaflet)) < 0)
    {
        perror("sendLeaflet: Could not write to fifo: \n");
        exit(-1);
    }
}

int readLine(int fd, char* file)
{
    char* buff = file;
    char c;
    int status;
    while (1)
    {
        status = read(fd, &c, 1);
        if (status < 0)
        {
            perror("readLine: read error!\n");
            exit(0);
        }
        if (c == '\n' || EOFILE)
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

//    if( optind >= argc )
//    {
//        printf("You need to specify configuration file path!\n");
//        exit(0);
//    }
    *path = (argv)[ optind ];
}