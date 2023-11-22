all: test RecipeServer

test: test.o HttpServer.o
	g++ -o test test.o HttpServer.o

RecipeServer: main.o HttpServer.o DugsServer.o
	g++ -o RecipeServer main.o HttpServer.o DugsServer.o -lstdc++fs -lpq

HttpServer.o: HttpServer.cpp HttpServer.hpp
	g++ -g -c HttpServer.cpp

test.o: test.cpp HttpServer.hpp
	g++ -g -c test.cpp

DugsServer.o: DugsServer.cpp DugsServer.hpp HttpServer.hpp
	g++ -g -c DugsServer.cpp -lstdc++fs -lpq

main.o: main.cpp DugsServer.hpp
	g++ -g -c main.cpp

clean:
	rm -r -f *.o test RecipeServer