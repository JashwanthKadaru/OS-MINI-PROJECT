// WRITTEN BY: KADARU JASHWANTH REDDY           IMT2021095
// OS MINI PROJECT  - ONLINE RETAIL STORE
#include<stdio.h>
#include<string.h>
#include "server.h"
#include "app.h"
struct DB_Handler *DB;

void signal_handler(){
    printf("Interrupt signal caught\n");
    close(DB->dbfd);
    close(DB->idx_fd);
    close(DB->cart_fd);
    exit(0);
}

// create db
void creatDB(){

    int fd=open("store.dat",O_CREAT|O_RDWR,0666);
    if(fd>0){
        DB->dbfd=fd;
    } 

    int fd1=open("index.dat", O_CREAT|O_RDWR, 0666);
    if(fd1>0){
        DB->idx_fd=fd1;
        int arr[MAX_SIZE];
        for(int i=0;i<MAX_SIZE;i++) arr[i]=-1;
        read(DB->idx_fd,arr,MAX_SIZE);
        memcpy(DB->deleted_array,arr,sizeof(DB->deleted_array));
    } 

    int fd2=open("cart.dat", O_CREAT|O_RDWR, 0666);
    if(fd2>0){
        DB->cart_fd=fd2;
    } 

    DB->productscount=lseek(DB->dbfd,0,SEEK_END)/sizeof(struct Product);

}

int addProduct(struct Product p){
    
    p.ProductId=DB->productscount+1;

    lseek(DB->dbfd,0,SEEK_END);
    int ret=write(DB->dbfd,&p,sizeof(struct Product));
    
    if(ret>0){
        // DB->productscount+=1;
        return 0;
    };
    return 1;
}

int removeProduct(int id){//Add existing error handling
    // int i=0;
    if(id>=DB->productscount) return 1;
    // while(DB->DeletedP[i]!=-1 && i<MAX_SIZE) i++;


    if(id<MAX_SIZE){
        
        DB->deleted_array[id]=id;
        lseek(DB->idx_fd,0,SEEK_SET);
        
        int w=write(DB->idx_fd,&DB->deleted_array,sizeof(DB->deleted_array));
          
        if(w>0){
            return 0;
        }
    } 

    
    return 1;

}

int updateProductQuantity(struct Product p,int option){

    lseek(DB->dbfd,0,SEEK_SET);
    struct Product fp;
    
    if(p.ProductId > DB->productscount) return 1; //have to update when server starts
    if(DB->deleted_array[p.ProductId]>0) return 1;

    struct flock lock;
    if(!option){
       
        memset (&lock, 0, sizeof(lock));
        lock.l_type=F_WRLCK;
        lock.l_start=(p.ProductId-1)*sizeof(struct Product);
        lock.l_whence=SEEK_SET;
        lock.l_len=sizeof(struct Product);
        lock.l_pid=getpid();
        fcntl(DB->dbfd, F_SETLKW, &lock);
    }

    lseek(DB->dbfd, (p.ProductId-1)*sizeof(struct Product),SEEK_SET);

    read(DB->dbfd,&fp,sizeof(struct Product));
    p.cost=fp.cost;
    strcpy(p.ProductName,fp.ProductName);

    if(option==1){
        if(p.quantity<=fp.quantity){
            p.quantity=fp.quantity-p.quantity;
        }
    }
    lseek(DB->dbfd, (-1)*sizeof(struct Product),SEEK_CUR);
    int x=write(DB->dbfd,&p,sizeof(struct Product));

    if(!option){
        lock.l_type = F_UNLCK;
        fcntl(DB->dbfd, F_SETLKW, &lock); 
    }
    
    if(x>0) return 0;
    return 1;

}

int updateProductPrice(struct Product p){
    lseek(DB->dbfd,0,SEEK_SET);
    struct Product fp;
    int i=0;
    if(p.ProductId > DB->productscount) return 1;
    // while(DB->DeletedP[i]!=-1){
        if(DB->deleted_array[p.ProductId]>0) return 1;
        // i++;
    // } 
    struct flock lock;
    memset (&lock, 0, sizeof(lock));
    lock.l_type=F_WRLCK;
    lock.l_start=(p.ProductId-1)*sizeof(struct Product);
    lock.l_whence=SEEK_SET;
    lock.l_len=sizeof(struct Product);

    fcntl(DB->dbfd, F_SETLKW, &lock);

    lseek(DB->dbfd, (p.ProductId-1)*sizeof(struct Product),SEEK_SET);
    read(DB->dbfd,&fp,sizeof(struct Product));
    p.quantity=fp.quantity;
    strcpy(p.ProductName,fp.ProductName);
    lseek(DB->dbfd, (-1)*sizeof(struct Product),SEEK_CUR);
    int x=write(DB->dbfd,&p,sizeof(struct Product));

    lock.l_type = F_UNLCK;
    fcntl(DB->dbfd, F_SETLKW, &lock);
    if(x>0) return 0;
    return 1;
}

