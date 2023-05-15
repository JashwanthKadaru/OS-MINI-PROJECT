// WRITTEN BY: KADARU JASHWANTH REDDY           IMT2021095
// OS MINI PROJECT  - ONLINE RETAIL STORE

#ifndef APP_H
#define APP_H
#include <netinet/in.h>
#include <sys/types.h>        
#include <sys/socket.h>
#include <arpa/inet.h>
#include<unistd.h>
#include<stdlib.h>
#include<pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include<sys/time.h>
#include <signal.h>
#include <time.h>

#define MAX_SIZE 1000

struct cart_item{
    int productId;
    int quantity;
};
struct product{
    int productId;
    char productName[60];
    int quantity;
    int price_per_piece;
};

#endif 
