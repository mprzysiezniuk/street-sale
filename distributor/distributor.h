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
#define MAX_LEAFLET_AMOUNT 1000
#define EOFILE status==0

typedef struct Leaflets
{
    char ad_name[TAB_SIZE];
    pid_t pid;
    int sig_num;
    int ad_id;
}Leaflet;

void getArgs( int* sig_num, int* ad_number, char** path, int argc, char* argv[] );
void readConfigurationFile( char** fifo_paths, char* path_to_conf_file, int* fd );
int isFifoEmpty(int fd);
int openFifo(int *fd, char *fifo);
void createLeaflet(Leaflet* leaflet, int signal_number, int ads_number);
void sendLeaflet(int* fd, Leaflet leaflet);
int readLine(int fd, char* file);
void handler(int sig, siginfo_t *si, void *uap);
Leaflet pickLeaflet(Leaflet** leaflets, int ads_number);