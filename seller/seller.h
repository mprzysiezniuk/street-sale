//#include "stdlib.h"
//#include "stdio.h"
//#include "unistd.h"
//#include "sys/types.h"
//#include "sys/stat.h"
//#include <fcntl.h>
//#include <errno.h>
//#include <signal.h>
//#include <string.h>
//#include <time.h>
//#include <sys/ioctl.h>
#include "../covid.h"

//#define TAB_SIZE 10240
#define MAX_PRODUCT_AMOUNT 1000

typedef struct Products
{
    char towar[TAB_SIZE];
    pid_t pid;
    int sig_num;
    int product_id;
} Product;

void getArgs( int* sig_num, char** path, int argc, char* argv[]);
//int readLine( int fd, char* file );
//void loadConfigFile( char** fifo_paths, char* path_to_conf_file );
int openFifo( int* fd, char* fifo);
//int isFifoEmpty( int fd );
void createProduct( Product* product, int id, int sig_num );
void sendProduct( int* fd, Product product);
void handler(int sig, siginfo_t *si, void *uap);
void handler1();
