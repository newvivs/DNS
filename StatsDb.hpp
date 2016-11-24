#ifndef STATSDB_H
#define STATSDB_H
#include <stdlib.h>
#include <mysql++.h>

class StatsDb {
    private:
        static const char* db_user;
        static const char* db_passwd;
        static const char* db_name;
        mysqlpp::Connection con;
    public:
        StatsDb() {};
	
        static void SetDbUser(const char* user) { 
	    db_user = user;
	    return;
	}
        static void SetDbPasswd(const char* passwd) {
	    db_passwd = passwd;
	    return;
	};
        static void SetDbName(const char* name) {
	    db_name = name;
	    return;
	}
        static const char* GetDbUser() { return db_user;}
        static const char* GetDbName() { return db_name;}
        static const char* GetDbPasswd() { return db_passwd;}
	int PersistLatency(std::string, int);
	int PersistStatEntry(mysqlpp::Connection&,std::string, int);
};
#endif

