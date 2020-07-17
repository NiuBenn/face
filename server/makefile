bin=server
cc=g++

$(bin):server.cc
	$(cc) -o $@ $^ -std=c++11 -lpthread  -lboost_system -ljsoncpp -L/usr/lib64/mysql -lmysqlclient 
