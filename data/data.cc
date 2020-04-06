#include <iostream>
#include <string> 
#include <stack> 
#include <algorithm>
#include <sstream>
#include <mysql/mysql.h>
#include <unistd.h>

using namespace std;

MYSQL mysql;   
MYSQL_ROW row;  
MYSQL_FIELD* field = NULL;       
MYSQL_RES* result;       

int main() {
    mysql_init(&mysql);
    if(!mysql_real_connect(&mysql,"localhost","root","root","face",0,NULL,0))
    {     
        cout << "connect fial\n";
        return -1;
    }
    cout << "connect success\n";
    mysql_close(&mysql);
    return 0;
}
