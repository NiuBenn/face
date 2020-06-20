#include <iostream>
#include <map>
#include <string>
#include <stack> 
#include <algorithm>
#include <sstream>
#include <mysql/mysql.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <ctime>
#include <sys/time.h>

#include "Log.hpp"

#define random(x) (rand()%x)

//srand((int)time(0));

using namespace std;


struct face_encoding {
    float _data[256];
    std::time_t _time;
};

string get_number() {
    std::string result = "000000";
    for(int i = 0; i < 6; ++i){
        result[i] += random(10);
    }
    return result;
}

time_t get_time(){
    return time(0);
}

string get_time_s()
{
    struct timeval nowtime;
    gettimeofday(&nowtime, NULL);
    char ntime[32];
    time_t tt = nowtime.tv_sec;
    struct tm* ttime;
    ttime = localtime(&tt);
    strftime(ntime,64,"%Y-%m-%d %H:%M:%S",ttime);
    string result = ntime;
    return result;
}

float sub_time(string& end_time, string& old_end_time)
{
    string hour1 = end_time.substr(0, 2), hour2 = old_end_time.substr(0, 2);
    string minute1 = end_time.substr(3, 2), minute2 = old_end_time.substr(3, 2);
    return atoi(hour1.c_str()) - atoi(hour2.c_str()) + (atoi(minute1.c_str()) - atoi(minute2.c_str()))/60.0;
}

bool face_info_add(MYSQL* mysql, string& name, string& sex, int age, string& enter_time, string& phone, string& addr, string& feature)
{
    string sql = "insert into faceinfo(name, sex, age, enter_time, phone, addr, feature) values('" + name + "','" + sex + "','" + to_string(age) + "','" + enter_time + "','" + phone + "','" + addr + "','" + feature +  "');";
    LOG(INFO, "sql:\n" + sql + "\n");
    int res = mysql_query(mysql, sql.c_str());
    if(res != 0) {
        LOG(WARNING, "add face_info failed");
    }
    else {
        LOG(INFO, "add face_info success");
    }
    return res == 0 ? true : false;
}

bool check_in_info_create(MYSQL* mysql, int id)
{
    string sql = "create table check_in_info_" + to_string(id) + "(date date primary key not null, start_time time, end_time time, work_time float default 0);";
    cout << "sql : " << endl << sql << endl;
    int res = mysql_query(mysql, sql.c_str());
    if(res != 0) {
        LOG(WARNING, "create check_in_info_" + to_string(id) + " failed");
        //cout << "create check_in_info_" << id << " failed" << endl;
    }
    else {
        LOG(INFO, "create check_in_info_" + to_string(id) + " success");
        //cout << "create check_in_info_" << id << " success" << endl;
    }
    return res == 0 ? true : false;
}

bool check_in_info_add(MYSQL* mysql, int id)
{
    int res = 0;
    MYSQL_ROW res_row;
    string sql;
    string date_time = get_time_s();
    int index = date_time.find(' ');
    string date = date_time.substr(0, index);
    string time = date_time.substr(index+1);
    //cout << "date:" << date << endl << "time:" << time << endl;

    sql = "select * from check_in_info_" + to_string(id) + " where date='" + date + "';";
    cout << "sql:\n" << sql << endl;
    LOG(INFO, "sql:\n" + sql + "\n");
    res = mysql_query(mysql,sql.c_str());
    if(res != 0) {
        LOG(WARNING, "select date from check_in_info_" + to_string(id) + " failed");
        cout << " select date from check_in_info_" << id << " failed" << endl;
        return false;
    }
    LOG(INFO, "select date from check_in_info_" + to_string(id) + " success");
    cout << " select date from check_in_info_" << id << " success" << endl;
    MYSQL_RES* result = mysql_store_result(mysql);
    if(result == NULL) {
        LOG(WARNING, "store date from check_in_info_" + to_string(id) + " failed");
        cout << "store date from check_in_info_" << id << " failed" << endl;
        return false;
    }
    else if ((res_row = mysql_fetch_row(result)) == NULL) {
        sql = "insert into check_in_info_" + to_string(id) + "(date, start_time, end_time, work_time) values('" + date + "', '" + time + "', '" + time + "', 0.0);";
        LOG(INFO, "sql:\n" + sql + "\n");
        cout << "sql:\n" << sql << endl;
        res = mysql_query(mysql,sql.c_str());
        if(res != 0) {
            LOG(WARNING, "add data for check_in_info_" + to_string(id) + " failed");
            cout << "add data for check_in_info_" << id << " failed" << endl;
            return false;
        }
        LOG(INFO, "add data for check_in_info_" + to_string(id) + " success");
        cout << "add data for check_in_info_" << id << " success" << endl;
    }
    else {
        //res_row = mysql_fetch_row(result);
        string old_end_time = res_row[2];
        float new_work_time = sub_time(time, old_end_time);
        sql = "update check_in_info_" + to_string(id) + " set end_time='" + time + "' , work_time=" + to_string(new_work_time) + "' where date='" + date + "');";
        LOG(INFO, "sql:\n" + sql + "\n");
        cout << "sql:\n" << sql << endl;
        res = mysql_query(mysql,sql.c_str());
        if(res != 0) {
            LOG(WARNING, "update data for check_in_info_" + to_string(id) + " failed");
            cout << " update data for check_in_info_" << id << " failed" << endl;
            return false;
        }
        LOG(INFO, "update data for check_in_info_" + to_string(id) + " success");
        LOG(INFO, "update end_time: " + old_end_time + " to " + time + " work_time: " + to_string(new_work_time));
        cout << "update data for check_in_info_" << id << " success" << endl;
    }
    return true;
}

bool registered_face(MYSQL* mysql, string& name, string& sex, int age, string& phone, string& addr, string& feature) {
    string enter_time = get_time_s();
    bool res = face_info_add(mysql, name, sex, age, enter_time, phone, addr, feature);
    if(res == false) {
        LOG(WARNING, "add face info failed");
        cout << "add face info failed" << endl;
        return false;
    }
    LOG(INFO, "add face info success");
    cout << "add face info success" << endl;

    string sql = "select max(id) from faceinfo;";
    res = mysql_query(mysql, sql.c_str());
    if(res != 0) {
        LOG(WARNING, "select max id failed");
        cout << "select max id failed" << endl;
        return false;
    }
    MYSQL_RES* result = mysql_store_result(mysql);
    if(result == NULL) {
        LOG(WARNING, "store max id faceinfo failed");
        cout << "store max id faceinfo failed" << endl;
        return false;
    }
    MYSQL_ROW row = mysql_fetch_row(result);
    string id = row[0];

    res = check_in_info_create(mysql, atoi(id.c_str()));
    if(res == false) {
        LOG(WARNING, "create check_in_info table failed");
        cout << "create check_in_info table failed" << endl;
        return false;
    }

    LOG(INFO, "registered face success");
    cout << "registered face success" << endl;
    return true;
}
