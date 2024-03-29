#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>
#include<string.h>
#include<mysql.h>
#include"database.h"



void  db_connect(MYSQL **conn, const char *host, 
				const char *user, const char *pass, 
				const char *name) {
	*conn = mysql_init(NULL);

	if (*conn == NULL) {
		db_error_handle(*conn);
	}

	if (!mysql_real_connect(*conn, host, user, pass, name, 3306,
						(char *) NULL, 0)) {
		db_error_handle(*conn);
	}
}
void db_select(MYSQL **conn, MYSQL_RES **sql_res,
					 const char *query) {

	if (mysql_query(*conn, query)) {
		db_error_handle(*conn);
	}
	
	*sql_res = mysql_store_result(*conn);
	if (sql_res == NULL) {
		db_error_handle(*conn);
	}

}
	/*
int main()
{
	MYSQL *conn = NULL;
	MYSQL_RES *sql_res;
	MYSQL_ROW sql_row;
	int query_stat;

	char value[32];
	char query[1024];
	int  frequency;
	float amount;

	if (mysql_query(conn, query)) {
		db_error_handle(conn);
	}

	sql_res = mysql_store_result(conn);
	if (sql_res == NULL) {
		db_error_handle(conn);
	}

	int num_fields = mysql_num_fields(sql_res);
	while (sql_row = mysql_fetch_row(sql_res)) {
		for (int i=0; i<num_fields; i++) {
			printf("%s\t", sql_row[i] ? sql_row[i] : "NULL");
		}
		printf("\n");
	}

	mysql_free_result(sql_res);
	mysql_close(conn);

	return 0;


}
	*/

void db_error_handle(MYSQL *conn)
{
	fprintf(stderr, "SQL ERROR: %s\n", mysql_error(conn));
	mysql_close(conn);
	exit(1);
}
