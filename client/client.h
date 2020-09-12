#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <signal.h>
#include <fcntl.h>

#define TAB_SIZE 193

struct Item
{
    char product_name[TAB_SIZE];
    pid_t pid;
    int sig_num;
    int product_id;
};

void getArguments(unsigned int* number_of_products, char** path, int argc, char* argv[]);
void receiveItem(char* fifo_path);