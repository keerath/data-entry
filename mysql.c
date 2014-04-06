#include <mysql.h>
#include <stdio.h>
#include <jansson.h>
#include <httpd.h>
#include <string.h>
#include <signal.h>
#include <stdlib.h>
#include <windows.h>
#define PORT_NUMBER 9080
#define MAX_RECORD_LENGTH 1024
#define MAX_VALUE_LENGTH 50
#define MAX_QUERY_LENGTH 200
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
    mysql_query(conn,"ALTER TABLE details ADD COLUMN num INT AUTO_INCREMENT UNIQUE");
    mysql_query(conn,"CREATE TABLE temp LIKE details");
    mysql_query(conn,"INSERT INTO temp (SELECT * FROM details)");
    mysql_query(conn,"ALTER TABLE temp DROP COLUMN num");
    mysql_query(conn, "show fields from temp");
    res1 = mysql_store_result(conn);
    while ((row1 = mysql_fetch_row(res1)) != NULL)
    {
        fields[j] = row1[0];
        j++;
    }
    mysql_query(conn,"SELECT * FROM temp");
    res2 = mysql_store_result(conn);
    while ((row2 = mysql_fetch_row(res2)) != NULL)
    {
        json_t *obj = json_object();

        for (i = 0 ; i < mysql_num_fields(res2) ; i++)
        {   
            json_object_set(obj, fields[i], json_string(row2[i]));
        }
        json_array_append(arr, obj);
    }
    mysql_query(conn,"DROP TABLE temp");

    return;
}
void update_data(httpd *server)
{
    httpVar *var2,*num,*field;
    char *str = (char *)malloc(MAX_QUERY_LENGTH);
    if(strcmp(httpdRequestMethodName(server),"POST")==0)
    {  
        var2 = httpdGetVariableByName(server,"celldata");
        num = httpdGetVariableByName(server,"rowNum");
        field = httpdGetVariableByName(server,"field");

        sprintf(str,"UPDATE details SET %s='%s' WHERE num='%s'",field->value,var2->value,num->value);
        printf("%s",str);
        if(mysql_query(conn,str))
        {
            fprintf(stderr,"%s",mysql_error(conn));
        }
    }
    free(str);
    return;
}

/*C function to dump and recieve the data. */
void get_data(httpd *server)
{       
    if(strcmp(httpdRequestMethodName(server),"GET")==0)
    {
        httpdPrintf(server,"%s",json_dumps(arr,JSON_PRESERVE_ORDER));
    }
    return;
}
void new_data(httpd *server)
{   httpVar *insert,*colname;
    int j=0;
    json_t *new_obj =json_object(),*new_array=json_array();
    const char *key;
    json_t *value;
    char *new_keys,*new_values,*query = (char *)malloc(MAX_QUERY_LENGTH);
    char temp[MAX_QUERY_LENGTH];
    size_t index;
    if(strcmp(httpdRequestMethodName(server),"POST")==0)
    {   
        insert = httpdGetVariableByName(server,"newrowdata");
        new_obj = json_loads(insert->value,0,&error);
        new_keys = calloc(MAX_RECORD_LENGTH,sizeof(char)) ;
        new_values = calloc(MAX_RECORD_LENGTH,sizeof(char));
        json_object_foreach(new_obj,key,value)
        {   
            j++; 
            strcat(new_keys,key);
            sprintf(temp,"'%s'",json_string_value(value));
            strcat(new_values,temp);

            if(j!=json_object_size(new_obj))
            {
                strcat(new_keys,",");
                strcat(new_values,",");
            }
        }
           sprintf(query,"INSERT INTO details (%s) VALUES (%s)",new_keys,new_values);

           if(mysql_query(conn,query))
                fprintf(stderr,"%s",mysql_error(conn));

           free(new_keys);
           free(new_values);
        
        }
    else if(strcmp(httpdRequestMethodName(server),"GET")==0)
    {   
        colname = httpdGetVariableByName(server,"new_th");
        sprintf(query,"ALTER TABLE details ADD COLUMN `%s` varchar(25)",colname->value);//datatype currently hard-coded
        if(mysql_query(conn,query))
            fprintf(stderr,"%s",mysql_error(conn));
        insert = httpdGetVariableByName(server,"newcoldata");
        new_array = json_loads(insert->value,0,&error);
        json_array_foreach(new_array,index,value)
        {
            sprintf(temp,"UPDATE details SET `%s`='%s' WHERE num = %d",colname->value,json_string_value(value),++j);
            if(mysql_query(conn,temp))
                fprintf(stderr,"%s",mysql_error(conn));
        }
    }
    free(query);
    return;
}
void del(httpd *server)
{     httpVar *rows,*cols;
      json_t *row_index= json_array(),*value,*col_data=json_array();
      size_t index;
      char *query =(char *)malloc(MAX_QUERY_LENGTH);
    if(strcmp(httpdRequestMethodName(server),"GET")==0)
    {   
        rows = httpdGetVariableByName(server,"indices");
        row_index = json_loads(rows->value,0,&error);
        json_array_foreach(row_index,index,value)
        {
            sprintf(query,"DELETE FROM details WHERE num = %s",json_string_value(value));
            if(mysql_query(conn,query))
                fprintf(stderr,"%s",mysql_error(conn));
        }
    }
    else if(strcmp(httpdRequestMethodName(server),"POST")==0)
    {   
        cols= httpdGetVariableByName(server,"colname");
        col_data = json_loads(cols->value,0,&error);
        json_array_foreach(col_data,index,value)
        {
            sprintf(query,"ALTER TABLE details DROP COLUMN %s",json_string_value(value));
            if(mysql_query(conn,query))
                fprintf(stderr,"%s",mysql_error(conn));
        }
    }
    free(query);
    return;
}
void drop()
{   
    if(mysql_query(conn,"ALTER TABLE details DROP COLUMN num"))
    
        fprintf(stdout,"%s",mysql_error(conn));
        return;
}
void sig_handler(int signo)
{   
    if(signo== SIGINT||signo== SIGABRT||signo==SIGKILL||signo==SIGQUIT)
        exit(1);
        return;
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
    httpdAddWildcardContent(server,"/scripts",NULL,"scripts");
    httpdAddWildcardContent(server,"/",NULL,"");
    httpdAddFileContent(server,"/","index.html",HTTP_TRUE,NULL,"table.html");
    httpdAddCContent(server,"/","get_data",HTTP_FALSE,NULL,get_data);
    httpdAddCContent(server,"/","update_data",HTTP_FALSE,NULL,update_data);
    httpdAddCContent(server,"/","new_data",HTTP_FALSE,NULL,new_data);
    httpdAddCContent(server,"/","del",HTTP_FALSE,NULL,del);
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