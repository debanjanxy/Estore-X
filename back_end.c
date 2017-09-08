/* A simple server in the internet domain using TCP
   The port number is passed as an argument */
#include <stdio.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sqlite3.h>

#define BUFFER_SIZE 1024


void error(char *msg)
{
    perror(msg);
    exit(1);
}
char value[20],price[20];
int f=0;
static int callback(void *data, int argc, char **argv, char **azColName){
  int i;
  //fprintf(stderr, "%s: ", (const char*)data);
  strcpy(value,argv[0]);
  strcpy(price,argv[1]);
  //printf("Value at CallBack = %s\n",value);
  //printf("price at callback = %s\n",price);
  f=1;
  /*
  for(i = 0; i<argc; i++){
    //strcpy(value,argv[i]);
      
    // printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
  }
   
  printf("\n");
  */
  return 0;
}

int database(char *item, char *cnt)
{
  sqlite3 *db;
  char *zErrMsg = 0;
  int rc,count,current_count;
  char sql[100],name[20];
  const char* data = "Callback function called";
  f=0;
  /* Open database */
  rc = sqlite3_open("Products.db", &db);
  if( rc ) {
    fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
    return(0);
  }
  
  sprintf(sql, "SELECT COUNT,PRICE from DATA where NAME='%s'",item);
  //printf("hii %s\n",sql);
  /* Execute SQL statement */
  rc = sqlite3_exec(db, sql, callback, (void*)data, &zErrMsg);
  if( rc != SQLITE_OK ) {
    fprintf(stderr, "SQL error: %s\n", zErrMsg);
    sqlite3_free(zErrMsg);
  }
  //printf("Value = %s\n",value);
  current_count=(strtol(value,NULL,10));
  count = (strtol(cnt,NULL,10));
  if(f==0)
  {
   	sqlite3_close(db);
   	return 0;
  }
  if(count > current_count)
  {
   	sqlite3_close(db);
   	return current_count;
  }
   	
  current_count -=count;
  sprintf(sql, "UPDATE DATA set COUNT = %d where NAME='%s'",current_count,item);
    
  /* Execute SQL statement */
  rc = sqlite3_exec(db, sql, callback, (void*)data, &zErrMsg);

  if( rc != SQLITE_OK ) {
    fprintf(stderr, "SQL error: %s\n", zErrMsg);
    sqlite3_free(zErrMsg);
  }
  sprintf(sql, "SELECT COUNT,PRICE from DATA where NAME='%s'",item);

  rc = sqlite3_exec(db, sql, callback, (void*)data, &zErrMsg);
  //printf("%s left = %s\n",item,value);
  if( rc != SQLITE_OK ) {
    fprintf(stderr, "SQL error: %s\n", zErrMsg);
    sqlite3_free(zErrMsg);
  }
  sqlite3_close(db);
  return 1;
}

int main(int argc, char *argv[])
{
	int sockfd, newsockfd, portno, clilen, no_of_prod,i,flag=0;
  double amount;
  char buff[BUFFER_SIZE],send_buff[BUFFER_SIZE];
  char *ptr, *item, *cnt, *fl;
  struct sockaddr_in serv_addr, cli_addr;
  int n;
  if (argc < 2) {
    fprintf(stderr,"ERROR, no port provided\n");
    exit(1);
  }
  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd < 0) error("ERROR opening socket");
  bzero((char *) &serv_addr, sizeof(serv_addr));
     portno = atoi(argv[1]);
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = INADDR_ANY;
  serv_addr.sin_port = htons(portno);
  if (bind(sockfd, (struct sockaddr *) &serv_addr,
      sizeof(serv_addr)) < 0) error("ERROR on binding");
  puts("Waiting for front-end server...");
  listen(sockfd,5);
  clilen = sizeof(cli_addr);
  newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
  if (newsockfd < 0) error("ERROR on accept");
  puts("Connected to front-end server");
  while(1){
		bzero(buff,BUFFER_SIZE);
		

    n = read(newsockfd,buff,BUFFER_SIZE-1);
		if (n < 0) error("ERROR reading from socket");

    printf("%s\n",buff);
		ptr = strtok (buff," ,.-");
		i=0;
		while (ptr != NULL){
		  if(i==0) item=ptr;
		  if(i==1) cnt=ptr;
      if(i==2) fl = ptr;
			ptr = strtok(NULL, " ,.-");
			i++;
		}
		flag=database(item,cnt); 
    //puts("Hello");
    //printf("First : Count = %s\tItem = %s\tfl = %s\n",cnt,item,fl);
    amount=atoi(cnt)*atof(price);
    printf("Total amount = %lf\n",amount);    
		
    bzero(send_buff,BUFFER_SIZE-1);
    
    printf("Count = %s\nItem = %s\nfl = %s\n",cnt,item,fl);
    //printf("FL Value = %s\n",fl);
		if(flag==0) sprintf(send_buff,"0");
		else if(flag==1){
      if(strcmp(fl,"0")==0) sprintf(send_buff,"$%lf",amount);
      else sprintf(send_buff,"1");
    }
		else sprintf(send_buff,"%d",flag);
    printf("Buffer value = %s\n",send_buff);
		n = write(newsockfd,send_buff,100);
		if (n < 0) error("ERROR writing to socket");
		
  }
  return 0; 
}

    
    /*
    strcpy(result,process_request(buff));
    bzero(buff,BUFFER_SIZE);
    strcpy(buff,result);
    n = write(newsockfd,buff,1);
    if(n<0) error("ERROR writing to socket");
    */