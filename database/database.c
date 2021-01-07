#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<stdbool.h>
#include<mysql.h>

#define HOST "127.0.0.1"
#define USER "admin"
#define PASS ""
#define NAME "KCDC"
#define TABLE "_PrivateSetY"

void db_error_handle(MYSQL *conn);



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

	conn = mysql_init(NULL);

	if (conn == NULL) {
		db_error_handle(conn);
	}
	
	if (!mysql_real_connect(conn, HOST,	USER, PASS,	NAME, 3306, (char *)NULL, 0)) {
		db_error_handle(conn);
	}
	
	sprintf(query, "SELECT * FROM %s", TABLE);
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

void db_error_handle(MYSQL *conn)
{
	fprintf(stderr, "SQL ERROR: %s\n", mysql_error(conn));
	mysql_close(conn);
	exit(1);
}
