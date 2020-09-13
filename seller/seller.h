#include "stdlib.h"
#include "stdio.h"
#include "unistd.h"
#include "sys/types.h"
#include "sys/stat.h"
#include <fcntl.h>
#include <errno.h>

//#define TAB_SIZE 10240
#define TAB_SIZE 193
#define MAX_PRODUCT_AMOUNT 1000
#define CONF_FILE_SIZE 50
#define EOFILE status==0

typedef struct Products
{
    char towar[TAB_SIZE];
    pid_t pid;
    int sig_num;
    int product_id;

} Product;

void getArgs( int* sig_num, char** path, int argc, char* argv[]);
void createProduct( Product* product, int id, int sig_num );
void readConfigurationFile( char** fifo_paths, char* path_to_conf_file, int* fd );
void openFifo( int* fd, char* fifo);
void sendProduct( int* fd, Product product);
int readLine( int fd, char* file );
