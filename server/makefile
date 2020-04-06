bin=server
cc=g++

$(bin):server.cc
	$(cc) -o $@ $^ -std=c++11 -lpthread  -lboost_system -ljsoncpp -I ../../cppjieba/include -I ../../cppjieba/deps 
