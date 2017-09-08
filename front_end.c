#include <stdio.h> 
#include <string.h>   
#include <stdlib.h> 
#include <errno.h> 
#include <unistd.h>   
#include <arpa/inet.h>   
#include <sys/types.h> 
#include <sys/socket.h> 
#include <netinet/in.h> 
#include <sys/time.h> 
#include <pthread.h>
#include <netdb.h>

//constants used in the program
#define TRUE   1 
#define FALSE  0 
#define PORT 8888
#define PORT1 8080  
#define BUFFER_SIZE 1024
#define MAX 100000
#define HOST "localhost"

//set of all usernames and passwords
char usernames[][BUFFER_SIZE] = {"debanjan","mamta","dhananjay","test"};
char passwd[][BUFFER_SIZE] = {"abcd","1234","0","test"};

//set of all products and their counts
char product_names[][BUFFER_SIZE] = {"Pen","Book","Notebook","Mobile"};
int product_count[MAX] = {1000,1000,1000,1000};
int product_price[MAX] = {10,200,30,10000};

struct arg_struct{
  int back_end_sockfd;
  int newsockfd;
};


//error meassage printing funtion
void error(char *msg)
{
    perror(msg);
    exit(1);
}


//checks if a given username and password matches or not
int _is_authentic(char *username_buffer,char *passwd_buffer){
    int num_of_usernames = sizeof(usernames)/sizeof(usernames[0]);
    int i;
    for(i=0;i<num_of_usernames;i++){
        if(strcmp(usernames[i],username_buffer)==0){
            if(strcmp(passwd_buffer,passwd[i])==0){
                return 1;
            }
            return 0;
        }
     }
     return 0;
}


//checks whether an user is authentic or not
int authentication(int newsockfd){
    int n,n_username,n_passwd,authentic;
    char username_buffer[BUFFER_SIZE],passwd_buffer[BUFFER_SIZE];
    char message[BUFFER_SIZE] = "1";
    bzero(username_buffer,BUFFER_SIZE);
    n_username = read(newsockfd,username_buffer,BUFFER_SIZE-1);
    if (n_username < 0) error("ERROR reading from socket");
    n = write(newsockfd,"1",1);
    bzero(passwd_buffer,BUFFER_SIZE);
    n_passwd = read(newsockfd,passwd_buffer,BUFFER_SIZE-1);
    if(n_passwd<0) error("ERROR reading from socket");
    authentic = _is_authentic(username_buffer,passwd_buffer);
    if(!authentic){
        puts("Invalid user/password\n");
        printf("User: %s , Password: %s is restricted.\n",username_buffer,passwd_buffer);
        printf("______________________________________________________________________\n");
        strcpy(message,"0");
        n = write(newsockfd,message,strlen(message));
     }
     else{
        printf("%s is allowed---",username_buffer);
        printf("%s is password.\n",passwd_buffer);
        strcpy(message,"1");
        n = write(newsockfd,message,strlen(message));
     }
     if(n < 0) error("ERROR writing to socket");
     return authentic;
}

int ___only_update_db(char *msg_db, int back_end_sock){
  int n_msg_db;
  strcat(msg_db," 1");
  n_msg_db = write(back_end_sock,msg_db,strlen(msg_db));
  if (n_msg_db < 0){ error("ERROR writing to socket");}
  bzero(msg_db,BUFFER_SIZE-1);
  n_msg_db = read(back_end_sock,msg_db,BUFFER_SIZE-1);
  if(n_msg_db<0){ error("ERROR writing to socket");}
  //printf("%s",msg_db);
  if(strcmp(msg_db,"1")==0) return 1;
  else return 0;
  //printf("%s\n",msg_db);
  
}

char *__decr_count_update_db(char *item,char *cnt,int index,char *input,int back_end_sock){
  int amount = atoi(cnt),status;
  int cost = amount*product_price[index];
  char message[BUFFER_SIZE],msg_db[BUFFER_SIZE];
  bzero(msg_db,BUFFER_SIZE-1);
  sprintf(msg_db,"%s %s",item,cnt);
  if(product_count[index]>=amount){
    product_count[index] -= amount;
    sprintf(message,"You have to pay %d to buy %d %s(s).",cost,amount,item);
    status = ___only_update_db(msg_db,back_end_sock);
    strcpy(input,message);
    if(!status){
      strcpy(input,"ERROR occured, please try again.");
      product_count[index] += amount;
    }
  }
  else{
    sprintf(message,"Sorry, we have only %d %s.",product_count[index],item);
    strcpy(input,message);
  }
  printf("User has requested %d %s(s)\n",amount,product_names[index]);
  printf("After processing the request remaining %ss are = %d\n",product_names[index],product_count[index]);
  printf("______________________________________________________________________\n");
  return input;
}

