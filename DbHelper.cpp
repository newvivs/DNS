#include <stdlib.h>
#include "DbHelper.hpp"

DbHelper*DbHelper::db_instance = NULL;

DbHelper* DbHelper::GetInstance() {
    if (db_instance == NULL) {
	db_instance = new DbHelper();
    }
    return db_instance;
};

void DbHelper::SetDbUser(const char* user) {
    db_user = user;
};

void DbHelper::SetDbPasswd(const char* passwd) {
    db_passwd = passwd;
};

void DbHelper::SetDbName(const char* name) {
    db_name = name;
};

const char* DbHelper::GetDbUser() {
    return db_user;
}
const char* DbHelper::GetDbPasswd() {
    return db_passwd;
}

int DbHelper::CreateDB() {
	const char* db_host = "localhost";
	//Lets create DB if it doesn't exist else continue
	mysqlpp::Connection con;
	con.connect(0, db_host, db_user, db_passwd, 0);
	bool new_db = false;
	{
	    mysqlpp::NoExceptions ne(con);
	    mysqlpp::Query query = con.query();
	    // Database doesn't exist yet, so create and select it.
	    if (con.select_db(db_name)) {
		std::cout << "DB Exists" << std::endl;
	    } else if (con.create_db(db_name) && con.select_db(db_name)) {
		    new_db = true;
		    std::cout << "DB created" << std::endl;
	    } else {
		std::cerr << "Error creating DB: " << con.error() << std::endl;
		return 1;
	    }
	}
	try {
		// Send the query to create the stock table and execute it.
		std::cout << "Creating latency table..." << std::endl;
		mysqlpp::Query query = con.query();
		query <<
			"CREATE TABLE latency (" <<
			"  domain CHAR(30) NOT NULL, " <<
			"  latencyvalue BIGINT NOT NULL) " <<
		"ENGINE = InnoDB " <<
		"CHARACTER SET utf8 COLLATE utf8_general_ci";
		query.execute();

		std::cout << "Creating the stat table..." << std::endl;
		query <<
			"CREATE TABLE stat_entry (" <<
			"  domain CHAR(30) NOT NULL, " <<
			"  average DOUBLE NOT NULL, " <<
			"  stddev DOUBLE NULL, " <<
			"  numqueries BIGINT NOT NULL, " <<
			"  starttimestamp TIMESTAMP NOT NULL, "<<
			"  lasttimestamp TIMESTAMP NOT NULL) "<< 
			"ENGINE = InnoDB " <<
			"CHARACTER SET utf8 COLLATE utf8_general_ci";
		query.execute();	

	}
	catch (const mysqlpp::BadQuery& er) {
		// Handle any query errors
		std::cerr << std::endl << "Query error: " << er.what() << std::endl;
		return 1;
	}
	catch (const mysqlpp::BadConversion& er) {
		// Handle bad conversions
		std::cerr << std::endl << "Conversion error: " << er.what() << std::endl <<
				"\tretrieved data size: " << er.retrieved <<
                                ", actual size: " << er.actual_size << std::endl;
                return 1;
	}
	catch (const mysqlpp::Exception& er) {
			// Catch-all for any other MySQL++ exceptions
                std::cerr << std::endl << "Error: " << er.what() << std::endl;
		return 1;
	}
	return 0;
};
