#include "seller.h"

int isSold[MAX_PRODUCT_AMOUNT];
int counter = 0;

int main(int argc, char *argv[])
{
    if (argc != 4) {
        printf("You need to specify correct parameters!\n");
        exit(EXIT_FAILURE);
    }

    char *path = NULL;

    int sig_num;
    memset(isSold, 0, MAX_PRODUCT_AMOUNT*sizeof(int));
//    product->towar = malloc(sizeof(char)*TAB_SIZE);
    getArgs(&sig_num, &path, argc, argv);
    printf("Wczytalem parametry.\n\n");

    Product** products = malloc( sizeof( Product ) * MAX_PRODUCT_AMOUNT );

    srand( time(NULL));

    struct sigaction sa;

    memset(&sa, '\0', sizeof(sa));
    sa.sa_sigaction = handler;
    sa.sa_flags = SA_SIGINFO;
    if (sigemptyset(&sa.sa_mask))
        perror("sigemptyset");
    if (sigaction(sig_num, &sa, NULL) == -1)
        perror("sigaction error");

// ========================

    struct sigaction sa1;

    memset(&sa1, '\0', sizeof(sa1));
    sa1.sa_sigaction = handler1;
    sa1.sa_flags = SA_SIGINFO;
    if (sigemptyset(&sa.sa_mask))
        perror("sigemptyset");
    if (sigaction(SIGUSR1, &sa1, NULL) == -1)
        perror("sigaction error");

// ========================

    int fd = 0;

    char **fifo_paths = malloc(CONF_FILE_SIZE * sizeof(char *));
    printf("Wczytuje plik konfiguracyjny.\n\n");
    readConfigurationFile(fifo_paths, path, &fd);
    printf("Wczytalem plik konfiguracyjny\n\n");

    printf("Otwieram fifo po kolei z listy\n\n");

    int arr_len = -1;
    while ((fifo_paths[++arr_len]) != NULL);

    int i = 0;

    Product *ptr = *products;
    while( 1 )
    {
        sleep(5);

        if ( i == arr_len - 1 )
        {
            i = 0;
        }
        printf("Otwieram fifo: %s\n\n", fifo_paths[i]);
        if( openFifo( &fd, fifo_paths[i] ) == ENXIO )
        {
            printf("W fifo: %s nikogo nie ma, jade dalej\n\n", fifo_paths[i]);
            i++;
            //close(fd);
            continue;
        } else
        {
            if( !isFifoEmpty( fd ) )
            {
                printf("Kanal dystrybucji jest zajety\n\n");
                i++;
                continue;
            } else
            {
                ptr = malloc(sizeof(Product));
                createProduct(ptr, counter, sig_num);
                printf("Na kanale dystrybucji %s jest klient, wysylam produkt\n\n", fifo_paths[i]);
                sendProduct(&fd, *ptr++);
                //1 - produkt wyslany, ale nie oplacony
                isSold[counter++] = 1;
                i++;
            }
        }
    }

    return 0;
}

void handler(int sig, siginfo_t *si, void *uap)
{
    printf("Otrzymalem zaplate za towar o ID: %d.\n\n", si->si_value.sival_int);
    //2 - produkt zostal oplacony
    isSold[si->si_value.sival_int] = 2;
}

void handler1()
{
    int amount = 0;
    printf("========= RAPORT =========\n");
    printf("Products released: %d\n\n", counter);
    for(int i = 0; i < counter; i++)
    {
        if(isSold[i] != 2)
        {
            amount++;
        }
        else
        {
            printf("Product number: %d is unpaid\n\n", i);
        }
    }
    printf("Unpaid products number: %d\n\n", amount);
}

int isFifoEmpty(int fd)
{
    int sz = 0;
    ioctl(fd, FIONREAD, &sz);
    return !sz;
}

void createProduct(Product *product, int id, int sig_num)
{
    product->product_id = id;
    product->sig_num = sig_num;
    product->pid = getpid();

    int fd = open("../pan_tadeusz.txt", O_RDONLY);
    if (fd < 0)
    {
        printf("createProduct: Could not open file\n");
        exit(EXIT_FAILURE);
    }

    if (read(fd, product->towar, TAB_SIZE - 1) < 0)
    {
        printf("createProduct: Could not read from file\n");
        exit(EXIT_FAILURE);
    }
    product->towar[TAB_SIZE - 1] = '\0';

    if (close(fd) < 0)
    {
        printf("createProduct: Could not close file\n");
        exit(EXIT_FAILURE);
    }
}

void sendProduct(int *fd, Product product)
{
    if (write(*fd, &product, sizeof(Product)) < 0)
    {
        perror("sendProduct: Could not write to fifo: \n");
        exit(EXIT_FAILURE);
    }
}

int openFifo(int *fd, char *fifo)
{
    errno = 0;
    if ((*fd = open(fifo, O_WRONLY | O_NONBLOCK)) < 0)
    {
        if (errno != ENXIO)
        {
            perror("openFile: Could not open file\n");
            exit(EXIT_FAILURE);
        }
    }
    if (errno == ENXIO)
    {
        return errno;
    }
    errno = 0;
    return 0;
}

void readConfigurationFile(char **fifo_paths, char *path_to_conf_file, int *fd)
{
    printf("wchodze do openFile\n");
    int fd_conf = open(path_to_conf_file, O_RDONLY);
    printf("otworzony konfiguracyjny\n");
    if (fd_conf < 0)
    {
        perror("readConfigurationFile failure");
        exit(EXIT_FAILURE);
    }
    *fifo_paths = (char *) malloc(256);
    while (readLine(fd_conf, *fifo_paths++) > 0)
    {
        *fifo_paths = (char *) malloc(256);
    }
}

int readLine(int fd, char *file)
{
    char *buff = file;
    char c;
    int status;
    while (1) {
        status = read(fd, &c, 1);
        if (status < 0) {
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

void getArgs(int *sig_num, char **path, int argc, char *argv[])
{
    int opt;
    *sig_num = -1;
    while ((opt = getopt(argc, argv, "s:")) != -1)
    {
        switch (opt)
        {
            case 's':
                *sig_num = strtol(optarg, NULL, 10);
                break;
            default:
                printf("Wrong parameters! Usage: -s <int> path \n");
                exit(EXIT_FAILURE);
        }
    }

    if (*sig_num == -1)
    {
        printf("You need to specify -s parameter!\n");
        exit(EXIT_FAILURE);
    }

    *path = (argv)[optind];
}