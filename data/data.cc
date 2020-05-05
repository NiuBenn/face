#include <iostream>
#include <string> 
#include <cstring>
#include <stack> 
#include <algorithm>
#include <sstream>
#include <fstream>
#include <mysql/mysql.h>
#include <unistd.h>

using namespace std;

MYSQL mysql;   
MYSQL_ROW row;  
MYSQL_FIELD* field = NULL;       
MYSQL_RES* result;       


void add(string name, string feature){
    string sql = "insert into faceinfo(name, feature) values('" + name + "','" + feature + "');";
    cout << "sql : " << endl << sql << endl;
    mysql_query(&mysql,sql.c_str());
}

void select(){
    string sql = "SELECT * FROM faceinfo;";
    mysql_query(&mysql,sql.c_str());
    result = mysql_store_result(&mysql);
    if(result == NULL)
        cout << "fail\n";

    for(int i=0; i<mysql_num_fields(result); i++)
    {
        field = mysql_fetch_field_direct(result,i);
        cout << field->name << "\t\t";
    }
    cout << endl;   
    row = mysql_fetch_row(result); 
    while(row != NULL)    
    {
        for(int i=0; i<mysql_num_fields(result); i++)
            cout << row[i] << "\t\t";
        cout << endl;
        row = mysql_fetch_row(result);
    }
}

int main() {
    mysql_init(&mysql);
    if(!mysql_real_connect(&mysql,"localhost","root","root","face",0,NULL,0))
    {     
        cout << "connect fial\n";
        return -1;
    }
    float arr[32];
    ifstream inFile("fature.bin",ios::in|ios::binary);
    if(!inFile) {
        cout << "error" <<endl;
        return 0;
    }
    inFile.read((char*)arr, sizeof(arr));
    int readedBytes = inFile.gcount();
    cout << "readedBytes : " << readedBytes <<endl;
    //select();
    string s;
    s.resize(sizeof(arr));
    char* ptr = (char*)(s.data());
    memcpy(ptr, arr, sizeof(arr));
    cout << "s:" << endl << s << endl;
    add("NiuBenn", s);
    cout << "connect success\n";
    mysql_close(&mysql);
    return 0;
}
