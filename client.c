// WRITTEN BY: KADARU JASHWANTH REDDY           IMT2021095
// OS MINI PROJECT  - ONLINE RETAIL STORE

#include<stdio.h>
#include<string.h>
#include "client.h"
#include "app.h"

struct Clientdetails cld;

void signal_handler(){
    printf("Interrupt Signal Caught\n");
    send(cld.clientFD,"END", 3,0);
    exit(0);
}

//Will implement file locking later
void adminPortal(int clientFD){
    write(1,"Welcome Admin!\n",16);

    while(1){ 

        write(1,"\n1)Add a product\n2)Delete a product\n3)Update the quantity of a product\n4)Update the price of a product\n5)Display All Products\n6)Admin Exit\n\n",141);

        write(1, "Enter an option: ", 18);
        int choice=0;
        char response[MAX_SIZE]={0};
        scanf("%d", &choice);

        if(choice==1){
            int x=send(clientFD, "add Product", 11,0);

            if(x>0){
                read(clientFD, response, 100);
                struct product p;
                write(1, response, strlen(response));

                write(1, "\nEnter Product Name: ",22);
                char name[100];
                getchar();
                scanf("%[^\n]s",name);
               
                // scanf("\n");
                // fgets(name, 100, stdin);
                strcpy(p.productName,name);

                write(1, "Enter Product Cost: ",20);
                scanf("%d", &p.price_per_piece);

                write(1, "Enter Product Quantity: ",24);
                scanf("%d", &p.quantity);

                send(clientFD,&p,sizeof(struct product), 0);

                memset(response, '\0', sizeof(response));
                read(clientFD,response, 100);
                write(1,response,strlen(response));

            }
            else write(1,"Couldn't Connect to Server\n",28);

        }
        else if(choice==2){ 
            int x=send(clientFD, "delete Product", 14,0);

            if(x>0){
                read(clientFD, response, 100);
                write(1, response, strlen(response));

                write(1, "\nEnter Product ID: ",20);
                int id=0;
                scanf("%d", &id);
                send(clientFD,&id,sizeof(int), 0);

                memset(response, '\0', sizeof(response));
                read(clientFD,response, 100);
                write(1,response,strlen(response));

            }
            else write(1,"Couldn't Connect to Server\n",28);

        }
        else if(choice==3){ //just need to overwrite at the correct location , using the product id
            int x=send(clientFD, "update Quantity", 15,0);
            if(x>0){
                struct product p;
                read(clientFD, response, 100);
                write(1, response, strlen(response));

                write(1, "\nEnter Product ID: ",20);
                int id=0,quantityP;
                scanf("%d", &id);

                write(1, "\nEnter New Quantity: ",20);
                scanf("%d", &quantityP);

                p.productId=id;
                p.quantity=quantityP;
                send(clientFD,&p,sizeof(struct product), 0);

                memset(response, '\0', sizeof(response));
                read(clientFD,response, 100);
                write(1,response,strlen(response));

            }
            else write(1,"Couldn't Connect to Server\n",28);
        }
        else if(choice==4){
            int x=send(clientFD, "update Product", 8,0);

            if(x>0){
                struct product p;
                read(clientFD, response, 100);
                write(1, response, strlen(response));

                write(1, "\nEnter Product ID: ",20);
                int id=0,cost;
                scanf("%d", &id);

                write(1, "\nEnter New Cost: ",18);
                scanf("%d", &cost);

                p.productId=id;
                p.price_per_piece=cost;
                send(clientFD,&p,sizeof(struct product), 0);

                memset(response, '\0', sizeof(response));
                read(clientFD,response, 100);
                write(1,response,strlen(response));

            }
            else write(1,"Couldn't Connect to Server\n",28);
        }
        else if(choice==5){
            displayProducts(clientFD);
        }
        else if(choice>=6){
            break;
        } 


        write(1,"------------------------------------------------------------------------\n",74);
    }
}

