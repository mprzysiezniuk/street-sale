#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <signal.h>
#include <fcntl.h>
#include <time.h>
#include <string.h>

#define TAB_SIZE 193
#define CONF_FILE_SIZE 50
#define EOFILE status==0

struct Item
{
    char product_name[TAB_SIZE];
    pid_t pid;
    int sig_num;
    int product_id;
};

void getArgs(unsigned int* number_of_products, char** path, int argc, char* argv[]);
char* pickFifo( char** fifo_paths );
void openFifo( int* fd, char* fifo_path );;
void collectItem( int* fd, char* fifo_path);
int readLine( int fd, char* file );
void readConfigurationFile( char** fifo_paths, char* path_to_conf_file, int* fd );