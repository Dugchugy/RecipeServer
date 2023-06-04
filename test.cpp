#include "HttpServer.hpp"
#include <iostream>
#include <unistd.h>
#include <sys/socket.h>

using namespace HTTPServer;

int main(){
    std::cout << "starting http sever\n";

    HttpServer server("127.0.0.1".c_str(), "25565".c_str());

    server.start();

    sleep(120);

    std::cout << "stoppinmg server\n";

    server.stop();
}