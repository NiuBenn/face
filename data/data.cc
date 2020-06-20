#include <iostream>
#include <string> 
#include <cstring>
#include <stack> 
#include <algorithm>
#include <sstream>
#include <fstream>
#include <mysql/mysql.h>
#include <unistd.h>

#include "data.hpp"

using namespace std;

MYSQL mysql;   
MYSQL_ROW row;  
MYSQL_FIELD* field = NULL;       
MYSQL_RES* result;       

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
    int a = 10;
    string temp = to_string(a);
    cout << temp << endl;
    mysql_init(&mysql);
    if(!mysql_real_connect(&mysql,"localhost","root","root","face",0,NULL,0))
    {     
        cout << "connect fial\n";
        return -1;
    }
    cout << "connect success\n";
    int res = mysql_query(&mysql, "set names utf8");
    if(res != 0)
        cout << "设置编码格式失败" <<endl;
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
    string name = "小王", sex = "男", time = get_time_s(), phone = "15829640914", addr = "西安市";
    bool ret = registered_face(&mysql, name, sex, 22, phone, addr, s);
    //cout << "s:" << endl << s << endl;
    //add("NiuBenn", s);
    //check_in_info_create(&mysql, 4);
    check_in_info_add(&mysql, 53);
    mysql_close(&mysql);
    return 0;
}
