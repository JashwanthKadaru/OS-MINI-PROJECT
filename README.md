# OS-MINI-PROJECT

KADARU JASHWANTH REDDY
IMT2021095

OS MINI PROJECT - ONLINE RETAIL STORE USING SOCKET PROGRAMMING

Read the report for instructions. The same are wtitten here:
INSTRUCTIONS:
Use the commands cc server.c app.h server.h -o server    and   cc client.c client.h app.h -o client, and run the ./server & ./client in separate terminal windows. 
The client application has 2 options (apart from exit) : Client Portal & Admin Portal.
Admin Portal has the options to add, delete, update the product prices and quantities in the store.
Client Portal is a customer portal and a client can add, remove, update products to his cart. Also, at the end of the transaction client can chose the Buy option, to buy items in the cart.
Small text messages are sent between client and server porcesses through sockets. These messages are for communcating and requesting the server & also to send response to the client.
For example, if the client wants to add a product to his/her cart, the client will send “add User Cart” message to server and the server will perform the corresponding operation onlyon reception of the exact same message. Without sending and recieving reply from server, the client cannot move from one option to other and cannot process requests.
The files store.dat, index.dat, cart.dat are used for maintaining the DB of products. 
Store.dat stores the main products list. Index.dat is for deleting purpose. The products which are removed by user are marked as -1 and stored in index.dat. This way we don’t have to bother about the data of that product residing in store.dat. If it is marked as -1 in index.dat, then that data is invalid.
cart .dat maintains user cart details, the products they added to cart and their userid.
The display is done in a Tabular format. For empty tables, just the table headings are displayed.
In the server side the server prints the messages it recieves from clients one after the other in that order. (Order might get jumble because multiple child processes are writing to console. This is for server side.)
