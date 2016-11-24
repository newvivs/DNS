#include <mysql++.h>
#include <ssqls.h>

sql_create_6(stat_entry,
1, 6,
mysqlpp::sql_char, domain,
mysqlpp::sql_double, average,
mysqlpp::sql_double, stddev,
mysqlpp::sql_bigint, numqueries,
mysqlpp::sql_timestamp, starttimestamp,
mysqlpp::sql_timestamp, lasttimestamp);

