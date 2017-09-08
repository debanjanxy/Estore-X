#include <stdio.h>
#include <stdlib.h>
#include <sqlite3.h> 

static int callback(void *NotUsed, int argc, char **argv, char **azColName) {
   int i;
   for(i = 0; i<argc; i++) {
      printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
   }
   printf("\n");
   return 0;
}

int main(int argc, char* argv[]) {
   sqlite3 *db;
   char *zErrMsg = 0;
   int rc;
   char *sql;

   /* Open database */
   rc = sqlite3_open("Products.db", &db);
   
   if( rc ) {
      fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
      return(0);
   } else {
      fprintf(stderr, "Opened database successfully\n");
   }

   /* Create SQL statement */
   sql = "UPDATE DATA SET COUNT=1000 WHERE NAME='Book';" \
         "UPDATE DATA SET COUNT=1000 WHERE NAME='Pen';" \
         "UPDATE DATA SET COUNT=1000 WHERE NAME='Mobile';" \
         "UPDATE DATA SET COUNT=1000 WHERE NAME='Notebook';" \
         "UPDATE DATA SET COUNT=1000 WHERE NAME='Laptop';" \
         "UPDATE DATA SET COUNT=1000 WHERE NAME='Watch';" \
         "UPDATE DATA SET COUNT=1000 WHERE NAME='Shirt';" \
         "UPDATE DATA SET COUNT=1000 WHERE NAME='Jeans';" \
         "UPDATE DATA SET COUNT=1000 WHERE NAME='Charger';" \
         "UPDATE DATA SET COUNT=1000 WHERE NAME='Headphone';" \
         "UPDATE DATA SET COUNT=1000 WHERE NAME='Deodorant';" \
         "UPDATE DATA SET COUNT=1000 WHERE NAME='Shampoo';" \
         "UPDATE DATA SET COUNT=1000 WHERE NAME='Keyboard';" ;
         

   /* Execute SQL statement */
   rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);
   
   if( rc != SQLITE_OK ){
      fprintf(stderr, "SQL error: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
   } else {
      fprintf(stdout, "Records created successfully\n");
   }
   sqlite3_close(db);
   return 0;
}
