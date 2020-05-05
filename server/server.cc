#include "httplib.h"
#include "../data/data.hpp"
#include  <cstring>

using namespace std;

std::map<string, face_encoding> m_face_encoding;

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

    using namespace httplib;
    Server server;

    server.Get("/submit.html", [](const Request& req,
                Response& res){
        std::ifstream file("./submit.html");
        std::string one_line;
        std::string result;

        if(!file.is_open())
            res.set_content("file open error", "text/plain");
        else {
            while(std::getline(file,one_line)) 
                result = result + one_line + "\n";
            file.close();
            res.set_content(result, "text/html");
        }
    });

    server.Post("/create", [](const Request& req,
                Response& res){
                std::string name = req.get_param_value("name");
                std::string id = req.get_param_value("id");
                std::string number = req.get_param_value("number");
                std::cout << "Get Submit!" <<std::endl;

                if(m_face_encoding.count(number) > 0) {
                    time_t now_time = get_time();
                    if(now_time - m_face_encoding[number]._time > 600){
                        res.set_content("注册码已失效", "text/html");
                    }
                    else {
                        char* data = (char*)(m_face_encoding[number]._data);
                        bool ret = registered_face(&mysql, data, name, id);
                        if (ret) {
                            res.set_content("注册成功", "text/html");
                        }
                    }
                    m_face_encoding.erase(number);
                }
                else {
                    res.set_content("无效验证码", "text/html");
                }
             });

    server.Post("/registered_face", [](const Request& req,
                Response& res){
            std::string body = req.body;
            std::string number = get_number();
            
            memcmp(m_face_encoding[number]._data, body.data(), body.length());
            m_face_encoding[number]._time = get_time();
            
            res.set_content(number, "text/html");
    });
    
    server.listen("0.0.0.0", 9000);

    mysql_close(&mysql);
}
