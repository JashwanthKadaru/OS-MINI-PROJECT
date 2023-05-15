// WRITTEN BY: KADARU JASHWANTH REDDY           IMT2021095
// OS MINI PROJECT  - ONLINE RETAIL STORE

#ifndef SERVER_H

#define SERVER_H
#include "app.h"
#define max(a,b) a>b ? a: b;

struct DB_Handler{
    int dbfd;
    int idx_fd;
    int productscount;
    int deleted_array[1000];
    int cart_fd;
};

struct Cart{ //need to chage to products
    char username[100];
    struct Cartitem items[1000];
    int userid;
};

int initiate_Server(void);

void server_routine(void *arg);

int addProduct(struct Product p);

int updateProductQuantity(struct Product p, int option);

int updateProductPrice(struct Product p);

struct Product* displayProducts(int fd,int opt, struct Cartitem items[]);

void creatDB();

int removeProduct(int id);

#endif

