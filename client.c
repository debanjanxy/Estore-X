#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"



#define TRUE   1 
#define FALSE  0 
#define PORT 8888  
#define HOST "localhost"
#define BUFFER_SIZE 1024

void error(char *msg)
{
    perror(msg);
    exit(0);
}

int log_in(int sockfd){
    char username_buffer[BUFFER_SIZE],
         passwd_buffer[BUFFER_SIZE],
         message_buffer[BUFFER_SIZE];
    int n_username, n_message, n_passwd,flag;
    //printf(ANSI_COLOR_RED "\n~~~~~~~~~~~~~~~~~~~WELCOME~~~~~~~~~~~~~~~~\n\n" ANSI_COLOR_RESET "\n");
    //char name[1024] = "Debanjan";
    //printf("\033[22;34mHello, world! %s\033[0m",name);
    printf(ANSI_COLOR_BLUE "============================LOGIN===========================\n" ANSI_COLOR_RESET);
    /*
    int c;
    FILE *file;
    file = fopen("start_show.txt", "r");
    if(file){
        while((c = getc(file)) != EOF)
            printf("\033[22;34m%c\033[0m",c);
    fclose(file);
    }
    */

    printf(ANSI_COLOR_MAGENTA "Username: " ANSI_COLOR_RESET);
    bzero(username_buffer,BUFFER_SIZE);
    //scanf("%s",username_buffer);
    fgets(username_buffer,BUFFER_SIZE-1,stdin);
    username_buffer[strlen(username_buffer)-1]='\0';
    printf(ANSI_COLOR_MAGENTA "Password: " ANSI_COLOR_RESET);
    bzero(passwd_buffer,BUFFER_SIZE);
    //scanf("%s",passwd_buffer);
    fgets(passwd_buffer,BUFFER_SIZE-1,stdin);
    passwd_buffer[strlen(passwd_buffer)-1]='\0';
    n_username = write(sockfd,username_buffer,strlen(username_buffer));
    if (n_username < 0){ error("ERROR writing to socket"); }
    bzero(username_buffer,BUFFER_SIZE);
    bzero(message_buffer,BUFFER_SIZE);
    n_message = read(sockfd,message_buffer,BUFFER_SIZE-1);
    if(strcmp(message_buffer,"1")==0){
        n_passwd = write(sockfd,passwd_buffer,strlen(passwd_buffer));
        if (n_passwd < 0){ error("ERROR writing to socket"); }
        bzero(passwd_buffer,BUFFER_SIZE-1);
    }
    n_message = read(sockfd,message_buffer,BUFFER_SIZE-1);
    flag = atoi(message_buffer);
    if(flag==1){
    	printf(ANSI_COLOR_GREEN "Log-in successful.\n" ANSI_COLOR_RESET);
    }
    else{
    	printf(ANSI_COLOR_RED "Log-in failed.\n" ANSI_COLOR_RESET);
    }
    return flag;
}

void place_order(int sockfd){
    char item,ans,again,amt[20],order_buffer[BUFFER_SIZE],order_status[BUFFER_SIZE];
    int n_order,d;
    char *ptr,*item1,*cnt,buff[BUFFER_SIZE];
    
    printf(ANSI_COLOR_YELLOW "\n~~~~~~~~~~~~~~~~~~~~~~~~~~~ITEMS~~~~~~~~~~~~~~~~~~~~~~~~~~~\n" ANSI_COLOR_RESET);
    printf(ANSI_COLOR_BLUE "==> Mobile \t\t==> Book \n==> Pen \t\t==> Notebook \n==> Laptop \t\t==> Keyboard \n==> Watch \t" ANSI_COLOR_RESET);
    printf(ANSI_COLOR_BLUE "\t==> Shirt \n==> Jeans \t\t==> Charger \n==> Headphone \t\t==> Shampoo \n==> Deodrant \n" ANSI_COLOR_RESET);
    shopping:printf(ANSI_COLOR_RED "\nUsage: Item_Name Amount_of_Item\n" ANSI_COLOR_RESET);
    printf(ANSI_COLOR_MAGENTA "\nInput: " ANSI_COLOR_RESET);
    bzero(order_buffer,BUFFER_SIZE-1);
    fgets(order_buffer,BUFFER_SIZE-1,stdin);
    order_buffer[strlen(order_buffer)-1]='\0';
    /*
    bzero(buff,BUFFER_SIZE);
    strcpy(buff,order_buffer);
    ptr = strtok (buff," ,.-");
    i=0;
    while (ptr != NULL){
        if(i==0) item1=ptr;
        if(i==1) cnt=ptr;
        i++;
    }
    printf("hello\n");
    printf("%s\n",cnt);
    if(!cnt){
        printf("Wrong input format. Try again.");
        goto label;
    }
    */
    //printf("%s\n",order_buffer);
    n_order = write(sockfd,order_buffer,strlen(order_buffer));
    if(n_order<0){ puts("ERROR writing to socket"); }
    n_order = read(sockfd,order_status,BUFFER_SIZE-1);
    printf("\033[22;31m\n%s\033[0m\n",order_status);
    ptr = strtok (order_status," ,.-");
    d=0;
    while (ptr != NULL){
        if(d==0) item1=ptr;
        ptr = strtok(NULL, " ,.-");
        d++;
    }
    //printf("Item1 = %s\n",item1);
    if(strcmp(item1,"Sorry,")==0||strcmp(item1,"Sorry")==0){exit(0);}
    if(strcmp(item1,"We")==0){exit(0);}
    if(strcmp(order_status,"ERROR occured, please try again.")==0){ exit(0); }
    //else if(strcmp(order_status,"Sorry, the product is not present in our store.")==0){ exit(0); }
    //printf(ANSI_COLOR_CYAN "Do you want to proceed ? (y/n)" ANSI_COLOR_RESET);
    //scanf("%c",&ans);
    //if(ans=='y'){
    bzero(order_status,BUFFER_SIZE-1);
    n_order = write(sockfd,"1",1);
    n_order = read(sockfd,order_status,BUFFER_SIZE-1);
    printf("%s\n",order_status);
    //}
    printf(ANSI_COLOR_CYAN "\nThanks for your interest. Visit again. Have a good day :)\n" ANSI_COLOR_RESET);
    /*
    printf("Shop again? (y/n): \n");
    scanf("%c",&again);
    if(again=='y') goto shopping;
    else
    */ 
}

int main(int argc, char *argv[])
{
    int sockfd, portno;
    int is_login_successful;
    struct sockaddr_in serv_addr;
    struct hostent *server;
    int c;
    FILE *file;
    file = fopen("start_show1.txt", "r");
    if (file) {
        while ((c = getc(file)) != EOF)
            printf("\033[22;34m%c\033[0m",c);
            //putchar(c);
        fclose(file);
    }
    if (argc < 2) {
       fprintf(stderr,"usage %s hostname\n", argv[0]);
       exit(0);
    }
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0){ error("ERROR opening socket"); }
    server = gethostbyname(argv[1]);
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, 
         (char *)&serv_addr.sin_addr.s_addr,
         server->h_length);
    serv_addr.sin_port = htons(atoi(argv[2]));
    if (connect(sockfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr)) < 0) 
        error("ERROR connecting");
    //do log in to the system
    is_login_successful = log_in(sockfd);
    if(is_login_successful){
        place_order(sockfd);
    }
    return 0;
}
