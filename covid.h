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

int readLine( int fd, char* file);
void loadConfigFile( char** fifo_paths, char* path_to_conf_file );
int isFifoEmpty( int fd );
