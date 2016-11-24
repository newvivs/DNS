# DNS
Usage: DNSApp <interval(seconds between successive queries)> <db_user(default:root)> <db_passwd(default: root)>

Here is the structure of files:

1. DNSApp.cpp - main file to perform queries

2. StatsDb.cpp - Persist to DB

3. DbHelper.cpp - Create DB

Compile:

Assuming lens and mysql++ is installed on the machine, I used the following

compiler options:

g++ -g -o DNSApp -I/usr/include/mysql/ -I/usr/include/mysql++/ -std=c++11 -

pthread DNSApp.cpp DbHelper.cpp StatsDb.cpp -lldns -lmysqlpp