void clientPortal(int clientFD){
    write(1,"Welcome Client\n",20);

    while(1){
        write(1,"---------------------------------------------------------------------------------\n", 83);
        write(1,"\n1)Display All Products\t2)Display Cart\t3)Update Cart\t4)Buy\t5)Client Exit\t\n",89);
        write(1,"---------------------------------------------------------------------------------\n", 83);

        write(1, "Enter your Choice: ", 18);
        int choice=0;
        scanf("%d", &choice);
        char response[MAX_SIZE];
        if(choice==1){
            displayProducts(clientFD);
        }
        else if(choice==2){
            
            write(1,"Client Cart\n",13);

            int x=send(clientFD, "display User Cart", 17,0);
            if(x>0){
                
                memset(response, '\0', sizeof(response));
                
                struct product *p=(struct product *)malloc(sizeof(struct product)*MAX_SIZE);
                read(clientFD, p,sizeof(struct product)*MAX_SIZE);
                write(1,"-------------------------------------------------------------------------\n",75);
        // write(1, "P_ID, P_Name, Cost, Quantity\n",30);
                write(1,"| Product ID\t| Product Name\t\t| Cost\t  | Quantity | SubTotal\t\t|\n",52);
                write(1,"-------------------------------------------------------------------------\n",75);
                int i=0;
                int totalcost=0;
                while(p[i].productId!=0 && i<MAX_SIZE){
                    printf("| %d\t| %-22s| %-8d| %-9d| %-17d|\n", p[i].productId, p[i].productName, p[i].price_per_piece, p[i].quantity, p[i].price_per_piece*p[i].quantity);
                    totalcost+=p[i].quantity*p[i].price_per_piece;
                    write(1,"-------------------------------------------------------------------------\n",75);
                    i++;
                }

                printf("\nTotal Cart Size: %d\n", i);
                printf("Total Cost: %d\n\n", totalcost);
            }
            else write(1,"Couldn't Connect to Server\n",28);

        }
        else if(choice==3){
            send(clientFD,"update User Cart",16,0);
            int choice2;
            write(1,"-----------------------------------------------------------------------------------------------------\n",103);
            write(1,"1)Add to Cart\t2)Delete from Cart\t3)Update Cart\nEnter choice: ", 64);
            write(1,"-----------------------------------------------------------------------------------------------------\n",103);
            scanf("%d", &choice2);
            memset(response, '\0', sizeof(response));

            if(choice2==1){
                send(clientFD,"add User Cart",13,0);
                write(1,"Enter ProductId: ", 18);
                struct cart_item select;
                scanf("%d", &select.productId);
                write(1,"Enter quantity: ", 17);
                scanf("%d", &select.quantity);
                send(clientFD,&select,sizeof(struct cart_item),0);
                read(clientFD,&response,100);
                write(1,&response,strlen(response));
            }

            else if(choice2==2){
                send(clientFD,"delete User Cart",16,0);
                write(1,"Enter ProductId: ", 18);
                struct cart_item select;
                scanf("%d", &select.productId);
                send(clientFD,&select,sizeof(struct cart_item),0);
                read(clientFD,&response,100);
                write(1,&response,strlen(response));

            }

            else if(choice2==3){
                send(clientFD,"update User Cart",16,0);
                write(1,"Enter ProductId: ", 18);
                struct cart_item select;
                scanf("%d", &select.productId);
                write(1,"Enter quantity: ", 17);
                scanf("%d", &select.quantity);
                send(clientFD,&select,sizeof(struct cart_item),0);
                read(clientFD,&response,100);
                write(1,&response,strlen(response));
            }
            else send(clientFD,"Failed",7,0);

        }
        else if(choice==4){
            
            memset(response, '\0', sizeof(response));
            send(clientFD, "Buy", 3,0);
            
            int quantity[MAX_SIZE],invalid[100];
            struct product *cart=(struct product *)malloc(sizeof(struct product)*MAX_SIZE);
            read(clientFD, quantity,sizeof(int)*MAX_SIZE);
            read(clientFD,cart,sizeof(struct product)*MAX_SIZE);
            
            write(1,"\nList of Items in Cart, available for Purchase : \n",52);
            write(1,"---------------------------------------------------------------------------------------------\n",93);
        // write(1, "P_ID, P_Name, Cost, Quantity\n",30);
            write(1,"| Product Id\t| Product Name\t\t| Cost\t  | Quantity | SubTotal\t\t|\n",52);
            write(1,"---------------------------------------------------------------------------------------------\n",93);
            int totalcost=0,size=0,esize=0;
            for(int i=0;i<MAX_SIZE;i++){
                if(cart[i].productId!=0){
                    if(cart[i].quantity<=quantity[cart[i].productId]){

                        printf("| %d\t| %-22s| %-8d| %-9d| %-17d|\n", cart[i].productId, cart[i].productName, cart[i].price_per_piece, cart[i].quantity, cart[i].price_per_piece*cart[i].quantity);

                        write(1,"---------------------------------------------------------------------------------------------\n",93);
                        totalcost+=cart[i].quantity*cart[i].price_per_piece;
                        size++;
                    }
                    else invalid[esize++]=i;
                } 
            }
            if(esize>0){
                write(1,"The following products can't be bought as there is not enough stock.\n",70);
                for(int i=0;i<esize;i++){
                    printf("Product Name: %s , quantity asked: %d, quBuyantity available: %d\n", cart[invalid[i]].productName, cart[invalid[i]].quantity, quantity[cart[invalid[i]].productId]);
                }
            }

            printf("\nTotal Cart Size: %d\n", size);
            printf("Total Cost: %d\n", totalcost);

            write(1,"enter the total amount to verify payment\nEnter amount: ",56);

            int amount=0;
            scanf("%d",&amount);

            if(amount==totalcost){
                send(clientFD,"ResetCart",10,0);

                read(clientFD,response,100);
                write(1,response,strlen(response));
                time_t t1;
                srand((unsigned)time(&t1));
                char num[10];
                sprintf(num,"%d",rand()%100);
                char filename[100]="log";
                strcat(filename,cld.username);
                strcat(filename,num);
                strcat(filename,".txt");
                int logfd=open(filename,O_CREAT|O_WRONLY,0666);
                write(logfd,"Receipt\n---------------------------------------------------------------------\n",78);

                write(logfd,"| P_ID\t| P_Name\t\t\t\t| Cost\t  | Quantity | SubTotal\t\t|\n",53);
                write(logfd,"---------------------------------------------------------------------\n",70);
                char output[200], result[200];
                for(int i=0;i<MAX_SIZE;i++){
                    if(cart[i].productId!=0){
                        if(cart[i].quantity<=quantity[cart[i].productId]){
                            
                            sprintf(output, "| %-5d\t| %-22s| %-8d| %-9d| %-13d|\n", cart[i].productId, cart[i].productName, cart[i].price_per_piece, cart[i].quantity, cart[i].price_per_piece*cart[i].quantity);
                            write(logfd,output,strlen(output));
                            write(logfd,"---------------------------------------------------------------------\n",70);
                        }
                    } 
                }
                sprintf(result,"\nTotal Cart Size: %d\nTotal Cost: %d\n", size,totalcost);
                write(logfd,result,strlen(result));
                
            }
            else {
                send(clientFD,"Failed",8,0);
                write(1,"Payment failed. Sorry, Try again!!\n", 36);
            } 
            // make log file
        }
        else if(choice>=5)break;

        write(1,"------------------------------------------------------------------------\n",74);
    }

    return ;
}

