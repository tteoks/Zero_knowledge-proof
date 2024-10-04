#include <mysql.h>
#define HOST "127.0.0.1"
#define USER "admin"
#define PASS "suwon"

void db_error_handle(MYSQL *conn);
void db_connect(MYSQL **conn, const char *host, const char *user,
                const char *pass, const char *name);

void db_select(MYSQL **conn, MYSQL_RES **sql_res, const char *query);
