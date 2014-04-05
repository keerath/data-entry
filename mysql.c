#include <mysql.h>
#include <stdio.h>
#include <jansson.h>
#include <httpd.h>
#include <string.h>
#include <signal.h>
#include <stdlib.h>
#define PORT_NUMBER 9080
json_error_t error;
MYSQL *conn;
json_t *arr;
/* Initialise MYSQL Connection , execute queries
to get fields and records and convert this to 
json i.e. field-name, field-value pairs. */
void init()
{  
    arr = json_array();
    MYSQL_RES *res1, *res2;
    MYSQL_ROW row1, row2;
    char *server = "127.0.0.1";
    char *user = "root";
    char *password = "iwillnotrocku123"; 
    char *database = "stu";
    int i, j = 0;
    char *s;
    char *fields[100];
    conn = mysql_init(NULL);
    /* Connect to database */
    if (!mysql_real_connect(conn, server, user, password, database, 0, NULL, 0))
    {
        fprintf(stderr, "%s\n", mysql_error(conn));
        exit(1);
    }
    mysql_query(conn, "show fields from details");
    res1 = mysql_store_result(conn);
    while ((row1 = mysql_fetch_row(res1)) != NULL)
    {
        fields[j] = row1[0];
        j++;
    }
    mysql_query(conn,"ALTER TABLE details ADD COLUMN num INT AUTO_INCREMENT UNIQUE FIRST");
    mysql_query(conn,"CREATE TABLE temp LIKE details");
    mysql_query(conn,"INSERT INTO temp (SELECT * FROM details)");
    mysql_query(conn,"ALTER TABLE temp DROP COLUMN num");
    mysql_query(conn,"SELECT * FROM temp");
    res2 = mysql_use_result(conn);
    while ((row2 = mysql_fetch_row(res2)) != NULL)
    {
        json_t *obj = json_object();
        for (i = 0 ; i < mysql_num_fields(res2) ; i++)
        {
            json_object_set(obj, fields[i], json_string_nocheck(row2[i]));
        
        }
        json_array_append(arr, obj);
    }
       mysql_query(conn,"DROP TABLE temp");
}
void update_data(httpd *server)
{
    httpVar *var2,*num,*field;
    char *str;
    if(strcmp(httpdRequestMethodName(server),"POST")==0)
    {  
        var2 = httpdGetVariableByName(server,"celldata");
        num = httpdGetVariableByName(server,"rowNum");
        field = httpdGetVariableByName(server,"field");

        sprintf(str,"UPDATE details SET %s='%s' WHERE num='%s'",field->value,var2->value,num->value);
        printf("%s",str);
        mysql_query(conn,str);
    }
}

/*C function to dump and recieve the data. */
void get_data(httpd *server)
{       
        json_t *col_data;
        int i;
        char *st;
    httpVar *col = malloc(sizeof(httpVar*));
    col_data = json_array();
    if(strcmp(httpdRequestMethodName(server),"GET")==0)
    {
        httpdPrintf(server,"%s",json_dumps(arr,JSON_INDENT(4)));
    }
    else if(strcmp(httpdRequestMethodName(server),"POST")==0)
    {
        col= httpdGetVariableByName(server,"colname");
        st = col->value;
        col_data = json_loads(st,0,&error);
        for(i=0;i<json_array_size(col_data);i++)
        {       
                json_t *col_obj,*st1;
                char *string1,*string2;
                st1 = json_object();
                col_obj=json_array_get(col_data,i);
                sprintf(string1,"%d",i+1);
                st1 = json_object_get(col_obj,string1);
                sprintf(string2,"ALTER TABLE details DROP COLUMN %s",json_string_value(st1));
                if(mysql_query(conn,string2))
                {
                    fprintf(stdout,"%s",mysql_error(conn));
                }
        }
    }
}
void drop()
 {   
    if(mysql_query("ALTER TABLE details DROP COLUMN num"))
    {
        fprintf(stdout,"%s",mysql_error(conn));
    }
 }
void sig_handler(int signo)
{   
    if(signo== SIGINT||signo== SIGABRT||signo==SIGKILL||signo==SIGQUIT)
        exit(1);
}
/* Initialising HTTPD Server and mapping File Names to URLs. */
int main(argc,argv)
{ 
    httpd *server;
server = httpdCreate(NULL,PORT_NUMBER);
if(server==NULL)
{
   perror("Cant Create Server");
}

signal(SIGINT,sig_handler);
signal(SIGABRT,sig_handler);
signal(SIGKILL,sig_handler);
signal(SIGQUIT,sig_handler);
atexit(drop); 
httpdSetAccessLog(server,stdout);
httpdSetErrorLog(server,stderr);
httpdSetFileBase(server,"/cygdrive/c/users/keerath/desktop/project");
httpdAddWildcardContent(server,"/icons",NULL,"icons");
httpdAddWildcardContent(server,"/styles",NULL,"styles");
httpdAddWildcardContent(server,"/styles/css",NULL,"styles/css");
httpdAddWildcardContent(server,"/styles/fonts",NULL,"styles/fonts");
httpdAddWildcardContent(server,"/",NULL,"");
httpdAddFileContent(server,"/","index.html",HTTP_TRUE,NULL,"table.html");
httpdAddCContent(server,"/","get_data",HTTP_FALSE,NULL,get_data);
httpdAddCContent(server,"/","update_data",HTTP_FALSE,NULL,update_data);
init();
while(1==1)
{
   if(httpdGetConnection(server,0)<0)
      continue;
   if(httpdReadRequest(server)<0)
   { 
      httpdEndRequest(server);
      continue;
   }
   else
   {
   httpdProcessRequest(server);
   httpdEndRequest(server);
}
}
return 0;
}