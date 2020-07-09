#include "httplib.h"
#include "../data/data.hpp"
#include  <cstring>

using namespace std;

map<string, face_encoding> m_face_encoding;

MYSQL mysql;   
MYSQL_ROW row;  
MYSQL_FIELD* field = NULL;       
MYSQL_RES* result;   

int main() {
    srand((int)time(0));
    mysql_init(&mysql);
    if(!mysql_real_connect(&mysql,"localhost","root","root","face",0,NULL,0))
    {     
        LOG(ERROR,"Failed to connect to database");
        return -1;
    }
    LOG(INFO, "Connection to database succeeded");

    int res = mysql_query(&mysql, "set names utf8");
    if(res != 0)
        LOG(ERROR, "Failed to set the encoding format of database");
    
    using namespace httplib;
    Server server;

    server.Get("/init", [](const Request& req,
                Response& res){
        LOG(INFO, "Get Init");
        string result;
        bool ret = get_init_data(&mysql, result);
        res.set_content(result, "text/html");
    });

    server.Get("/submit.html", [](const Request& req,
                Response& res){
        LOG(INFO, "get submit.html");
        ifstream file("./html/submit.html");
        string one_line;
        string result;

        if(!file.is_open())
            res.set_content("file open error", "text/plain");
        else {
            while(getline(file,one_line)) 
                result = result + one_line + "\n";
            file.close();
            res.set_content(result, "text/html");
        }
    });

    server.Post("/create", [](const Request& req,
                Response& res){
                LOG(INFO, "Get Create Submit!");
                string name = req.get_param_value("name");
                string ename = req.get_param_value("ename");
                string sex = req.get_param_value("sex");
                string age = req.get_param_value("age");
                string phone = req.get_param_value("phone");
                string addr = req.get_param_value("addr");
                string code = req.get_param_value("code");
                LOG(INFO, "submit name: " + name);
                LOG(INFO, "submit ename: " + ename);
                LOG(INFO, "submit sex: " + sex);
                LOG(INFO, "submit age: " + age);
                LOG(INFO, "submit phone: " + phone);
                LOG(INFO, "submit addr: " + addr);
                LOG(INFO, "submit code: " + code);
                ifstream file;
                if(m_face_encoding.count(code) > 0) {
                    time_t now_time = get_time();
                    if(now_time - m_face_encoding[code]._time > 600){
                        LOG(INFO, "code time out");
                        file.open("./html/timeout.html");
                    }
                    else {
                        string& data = m_face_encoding[code]._data;
                        bool ret = registered_face(&mysql, name, ename, sex, atoi(age.c_str()), phone, addr, data);
                        if (ret) {
                            LOG(INFO, "Registration successful");
                            file.open("./html/success.html");
                        }
                        else {
                            LOG(INFO, "Registration failed");
                            file.open("./html/failed.html");
                        }
                    }
                    m_face_encoding.erase(code);
                }
                else {
                    LOG(INFO, "Invalid verification code");
                    file.open("./html/invalid.html");
                }
                string one_line;
                string result;
                if(!file.is_open())
                    res.set_content("file open error", "text/plain");
                else {
                    while(getline(file,one_line)) 
                        result = result + one_line + "\n";
                    file.close();
                    res.set_content(result, "text/html");
                }
             });

    server.Post("/registered_face", [](const Request& req,
                Response& res){
            LOG(INFO, "Get registered_face");
            string body = req.body;
            string number = get_number();
            LOG(INFO, "code: " + number);
            m_face_encoding[number]._data = body;
            m_face_encoding[number]._time = get_time();
            
            res.set_content(number, "text/html");
    });
   
    
    server.Post("/clock", [](const Request& req,
                Response& res){
            LOG(INFO, "Get clock");
            string id = req.get_param_value("id");
            LOG(INFO, "id: " + id);
            bool ret = check_in_info_add(&mysql, atoi(id.c_str()));
            if(ret == true)
                res.set_content("SUCCESS", "text/html");
            else
                res.set_content("FAILED", "text/html");
    });

    server.Get("/index.html", [](const Request& req,
                Response& res){
                std::ifstream file("./html/index.html");
                std::string one_line;
                std::string result;
                
                if(!file.is_open())
                {
                    res.set_content("file open error", "text/plain");
                }
                else
                {
                    while(std::getline(file,one_line))
                    {
                        result = result + one_line + "\n";
                    }
                    file.close();
                    res.set_content(result, "text/html");
                }
            });
    
    server.Get("/search", [](const Request& req,
                Response& res){
                std::string query = req.get_param_value("query");
                std::string result;
                std::cout << "Get query: " << query <<std::endl;
                bool ret = browse(&mysql, result);
                res.set_content(result, "application/json");
            });
    
    server.Get("/browse", [](const Request& req,
                Response& res){
                std::string id = req.get_param_value("id");
                std::string name = req.get_param_value("name");
                cout << "id:" << id << " name:" << name << endl;
                string result;
                bool ret = browse_check_info(&mysql, name, id, result);
                res.set_content(result, "text/html");
            });

    server.listen("0.0.0.0", 9000);

    mysql_close(&mysql);
}
