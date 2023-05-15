// WRITTEN BY: KADARU JASHWANTH REDDY           IMT2021095
// OS MINI PROJECT  - ONLINE RETAIL STORE
#ifndef CLIENT_H

#define CLIENT_H
#include "app.h"

struct Clientdetails{
    int clientFD;
    // struct Product item[1000];
    char username[100];
    // int cartsize;
};

void adminPortal(int clientFD);

void clientPortal(int clientFD);

int connectToServer();

void displayProducts(int clientFD);

#endif 