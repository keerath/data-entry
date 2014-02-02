#include <mysql.h>
#include <stdio.h>
#include <jansson.h>
#include <httpd.h>
#include <string.h>
json_t *arr;
httpd *server;
void init()
{
    arr = json_array();
    MYSQL *conn;
    MYSQL_RES *res1, *res2;
    MYSQL_ROW row1, row2;
    char *server = "127.0.0.1";
    char *user = "root";
    char *password = "iwillnotrocku123"; /* set me first */
    char *database = "stu";
    int i, j = 0;
    char *s;
    char *fields[100];
    conn = mysql_init(NULL);

    char *str;
    /* Connect to database */
    if (!mysql_real_connect(conn, server,
                            user, password, database, 0, NULL, 0))
    {
        fprintf(stderr, "%s\n", mysql_error(conn));
        exit(1);
    }

    if (mysql_query(conn, "show fields from details"))
    {
        fprintf(stderr, "%s\n", mysql_error(conn));
        exit(1);
    }
    res1 = mysql_store_result(conn);
    while ((row1 = mysql_fetch_row(res1)) != NULL)
    {
        fields[j] = row1[0];
        j++;
    }

    //mysql_free_result(res);
    mysql_query(conn, "Select * from details");
    res2 = mysql_use_result(conn);
    while ((row2 = mysql_fetch_row(res2)) != NULL)
    {
        json_t *obj = json_object();
        for (i = 0 ; i < mysql_num_fields(res2) ; i++)
        {
            json_object_set(obj, fields[i], json_string_nocheck(row2[i]));
            //printf(row2[i]);
            //printf("\t");
        }
        //printf("\n");
        json_array_append(arr, obj);
    }
}

void get_data()
{
    if(strcmp(httpdRequestMethodName(server),"GET")==0)
    {
        httpdPrintf(server,"%s",json_dumps(arr,JSON_INDENT(4)));
    }
}

main()
{
server = httpdCreate(NULL,9080);
if(server==NULL)
{
   perror("Cant Create Server");
}
httpdSetAccessLog(server,stdout);
httpdSetErrorLog(server,stderr);
httpdSetFileBase(server,"/cygdrive/c/users/keerath/desktop/project");
httpdAddWildcardContent(server,"/icons",NULL,"icons");
httpdAddWildcardContent(server,"/styles",NULL,"styles");
httpdAddWildcardContent(server,"/styles/css",NULL,"styles/css");
httpdAddWildcardContent(server,"/styles/fonts",NULL,"styles/fonts");
httpdAddWildcardContent(server,"/",NULL,"");
httpdAddFileContent(server,"/","index.html",HTTP_TRUE,NULL,"table.html");
init();
httpdAddCContent(server,"/","get_data",HTTP_FALSE,NULL,get_data);
while(1==1)
{
   if(httpdGetConnection(server,NULL)<0)
      continue;
   if(httpdReadRequest(server)<0)
   {
      httpdEndRequest(server);
      continue;
   }
   httpdProcessRequest(server);
   httpdEndRequest(server);
}
}


