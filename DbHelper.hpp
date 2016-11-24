#ifndef DBHELPER_H
#define DBHELPER_H
#include <mysql++.h>
#include <ssqls.h>

class DbHelper {
    private:
	const char* db_user;
	const char* db_passwd;
	const char* db_name;
	DbHelper() {};
	static DbHelper* db_instance;
    public:
	static DbHelper* GetInstance();
	void SetDbUser(const char*);
	void SetDbPasswd(const char*);
	void SetDbName(const char*);
	const char* GetDbUser();
	const char* GetDbPasswd();
	int CreateDB();
};
#endif
