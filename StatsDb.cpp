#include <stdlib.h>
#include <math.h>
#include "StatsDb.hpp"
#include "sqlssStruct.h"
#include "sqlssStruct1.h"

const char* StatsDb::db_user;
const char* StatsDb::db_passwd;
const char* StatsDb::db_name;

int StatsDb::PersistLatency(std::string domain, int latency_value) {
	const char* db_host = "localhost";

	// Establish the connection to the database server.
	try {
            mysqlpp::Connection con(StatsDb::GetDbName(), db_host, StatsDb::GetDbUser(), StatsDb::GetDbPasswd());

            mysqlpp::Query query = con.query();
	    latency row(domain, latency_value);

            query.insert(row);

            // Show the query about to be executed.
            std::cout << "Query: " << query << std::endl;

            // Execute the query.  We use execute() because INSERT doesn't
            // return a result set.
            query.execute();
	    PersistStatEntry(con, domain, latency_value);
	}
	catch (const mysqlpp::BadQuery& er) {
        // Handle any query errors
            std::cerr << "Query error: " << er.what() << std::endl;
            return -1;
        }
        catch (const mysqlpp::BadConversion& er) {  
        // Handle bad conversions
            std::cerr << "Conversion error: " << er.what() << std::endl <<
                "\tretrieved data size: " << er.retrieved <<
                ", actual size: " << er.actual_size << std::endl;
            return -1;
        }
        catch (const mysqlpp::Exception& er) {
        // Catch-all for any other MySQL++ exceptions
            std::cerr << "Error: " << er.what() << std::endl;
            return -1;
	}
	return 0;
}

static const std::string currentDateTime() {
    time_t now = time(0);
    struct tm tstruct;
    char buf[80];
    tstruct = *localtime(&now);
    strftime(buf, sizeof(buf), "%Y-%m-%d.%X", &tstruct);
    return buf;
}    
static const double currentStdDev(int latency_val, int num_queries, double old_mean, double old_std_dev) {
    double delta = latency_val - old_mean;
    double newmean  = old_mean + delta/num_queries;
    double variance = (old_std_dev*old_std_dev) + (delta)*(latency_val - newmean);
    std::cout << "Variance is \n " << variance;
    return sqrt(variance/(num_queries-1));
}
    
    
int StatsDb::PersistStatEntry(mysqlpp::Connection& con, std::string domain, int latency_value) {
	
	try {
	    //update the value
	    mysqlpp::Query query = con.query("select * from stat_entry ");
            query << "where domain = " << mysqlpp::quote << domain;

	    // Retrieve the row, throwing an exception if it fails.
            mysqlpp::StoreQueryResult res = query.store();
            if (res.empty()) {
	     //first time
		int numqueries = 1;
		double stddev = 0.0;
		double average = (double)latency_value;
		const std::string start = currentDateTime();
		mysqlpp::sql_timestamp starttime =
			mysqlpp::sql_timestamp(start);
		 mysqlpp::sql_timestamp lasttime = 
			mysqlpp::sql_timestamp(start);
		
		mysqlpp::Query query = con.query();
	        stat_entry row(domain, average, stddev, numqueries, starttime, lasttime);
		//stat_entry row1("vivs.com", 2345.345, 2324.456, 2345,mysqlpp::sql_timestamp("2016-09-02 12:34:45") , mysqlpp::sql_timestamp("2016-09-02 12:45:30"));
                query.insert(row);

                // Show the query about to be executed.
                std::cout << "Query: " << query << std::endl;

                // Execute the query.  We use execute() because INSERT doesn't
                // return a result set.
                query.execute();
            } else { 

                stat_entry row = res[0];

	    	//Create a copy so that the replace query knows what the
	    	// original values are.
                stat_entry orig_row = row;

		row.numqueries = orig_row.numqueries+1;
                row.average = (((orig_row.average) * orig_row.numqueries)+latency_value)/row.numqueries;
		const double new_std_dev = currentStdDev(latency_value, row.numqueries, orig_row.average, orig_row.stddev);
		row.stddev = new_std_dev;
		const std::string start = currentDateTime();
                 mysqlpp::sql_timestamp lasttime =
                        mysqlpp::sql_timestamp(start);
		row.lasttimestamp = lasttime;	
           	// Form the query to replace the row in the stock table.
               query.update(orig_row, row);

           	// Show the query about to be executed.
               std::cout << "Query: " << query << std::endl;

           	// Run the query with execute(), since UPDATE doesn't return a
           	// result set.
               query.execute();
	    }
	}	
	catch (const mysqlpp::BadQuery& er) {
            // Handle any query errors
            std::cerr << "Query error: " << er.what() << std::endl;
            return -1;
    	}
        catch (const mysqlpp::BadConversion& er) {
            // Handle bad conversions
            std::cerr << "Conversion error: " << er.what() << std::endl <<
                "\tretrieved data size: " << er.retrieved <<
                ", actual size: " << er.actual_size << std::endl;
            return -1;
    	}
        catch (const mysqlpp::Exception& er) {
            // Catch-all for any other MySQL++ exceptions
            std::cerr << "Error: " << er.what() << std::endl;
            return -1;
	}

    return 0;  
}