int connectToServer(){

    write(1, "Connecting to server...\n", 25);
    write(1, "Please wait....\n", 16);
    
    /* CONNECTING SERVER SOCKET AND PORT ADDR*/
    struct sockaddr_in server_sock;
    int sd=socket(AF_INET,SOCK_STREAM,0);
    server_sock.sin_family=AF_INET;
    server_sock.sin_addr.s_addr=INADDR_ANY;
    server_sock.sin_port=htons(8080);
    int p=connect(sd,(struct sockaddr*)&server_sock,sizeof(server_sock));
    if(!p){
        write(1,"Successfully connected to server\n", 34); 
        return sd;
    }
    else{ 
        write(1, "Connection Failed. Hint: Check Server\n", 19);
        exit(0);
    }
    return 0;
}

void displayProducts(int clientFD){
    int x=send(clientFD, "display User Cart", 17,0);
    if(x>0){
        struct product *p=(struct product *)malloc(sizeof(struct product)*MAX_SIZE);
        read(clientFD, p,sizeof(struct product)*MAX_SIZE);
        write(1,"------------------------------------------------------------------\n",68);
        // write(1, "P_ID, P_Name, Cost, Quantity\n",30);
        write(1,"| Product Id\t| Product Name\t\t| Price\t  | Quantity |\n",57);
        write(1,"------------------------------------------------------------------\n",68);
        int i=0;
        while(p[i].productId!=0 && i<MAX_SIZE){
            printf("| %d\t| %-22s| %-8d| %-9d|\n", p[i].productId, p[i].productName, p[i].price_per_piece, p[i].quantity);
            i++;
            write(1,"--------------------------------------------------------------\n",68);
        }
        write(1,"\n",2);
    }
    else write(1,"Error. Couldn't Connect to Server.\n",36);
}

int main(){
    
    write(1,"Welcome, Online Retail Store\n", 30);

    int clientFD=connectToServer();
    cld.clientFD=clientFD;
    signal(SIGINT, &signal_handler);

    while(1){
        write(1,"------------------------------------------------------------------------\n",74);
        write(1,"\n1)Admin\t\t2)Customer\t\t3)Exit\n\n", 32);
        write(1,"Select an option: ", 19);
        int choices=0;
        scanf("%d", &choices);
        write(1,"------------------------------------------------------------------------\n",74);
        
        if(choices==1){
            adminPortal(cld.clientFD);
        }
        else if(choices==2){
            send(cld.clientFD,"Login", 5, 0);
            char response[100];
            read(cld.clientFD,response,100);
            write(1,response,strlen(response));
            char username[100];
            getchar();
            scanf("%[^\n]s",username);
            getchar();
            strcpy(cld.username,username);
            send(cld.clientFD,cld.username,strlen(cld.username),0);
            clientPortal(cld.clientFD);
        }
        else{
            send(cld.clientFD,"END", 3,0);
            shutdown(cld.clientFD, O_RDWR);
            break;
            
        }
    }
    return 0;
}

