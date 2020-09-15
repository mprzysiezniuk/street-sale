#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h>
#include <fcntl.h>
#include <time.h>
#include <errno.h>
#include <string.h>
#include <sys/ioctl.h>

#define TAB_SIZE 193
#define CONF_FILE_SIZE 50
#define EOFILE status==0
#define EVER ;;
#define FIFO_ERR 999

typedef struct Flyers
{
    char ad_name[TAB_SIZE];
    pid_t pid;
    int sig_num;
    int ad_id;
} Flyer;

void getArgs( int* sig_num, int* ad_number, char** path, int argc, char* argv[] );
int readLine( int fd, char* file);
void loadConfigFile( char** fifo_paths, char* path_to_conf_file );
int openFifo( int *fd, char *fifo );
int isFifoEmpty( int fd );
void createFlyer( Flyer* flyer, int signal_number, int ads_number );
void sendFlyer( int* fd, Flyer flyers );
Flyer pickFlyer( Flyer** flyers, int ads_number );
void setSigaction( int sig_num, void(*handler) );
void handler( int sig, siginfo_t *si, void *uap );
void setSigactionUsr( void( *handler ) );
void handler1(  );