char *__call_backend(char *item,char *cnt, char *input,int back_end_sock){
  int n,n1;
  char msg_db[BUFFER_SIZE],message[BUFFER_SIZE],item_name[BUFFER_SIZE];
  bzero(item_name,BUFFER_SIZE-1);
  strcpy(item_name,item);
  printf("Front-end server does not have %s\n",item);
  printf("______________________________________________________________________\n");
  bzero(msg_db,BUFFER_SIZE-1);
  sprintf(msg_db,"%s %s 0",item,cnt);
  //printf("Inside __call_backend : %s\n",msg_db);
  n = write(back_end_sock,msg_db,strlen(msg_db));
  if (n < 0){ error("ERROR writing to socket");}
  //printf("msg_db = %d",n);
  bzero(msg_db,BUFFER_SIZE-1);
  
  n1 = read(back_end_sock,msg_db,BUFFER_SIZE-1);
  if(n1<0){ error("ERROR writing to socket");}
  //printf("Read msg_db = %s",msg_db);
  
  bzero(input,BUFFER_SIZE-1);
  if(msg_db[0]=='$'){
    sprintf(message,"You have to pay Rs.%s to buy %s %s(s).",(msg_db+1),cnt,item);
    strcpy(input,message);
  }
  else if(strcmp(msg_db,"0")==0){
    strcpy(input,"Sorry, the product is not present in our store.");
  }
  else{
    //printf("\nHello in else : %s\n",msg_db);
    sprintf(input,"We don't have enough %s(s).",item_name);
  }
  //printf("%s\n",input);
  return input;
}

//process the order command and sends appropriate messages to clients
char* _process_order(char *input,int back_end_sock){
  //char item = order[0];
  char message[BUFFER_SIZE];
  int n_size = sizeof(product_names)/sizeof(product_names[0]);
  int flag=0,index,i,amount;
  char p,amt[BUFFER_SIZE];
  char *ptr,*item,*cnt;
  ptr = strtok (input," ,.-");
  i=0;
  while(ptr != NULL){
    if(i==0) item=ptr;
    if(i==1) cnt=ptr;
    ptr = strtok (NULL, " ,.-");
    i++;
  }
  amount = atoi(cnt);
  if(strcmp(item,"Pen")==0){index=0; flag=1;}
  else if(strcmp(item,"Book")==0){index=1; flag=1;}
  else if(strcmp(item,"Notebook")==0){index=2; flag=1;}
  else if(strcmp(item,"Mobile")==0){flag=1;index=3;}
  if(flag){
    strcpy(input,__decr_count_update_db(item,cnt,index,input,back_end_sock));
  }
  else{
    strcpy(input,__call_backend(item,cnt,input,back_end_sock));
  }
  return input;
}
  
//used to get the order command and prints the buying status
void get_order(int new_socket,int back_end_sock){
  int n_order;
  char order[BUFFER_SIZE],buying_status[BUFFER_SIZE],temp[BUFFER_SIZE];
  bzero(order,BUFFER_SIZE);
  n_order = read(new_socket,order,BUFFER_SIZE-1);
  if(n_order<0) error("ERROR reading from socket");
  strcpy(buying_status,_process_order(order,back_end_sock));
  n_order = write(new_socket,buying_status,strlen(buying_status));
  if(n_order<0) error("ERROR writing to socket");
  n_order = read(new_socket,temp,BUFFER_SIZE-1);
  if(strcmp(temp,"1")==0){
    bzero(temp,BUFFER_SIZE-1);
    strcpy(temp,"Your order has been placed. Payment will be cash on delivery.");
    n_order = write(new_socket,temp,strlen(temp));
  }
}

//function for threads
void *do_shopping(void *arguments)
{
  struct arg_struct *args = arguments;
  int n;
  int sock = args->newsockfd;
  int back_end_sock = args->back_end_sockfd;
  //char buffer[256];
  int flag_authentication = authentication(sock);          
  if(flag_authentication){
    get_order(sock,back_end_sock);
  }
}

//main program
int main(int argc, char *argv[]){

  struct arg_struct args;
  int sockfd, newsockfd, portno, clilen, pid,*new_sock;
  struct sockaddr_in serv_addr, cli_addr;
  int back_end_sockfd,back_end_newsockfd,b_clilen;
  struct sockaddr_in b_serv_addr,b_cli_addr;
  struct hostent *backend_server;

  //connect to back end server as a client
  backend_server = gethostbyname(argv[1]);
  if (backend_server == NULL) {
    fprintf(stderr,"ERROR, no such host\n");
    exit(0);
  }
  back_end_sockfd = socket(AF_INET,SOCK_STREAM,0);
  args.back_end_sockfd = back_end_sockfd;
  if(back_end_sockfd<0) error("ERROR opening backend socket");
  bzero((char *) &b_serv_addr, sizeof(b_serv_addr));
  b_serv_addr.sin_family = AF_INET;
  bcopy((char *)backend_server->h_addr, 
        (char *)&b_serv_addr.sin_addr.s_addr,
        backend_server->h_length);
  b_serv_addr.sin_port = htons(atoi(argv[2]));
  if(connect(back_end_sockfd,(struct sockaddr *)&b_serv_addr,sizeof(b_serv_addr)) < 0) 
        error("ERROR connecting to Backend Server");
  puts("Connected to back-end server.\n");

  //connect to all the customer clients
  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if(sockfd < 0) error("ERROR opening socket");
  bzero((char *) &serv_addr, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = INADDR_ANY;
  serv_addr.sin_port = htons(atoi(argv[3]));
  if (bind(sockfd, (struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0)error("ERROR on binding");
  listen(sockfd,5);
  clilen = sizeof(cli_addr);
  puts("Waiting for clients...");
  
  while(1){
    //synchronization needed
    newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
    args.newsockfd = newsockfd;
    if (newsockfd < 0) error("ERROR on accept");
    puts("Connection accepted");
    pthread_t client_thread;
    new_sock = malloc(1);
    *new_sock = newsockfd;
    if( pthread_create( &client_thread,NULL,do_shopping,(void*)&args) < 0){
      perror("could not create thread");
      return 1;
    }
    puts("Thread assigned");
  }
  return 0; 
}




