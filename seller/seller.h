#include "stdlib.h"
#include "stdio.h"
#include "unistd.h"
#include "sys/types.h"

#define TAB_SIZE 10240

struct Product
{
    pid_t pid;
    int product_id;
    char towar[TAB_SIZE];

};

void getArgs( unsigned* sig_num, char** path, int argc, char* argv[]);

