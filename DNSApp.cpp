#include <string>
#include <iostream>
#include <thread>
#include <vector>
#include <ldns/ldns.h>
#include <time.h>
#include <mysql++.h>
#include "DbHelper.hpp"
#include "StatsDb.hpp"

#define DB_NAME "Test42"

class DNSQuery {
    private:
	std::unique_ptr<StatsDb> statsd;
    public:
	DNSQuery():statsd(new StatsDb) {};

	std::string generateRandomString(int len) {
	    std::string random_string;
	    for (int i = 0; i < len; ++i) {
         	int randomChar = rand()%(26+26+10);
		char c;
         	if (randomChar < 26)
             	    c = 'a' + randomChar;
         	else if (randomChar < 26+26)
             	    c = 'A' + randomChar - 26;
         	else
             	    c  = '0' + randomChar - 26 - 26;
		random_string = random_string + c;
     	    }
	    return random_string;
	}

	void query(std::string domain) {
	    
	    ldns_resolver *res;
	    ldns_rdf *name;
	    ldns_rdf *version, *id;
	    ldns_pkt *p;
	    ldns_rr_list *addr;
	    ldns_rr_list *info;
	    ldns_status s;
	    ldns_rdf *pop;
	    size_t i; 
	    struct timeval start_tv, end_tv;

	    /* create a rdf from the command line arg */
	    std::string query_string = generateRandomString(5) + "." + domain;
	    const char* dom = query_string.c_str();
	    name = ldns_dname_new_frm_str(dom);

	    /* create a new resolver from /etc/resolv.conf */
	    s = ldns_resolver_new_frm_file(&res, NULL);
	    if (s != LDNS_STATUS_OK) {
		ldns_rdf_deep_free(name);
		exit(EXIT_FAILURE);
	    }
	    ldns_resolver_set_retry(res, 1); /* don't want to wait too long */
	
	    /* use the resolver to send it a query for the a/aaaa of name */
	    gettimeofday(&start_tv, NULL);
	    addr = ldns_get_rr_list_addr_by_name(res, name, LDNS_RR_CLASS_IN, LDNS_RD);
	    std::cout << "Domain name is " << dom <<std::endl;
	    gettimeofday(&end_tv, NULL);
	    long durationms = ((end_tv.tv_sec*1000 + end_tv.tv_usec/1000) - (start_tv.tv_sec*1000 + start_tv.tv_usec/1000));
	    statsd->PersistLatency(domain, durationms);
	    /* remove current list of nameservers from resolver */
	    while((pop = ldns_resolver_pop_nameserver(res))) { ldns_rdf_deep_free(pop); }
	    ldns_rdf_deep_free(name);
	    ldns_resolver_deep_free(res);
	}
};

class MultiThread {
    private:
	std::vector<std::string> domains = {"google.com", "facebook.com", "youtube.com", "yahoo.com", "live.com", "wikipedia.org", "baidu.com","blogger.com", "msn.com","qq.com"};
        std::vector<std::string>::iterator myDomainIterator;
        std::vector<std::thread> Threads;
    public:
    	void run() {
	    for (myDomainIterator = domains.begin(); myDomainIterator != domains.end(); myDomainIterator++) {
		std::unique_ptr<DNSQuery>dns (new DNSQuery);
		Threads.push_back(std::thread(&DNSQuery::query, std::move(dns), *myDomainIterator));
            }
        }
	~MultiThread() {
	    std::thread::id idVal;
	    for (auto &t : Threads) {
		idVal = t.get_id();
		t.join();
	    }
	}

};

int main(int argc, char* argv[]) {
        const char* db_user;
	const char* db_passwd;
	unsigned int interval;
	    	
	srand(time(NULL));
	if (argv[1] != NULL && atoi(argv[1]) < 1) {
	    std::cout << "Please enter an interval value greater or equal to 1 " << std::endl;
	    return 1;
	}	
	switch (argc) {
	    case 2:
		interval = atoi(argv[1]);
		db_user = "root";
		db_passwd = "root";
		break;
	    case 3:
		interval = atoi(argv[1]);
                db_user = argv[2];
		db_passwd = "root";
		break;
	    case 4:
		interval = atoi(argv[1]);
                db_user = argv[2];
                db_passwd = argv[3];
		break;
	    default:
		std::cout << "Usage: DNSApp <interval(secs between successive queries)> <db_user(default:root)> <db_password(default:root)>\n" << std::endl;
		return 1;
        }
	DbHelper* dbHelp = DbHelper::GetInstance();
	dbHelp->SetDbUser(db_user);
	dbHelp->SetDbPasswd(db_passwd);
	dbHelp->SetDbName(DB_NAME);
	dbHelp->CreateDB();

	StatsDb::SetDbUser(db_user);
	StatsDb::SetDbPasswd(db_passwd);
	StatsDb::SetDbName(DB_NAME);
	 
	MultiThread multi;
	unsigned int ticks = interval*1000*1000;
	while (1) {
	    std::cout << "Running queries to top 10 domains every " << interval << "secs";
	    multi.run();
	    usleep(ticks);
	}
}
