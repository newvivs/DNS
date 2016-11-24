# DNS
This application queries popular websites and measures the DNS query response times and saves them in a DB. It further allows the user to retrieve the following from the DB:
- Average query time
- Standard deviation of query times
- Number of queries made so far
- Timestamp of first and last query made

Usage: DNSApp <interval(seconds between successive queries)> <db_user(default:root)> <db_passwd(default: root)>

Here is the structure of files:

1. DNSApp.cpp - main file to perform queries

2. StatsDb.cpp - Persist to DB

3. DbHelper.cpp - Create DB

Compile:

Assuming ldns and mysql++ is installed on the machine, I used the following

compiler options:

g++ -g -o DNSApp -I/usr/include/mysql/ -I/usr/include/mysql++/ -std=c++11 -

pthread DNSApp.cpp DbHelper.cpp StatsDb.cpp -lldns -lmysqlpp
