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
#include <string.h>
#include <jsoncpp/json/json.h>

#include "Log.hpp"

#define random(x) (rand()%x)

//srand((int)time(0));

using namespace std;


struct face_encoding {
    string _data;
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
    cout << "hour1: " << hour1 << "  hour2: " << hour2 << endl;
    string minute1 = end_time.substr(3, 2), minute2 = old_end_time.substr(3, 2);
    cout << "minute1: " << minute1 << "  minute2: " << minute2 << endl;
    float hour = atoi(hour1.c_str()) - atoi(hour2.c_str());
    float minute = (atoi(minute1.c_str()) - atoi(minute2.c_str()))/60.0;
    cout << "hour: " << hour << " minute: " << minute;
    return hour + minute;
}


bool browse_check_info(MYSQL* mysql, string name, string id, string& info)
{
    LOG(INFO, "get browse check info");
    string sql = "select * from check_in_info_" + id + ";";
    LOG(INFO, "sql: " + sql);
    bool res = mysql_query(mysql, sql.c_str());
    if(res != 0) {
        LOG(WARNING, "select failed");
        return false;
    }
    MYSQL_RES* result = mysql_store_result(mysql);
    if(result == NULL) {
        LOG(WARNING,"store failed");
        return false;
    }
    info += "<!DOCTYPE html><html><head><meta charset=\"utf-8\" /></head><body><br /><h4>" + name;
    info += "</h4><table border=\"1\"><tr><th>日期</th><th>上班打卡时间</th><th>下班打卡时间</th><th>当天工作时长/h</th></tr>";

    MYSQL_ROW row = mysql_fetch_row(result);
    while(row != NULL) {
        string data = row[0];
        string start = row[1];
        string end = row[2];
        string work = row[3];
        info += "<tr><td>" + data + "</td><td>" + start + "</td><td>" + end + "</td><td>" + work + "</td></tr>";
        row = mysql_fetch_row(result);
    }
    info += "</table></body></html>";
    return true;
}

bool browse(MYSQL* mysql, string& data)
{
    LOG(INFO, "get browse data");
    Json::Value results;
    string sql = "select id,name,ename,sex,age from faceinfo;";
    LOG(INFO, "sql: " + sql);
    bool res = mysql_query(mysql, sql.c_str());
    if(res != 0) {
        LOG(WARNING, "select id,ename,feature failed");
        return false;
    }
    MYSQL_RES* result = mysql_store_result(mysql);
    if(result == NULL) {
        LOG(WARNING,"store id,name,ename,sex,age faceinfo failed");
        return false;
    }
    MYSQL_ROW row = mysql_fetch_row(result);
    while(row != NULL) {
        Json::Value res;
        string id = row[0];
        string name = row[1];
        string ename = row[2];
        string sex = row[3];
        string age = row[4];
        res["title"] = name + " " + ename;
        res["desc"] = "ID:" + id + " 性别:" + sex + " 年龄:" + age;
        res["url"] = "/browse?id=" + id + "&name=" + name ;
        results.append(res);
        row = mysql_fetch_row(result);
    }
    Json::FastWriter writer;
    string temp = writer.write(results);
    data += temp;
    return true;
}

bool get_init_data(MYSQL* mysql, string& init_data) 
{
    LOG(INFO, "get init data");
    Json::Value results;
    string sql = "select id,ename,feature from faceinfo;";
    LOG(INFO, "sql: " + sql);
    bool res = mysql_query(mysql, sql.c_str());
    if(res != 0) {
        LOG(WARNING, "select id,ename,feature failed");
        return false;
    }
    MYSQL_RES* result = mysql_store_result(mysql);
    if(result == NULL) {
        LOG(WARNING,"store id,ename,feature faceinfo failed");
        return false;
    }
    MYSQL_ROW row = mysql_fetch_row(result);
    init_data.resize(4);
    string feature;
    feature.resize(512);
    int count = 0;
    while(row != NULL) {
        Json::Value res;
        res["id"] = row[0];
        res["ename"] = row[1];
        results.append(res);
        string feature_path = row[2];
        FILE *feature_bin;
        feature_bin = fopen(feature_path.c_str(), "r");
        if(feature_bin == NULL) {
            cout  << "FILE OPEN ERR " << endl;
            LOG(WARNING, "FILE OPEN ERR " + feature_path);
            continue;
        }
        else {
            fread(&(feature[0]), 1, feature.size(), feature_bin);
            fclose(feature_bin);
        }
        init_data += feature;
        row = mysql_fetch_row(result);
        count++;
    }
    memcpy(&(init_data[0]), &count, 4);
    LOG(INFO, "feature count: " + count);
    Json::FastWriter writer;
    string temp = writer.write(results);
    init_data += temp;
    return true;
}

