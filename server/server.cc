#include "httplib.h"

int main() {
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
    
    server.listen("0.0.0.0", 9000);
}