struct Product* displayProducts(int fd, int opt, struct Cartitem items[]){
 
    lseek(fd,0,SEEK_SET);

    int i=0;//number of products

    struct Product p;
    struct Product *products=(struct Product *)malloc(MAX_SIZE*(sizeof(struct Product)));
    while(read(fd,&p,sizeof(struct Product))>0){ 
        if(DB->deleted_array[p.ProductId]==-1){
            products[i]=p;
            if(i<MAX_SIZE) i++;
            else break;
        }
    }

    if(opt==1){
        
        struct Product *carts=(struct Product *)malloc(MAX_SIZE*(sizeof(struct Product)));
        int x=0;
        for(int index=0;index<i;index++){
            if(items[products[index].ProductId].ProductId>0){
                carts[x].cost=products[index].cost;
                carts[x].ProductId=products[index].ProductId;
                strcpy(carts[x].ProductName,products[index].ProductName);
                carts[x].quantity=items[products[index].ProductId].quantity;
                x++;
            } 
        }
        return carts;
    }

    return products;
}

int initiate_Server(void){
    struct sockaddr_in server_sock;
    int addrlen=sizeof(server_sock);
    int sd = socket (AF_INET, SOCK_STREAM, 0);
    server_sock.sin_family = AF_INET;
    server_sock.sin_addr.s_addr =INADDR_ANY;
    server_sock.sin_port = htons (8080);

    if(sd==-1){
        write(1,"The Server has failed to start\n",32);
        exit(0);
    }
    bind (sd, (struct sockaddr*)&server_sock, sizeof (server_sock));

    creatDB();

    write(1,"The Server is running successfully\n", 36);
    return sd;
}

