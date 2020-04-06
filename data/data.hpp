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
#define random(x) (rand()%x)

srand((int)time(0));

struct face_encoding {
    float[256] _data;
    std::time_t _time;
};

std::string get_number() {
    std::string result = "000000";
    for(int i = 0; i < 6; ++i){
        result[i] += random(10);
    }
    return result;
}

std::time_t get_time(){
    return std::time(0);
}

bool registered_face(MYSQL* mysql, char* data, std::string name, std::string id) {
    return true;
}