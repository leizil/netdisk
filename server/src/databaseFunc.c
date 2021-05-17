#include "../include/head.h"

int insertUserInfo(char* userName,char * salt,char * userPass)
{
	MYSQL *conn;
	char *server = "localhost";
	char *user = "root";
	char *password = "root";
	char *database = "netdisk";//要访问的数据库名�?
    char query[200]={0};
    sprintf(query,"insert into userInfo ( userName, userPass,salt) values ('%s','%s','%s')",userName,userPass,salt);
    printf("insertUserInfo:query is %s\n",query);
    int queryResult;

	conn = mysql_init(NULL);

	if(!mysql_real_connect(conn, server, user, password, database, 0, NULL, 0))
	{
		printf("Error connecting to database: %s\n", mysql_error(conn));
	}
    else
    {
		printf("Connected...\n");
	}

	queryResult = mysql_query(conn, query);
	if(queryResult)
	{
		printf("Error making query:%s\n", mysql_error(conn));
	}
    else
    {
		printf("insert success\n");
	}
	mysql_close(conn);

	return 0;
}


int select_byusername(char *username,char *salt,char *userPass)
{
    MYSQL *conn;
    MYSQL_RES *res;
    MYSQL_ROW row;
    char *host = "localhost";
    char *user = "root";
    char *passwd = "root";
    char *database = "netdisk";
    char query[200] = "select * from userInfo where username = '";
    sprintf(query, "%s%s%s", query, username, "'");

    printf("%s\n", query);

    conn = mysql_init(NULL);
    if(!conn)
    {
        printf("mysql init fail\n");
        return -1;
    }

    if(!mysql_real_connect(conn, host, user, passwd, database, 0, NULL, 0))
    {
        printf("Error connecting to database: %s\n", mysql_error(conn));
        return -1;
    }
    else
    {
        printf("MySQL connected...\n");
    }

    int queryRet = mysql_query(conn, query);
    if(queryRet)
    {
        printf("Error query: %s\n", mysql_error(conn));
        return -1;
    }
    else
    {
        res = mysql_store_result(conn);
        /* printf("mysql_affacted_rows: %lu\n", (unsigned long)mysql_affected_rows(conn)); */
        printf("mysql_num_rows: %lu\n", (unsigned long)mysql_num_rows(res));

        row = mysql_fetch_row(res);
        if(NULL == row)
        {
            printf("Don't query any data\n");
            return 0;
        }
        else
        {
            /* for(queryRet = 0; queryRet != (int)mysql_num_fields(res); ++queryRet) */
            /* { */
            /*     printf("%8s ", row[queryRet]); */
            /* } */
            strcpy(userPass,row[2]);
            strcpy(salt,row[3]);
            printf("\n");
        }

        mysql_free_result(res);
    }

    mysql_close(conn);
    return 1;
}