void server_routine(void *arg){

    /* clientfd -> server->client socket*/
    int *clientfd=(int *) arg;
    struct Cart session={0};
    
    /* start reading requests from client and writing replies to client via clientfd*/
    while(1){

        char buffer[MAX_SIZE]={0};
        read(*clientfd,buffer,MAX_SIZE);
        struct Product p={0};
        write(1,buffer,strlen(buffer));
        lseek(DB->dbfd,0,SEEK_SET);

        DB->productscount=lseek(DB->dbfd,0,SEEK_END)/sizeof(struct Product);
        
        write(1,"\n", 1);


        if(strcmp(buffer,"add Product")==0){
            send(*clientfd, "Give Product Details", 19, 0);
            read(*clientfd,&p,sizeof(struct Product));
            if(!addProduct(p)) send(*clientfd,"Successfully added to Database\n", 32,0);

            else send(*clientfd, "Failed to add product to Database!\n",36,0);
        }

        else if(strcmp(buffer, "delete Product")==0){
            send(*clientfd, "Give Product ID", 14, 0);

            int pid;
            read(*clientfd, &pid, sizeof(int));
            write(1,&pid,4);
            if(!removeProduct(pid)) send(*clientfd,"Product successfully deleted from Database\n", 44,0);

            else send(*clientfd, "Failed to delete product from Database!\n",41,0);
        }

        else if(strcmp(buffer, "update Quantity")==0){

            send(*clientfd, "get Updated Product Details", 27, 0);
            read(*clientfd,&p,sizeof(struct Product));
            if(!updateProductQuantity(p,0)) send(*clientfd,"Successfully updated product in Database\n", 42,0);

            else send(*clientfd, "Failed to update product in Database!\n",39,0);

        }

        else if(strcmp(buffer, "update Product")==0){
            send(*clientfd, "get Updated Product Details", 27, 0);
            read(*clientfd,&p,sizeof(struct Product));

            if(!updateProductPrice(p)) send(*clientfd,"Successfully updated product in Database\n", 42,0);

            else send(*clientfd, "Failed to update product in Database\n",38,0);
        }

        else if(strcmp(buffer, "display Product")==0){

            struct Product *ps=(struct Product *)malloc(sizeof(struct Product)*MAX_SIZE);
            ps=displayProducts(DB->dbfd, 0,(void *)0);
            int i=0;
            send(*clientfd,ps,sizeof(struct Product)*MAX_SIZE,0);
        }

        else if(strcmp(buffer, "display User Cart")==0){
            lseek(DB->cart_fd,(session.userid-1)*sizeof(struct Cart),SEEK_SET);
            read(DB->cart_fd,&session,sizeof(struct Cart));
            struct Product *ps=(struct Product *)malloc(sizeof(struct Product)*MAX_SIZE);

            ps=displayProducts(DB->dbfd,1,session.items);

            send(*clientfd,ps,sizeof(struct Product)*MAX_SIZE,0);

        }

        else if(strcmp(buffer,"Buy")==0){
            struct Product *ps=(struct Product *)malloc(sizeof(struct Product)*MAX_SIZE);
            int quantity[MAX_SIZE];
            lseek(DB->cart_fd,(session.userid-1)*(sizeof(struct Cart)),SEEK_SET);
            

            ps=displayProducts(DB->dbfd,1,session.items);

            struct flock lock[MAX_SIZE];
            for(int i=0;i<MAX_SIZE;i++){
                if(ps[i].ProductId>0){
                    int q=ps[i].ProductId;
                    memset (&lock[q], 0, sizeof(lock[q]));
                    lock[q].l_type=F_WRLCK;
                    lock[q].l_whence=SEEK_SET;
                    lock[q].l_start=(q-1)*sizeof(struct Product);
                    lock[q].l_len=sizeof(struct Product);
                    lock[q].l_pid=getpid();
                    fcntl(DB->dbfd, F_SETLKW, &lock[q]);
                } 
            }

            ps=displayProducts(DB->dbfd, 0,(void *)0);

            for(int i=0;i<MAX_SIZE;i++){
                if(ps[i].ProductId<=MAX_SIZE) quantity[ps[i].ProductId]=ps[i].quantity;
            }
            send(*clientfd,quantity,sizeof(int)*MAX_SIZE,0);

            ps=displayProducts(DB->dbfd,1,session.items);
            send(*clientfd,ps,sizeof(struct Product)*MAX_SIZE,0);

            char buf[100];
            read(*clientfd,buf,100);
            
            if(strcmp(buf,"ResetCart")==0){
                //read from file

                for(int i=0;i<MAX_SIZE;i++){
                    if(ps[i].ProductId>0){
                        int q=ps[i].ProductId;
                        if(ps[i].quantity<=quantity[ps[i].ProductId]){

                            int x=updateProductQuantity(ps[i],1);
                            
                            if(!x){

                                session.items[q].ProductId=-1;
                                session.items[q].quantity=0;
                                lseek(DB->cart_fd,(session.userid-1)*(sizeof(struct Cart)),SEEK_SET);
                                write(DB->cart_fd,&session, sizeof(struct Cart));
                            }
                        } 
                        lock[q].l_type = F_UNLCK;
                        fcntl(DB->dbfd, F_SETLKW, &lock[q]); 
                    }
                }
                
                send(*clientfd,"Payment is successful, the receipt is available in pwd.\nYour cart is updated\n",78,0);
            }

            else if(strcmp(buf,"Failed")==0){
                for(int i=0;i<MAX_SIZE;i++){
                    if(ps[i].ProductId>0){
                        int q=ps[i].ProductId;
                        lock[q].l_type = F_UNLCK;
                        fcntl(DB->dbfd, F_SETLKW, &lock[q]); 
                    } 
                }
            }
        }

        else if(strcmp(buffer, "update User Cart")==0){
        
            lseek(DB->cart_fd,(session.userid-1)*(sizeof(struct Cart)),SEEK_SET);

            char choice[50];
            read(*clientfd, choice,20);
            struct Cartitem item;

            if(strcmp(choice,"add User Cart")==0){

                read(*clientfd,&item,sizeof(struct Cartitem));
                if(item.ProductId<=MAX_SIZE){
                    if(DB->deleted_array[item.ProductId]==-1 && session.items[item.ProductId].ProductId<=0 && item.ProductId<=DB->productscount)
                    {
                        lseek(DB->dbfd,(item.ProductId-1)*sizeof(struct Product),SEEK_SET);
                        read(DB->dbfd,&p,sizeof(struct Product));
                        if(p.quantity>=item.quantity){
                            session.items[item.ProductId].ProductId=item.ProductId;
                            session.items[item.ProductId].quantity=item.quantity;

                            write(DB->cart_fd,&session, sizeof(struct Cart)); //check if quantity is available
                            send(*clientfd, "Added to Cart!!\n",17,0);
                        }
                        else send(*clientfd, "The quantity requested isn't available!!\n",42,0);

                    } 
                    else send(*clientfd, "This Product doesn't exist or is already in cart!\n",51,0);
                }
                else send(*clientfd, "Invalid Product Id sent!!\n",27,0);

            }

            else if(strcmp(choice,"delete User Cart")==0){

                read(*clientfd,&item,sizeof(struct Cartitem));
                if(item.ProductId<=MAX_SIZE){
                    if(DB->deleted_array[item.ProductId]==-1 && session.items[item.ProductId].ProductId >0 && item.ProductId<=DB->productscount){
                        
                        session.items[item.ProductId].ProductId=-1;
                        session.items[item.ProductId].quantity=0;
                        write(DB->cart_fd,&session, sizeof(struct Cart)); 
                        send(*clientfd, "Deleted from Cart!!\n",21,0);

                    } 
                    else send(*clientfd, "This Product doesn't exist or is already deleted!\n",51,0);
                }
                else send(*clientfd, "Invalid Product Id sent!!\n",27,0);
            }

            else if(strcmp(choice,"update User Cart")==0){

                read(*clientfd,&item,sizeof(struct Cartitem));
                if(item.ProductId<=MAX_SIZE){
                    if(DB->deleted_array[item.ProductId]==-1 && session.items[item.ProductId].ProductId>0 && item.ProductId<=DB->productscount)
                    {
                        lseek(DB->dbfd,(item.ProductId-1)*sizeof(struct Product),SEEK_SET);
                        read(DB->dbfd,&p,sizeof(struct Product));

                        if(p.quantity>=item.quantity){
                            session.items[item.ProductId].ProductId=item.ProductId;
                            session.items[item.ProductId].quantity=item.quantity;

                            write(DB->cart_fd, &session, sizeof(struct Cart)); //check if quantity is available
                            send(*clientfd, "Updated the Cart!!\n",20,0);
                        }
                        else send(*clientfd, "The quantity requested isn't available!!\n",42,0);

                    } 
                    else send(*clientfd, "This Product doesn't exist or is Deleted!\n",43,0);
                }
                else send(*clientfd, "Invalid Product Id sent!!\n",27,0);
            }

        }
        
        else if(strcmp(buffer, "Login")==0){

            send(*clientfd,"Enter Username: ", 17, 0);
            char response[100]={0};
            read(*clientfd, response,100);
            lseek(DB->cart_fd,0,SEEK_SET);
            struct Cart user={0};//can have error
            strcpy(session.username,response);
            int check=0,newuserid=0;
            while(read(DB->cart_fd,&user,sizeof(struct Cart))>0){
                if(strcmp(user.username,response)==0) {
                    session.userid=user.userid;
                    for(int i=0;i<MAX_SIZE;i++){
                        session.items->ProductId=user.items->ProductId;
                        session.items->quantity=user.items->quantity;
                    }
                    // session.items=user.items;
                    check=1;
                    break;
                }
                newuserid=max(newuserid,user.userid); 
            }
            if(check==0){
                session.userid=newuserid+1;
                memset(session.items,-1,sizeof(struct Cartitem)*MAX_SIZE);
                write(DB->cart_fd,&session,sizeof(struct Cart));
            } 

        }
        else if(strcmp(buffer, "END")==0) break;

    }
    return;
    
}



int main()
{
    /* Creating a variable of struct DB_Handler*/
    DB=(struct DB_Handler *)malloc(sizeof(struct DB_Handler));
    signal(SIGINT, &signal_handler);

    /* Creating server socket and creating db files.*/
    int serverfd = initiate_Server();
    
    /* */
    int i=0;
    struct sockaddr_in sockadd;
    int len=sizeof(sockadd);

    /* Listening ...*/
    listen (serverfd, 100);

    /* Creating a new child process for each client request.*/
    while(1){
        int clientfd = accept (serverfd, (struct sockaddr*)&sockadd, (socklen_t*)&len);
        if(clientfd==-1) break;
        
        if(!fork()){
            server_routine(&clientfd);
        }
        else close(clientfd);

    }

    /* closes server socket.*/
    shutdown(serverfd,SHUT_RDWR);
    return 0;
}