bool face_info_add(MYSQL* mysql, string& name, string& ename, string& sex, int age, string& enter_time, string& phone, string& addr, string& feature)
{
    LOG(INFO, "face info add");
    string sql = "insert into faceinfo(name, ename, sex, age, enter_time, phone, addr, feature) values('" + name + "','" + ename + "','" + sex + "','" + to_string(age) + "','" + enter_time + "','" + phone + "','" + addr + "','" + feature +  "');";
    LOG(INFO, "sql:" + sql + "");
    int res = mysql_query(mysql, sql.c_str());
    if(res != 0) {
        cout <<  mysql_error(mysql) << endl;
        LOG(WARNING, "add face_info failed");
    }
    else {
        LOG(INFO, "add face_info success");
    }
    return res == 0 ? true : false;
}

bool check_in_info_create(MYSQL* mysql, int id)
{
    LOG(INFO, "check_in info create");
    string sql = "create table check_in_info_" + to_string(id) + "(date date primary key not null, start_time time, end_time time, work_time float default 0);";
    LOG(INFO, "sql: " + sql);
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
    LOG(INFO, "check_in info add");
    int res = 0;
    MYSQL_ROW res_row;
    string sql;
    string date_time = get_time_s();
    int index = date_time.find(' ');
    string date = date_time.substr(0, index);
    string time = date_time.substr(index+1);
    //cout << "date:" << date << endl << "time:" << time << endl;

    sql = "select * from check_in_info_" + to_string(id) + " where date='" + date + "';";
    LOG(INFO, "sql: " + sql);
    res = mysql_query(mysql,sql.c_str());
    if(res != 0) {
        LOG(WARNING, "select date from check_in_info_" + to_string(id) + " failed");
        return false;
    }
    LOG(INFO, "select date from check_in_info_" + to_string(id) + " success");
    MYSQL_RES* result = mysql_store_result(mysql);
    if(result == NULL) {
        LOG(WARNING, "store date from check_in_info_" + to_string(id) + " failed");
        return false;
    }
    else if ((res_row = mysql_fetch_row(result)) == NULL) {
        sql = "insert into check_in_info_" + to_string(id) + "(date, start_time, end_time, work_time) values('" + date + "', '" + time + "', '" + time + "', 0.0);";
        LOG(INFO, "sql: " + sql);
        res = mysql_query(mysql,sql.c_str());
        if(res != 0) {
            LOG(WARNING, "add data for check_in_info_" + to_string(id) + " failed");
            return false;
        }
        LOG(INFO, "add data for check_in_info_" + to_string(id) + " success");
    }
    else {
        //res_row = mysql_fetch_row(result);
        string start_time = res_row[1];
        string old_end_time = res_row[2];
        float new_work_time = sub_time(time, start_time);
        sql = "update check_in_info_" + to_string(id) + " set end_time='" + time + "' , work_time=" + to_string(new_work_time) + " where date='" + date + "';";
        LOG(INFO, "sql: " + sql);
        res = mysql_query(mysql,sql.c_str());
        if(res != 0) {
            LOG(WARNING, "update data for check_in_info_" + to_string(id) + " failed");
            return false;
        }
        LOG(INFO, "update data for check_in_info_" + to_string(id) + " success");
        LOG(INFO, "update end_time: " + old_end_time + " to " + time + " work_time: " + to_string(new_work_time));
    }
    return true;
}

bool registered_face(MYSQL* mysql, string& name, string& ename, string& sex, int age, string& phone, string& addr, string& feature) {
    LOG(INFO, "registered face");
    string sql = "select max(id) from faceinfo;";
    bool res = mysql_query(mysql, sql.c_str());
    if(res != 0) {
        LOG(WARNING, "select max id failed");
        return false;
    }
    MYSQL_RES* result = mysql_store_result(mysql);
    if(result == NULL) {
        LOG(WARNING, "store max id faceinfo failed");
        return false;
    }
    MYSQL_ROW row = mysql_fetch_row(result);
    int id = atoi(row[0]);
    string ids = to_string(++id);

    string feature_path = "../Feature/feature_" + ids + ".bin";
    LOG(INFO, "feature_path: " + feature_path);
    FILE *feature_bin;
    feature_bin = fopen(feature_path.c_str(), "w");
    if(feature_bin == NULL) {
        cout  << "FILE OPEN ERR " << endl;
        return false;
    }
    else {
        fwrite(feature.data(), 1, feature.size(), feature_bin);
        fclose(feature_bin);
    }

    string enter_time = get_time_s();
    res = face_info_add(mysql, name, ename, sex, age, enter_time, phone, addr, feature_path);
    if(res == false) {
        LOG(WARNING, "add face info failed");
        return false;
    }
    LOG(INFO, "add face info success");
    res = check_in_info_create(mysql, id);
    if(res == false) {
        LOG(WARNING, "create check_in_info table failed");
        return false;
    }

    LOG(INFO, "registered face success");
    return true;
}
