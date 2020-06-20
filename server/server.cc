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
        cout << "connect fial\n";
        return -1;
    }
    cout << "connect success\n";
    
    using namespace httplib;
    Server server;

    server.Get("/submit.html", [](const Request& req,
                Response& res){
        ifstream file("./submit.html");
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
                cout << "Get Submit!" << endl;
                string name = req.get_param_value("name");
                string sex = req.get_param_value("sex");
                string age = req.get_param_value("age");
                string phone = req.get_param_value("phone");
                string addr = req.get_param_value("addr");
                string code = req.get_param_value("code");
                cout << "name: " << name << endl;
                cout << "sex: " << sex << endl;
                cout << "age: " << age << endl;
                cout << "phone: " << phone << endl;
                cout << "addr: " << addr << endl;
                cout << "code: " << code << endl;
               
                if(m_face_encoding.count(code) > 0) {
                    time_t now_time = get_time();
                    if(now_time - m_face_encoding[code]._time > 600){
                        res.set_content("注册码已失效", "text/html");
                    }
                    else {
                        char* data = (char*)(m_face_encoding[code]._data);
                        /*bool ret = registered_face(&mysql, data, name, id);
                        if (ret) {
                            res.set_content("注册成功", "text/html");
                        }*/
                    }
                    m_face_encoding.erase(code);
                }
                else {
                    res.set_content("无效验证码", "text/html");
                }
             });

    server.Post("/registered_face", [](const Request& req,
                Response& res){
            string body = req.body;
            string number = get_number();
            
            memcmp(m_face_encoding[number]._data, body.data(), body.length());
            m_face_encoding[number]._time = get_time();
            
            res.set_content(number, "text/html");
    });
    
    server.listen("0.0.0.0", 9000);

    mysql_close(&mysql);
}
