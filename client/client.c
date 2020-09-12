#include "client.h"

int main(int argc, char* argv[])
{
    if(argc < 2 || argc > 4)
    {
        perror("Wrong number of parameters. Correct: [-k <int>] <path>\n");
        exit(-1);
    }

    unsigned int products_amount = 1;
    char* path = NULL;
    getArguments(&products_amount, &path, argc, argv);
    printf("Number of products: %u\nPath: %s\n", products_amount, path);
    receiveItem(path);

    return 0;
}

void getArguments(unsigned int* number_of_products, char** path, int argc, char* argv[])
{
    int opt;
    while((opt = getopt(argc, argv,"k:")) != -1)
    {
        switch(opt)
        {
            case 'k':
                *number_of_products=strtol(optarg, NULL, 10);
                break;
            default:
                perror("Wrong parameters. Correct: [-k <int>] <path>\n");
                exit(-1);
        }
    }

    *path=(argv)[optind];
}

void receiveItem(char* fifo_path)
{
    int fd = 0;
    struct Item* item = malloc(sizeof(struct Item));
    //union sigval sig;
    if((fd = open(fifo_path, O_RDONLY)) < 0)
    {
        perror("Błąd");
        exit(-1);
    }
    int size = read(fd, item, sizeof(struct Item) );
    printf("size of read: %d", size);

    //sig.sival_int = item->product_id;
    printf("Signal: %d\n", item->sig_num);
    printf("Towar: %s\n", item->product_name);
    close(fd);
    //sigqueue(item->pid, item->sig_num, sig);
}