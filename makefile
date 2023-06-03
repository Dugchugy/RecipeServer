all: test

test: test.o HttpServer.o
	g++ -o test test.o HttpServer.o

HttpServer.o: HttpServer.cpp HttpServer.hpp
	g++ -g -c HttpServer.cpp

test.o: test.cpp HttpServer.hpp
	g++ -g -c test.cpp