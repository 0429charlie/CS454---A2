all: stringServer.cpp stringClient.cpp
	g++ -pthread -o stringServer stringServer.cpp
	g++ -pthread -o stringClient stringClient.cpp
