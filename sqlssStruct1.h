#include <mysql++.h>
#include <ssqls.h>

sql_create_2(latency,
1, 2,
mysqlpp::sql_char, domain,
mysqlpp::sql_bigint, latencyvalue);
