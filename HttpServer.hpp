#ifndef HTTPSERVER_H
#define HTTPSERVER_H

#include <sys/socket.h>
#include <string>

namespace HTTPServer{

    class HttpServer{
    public:
        //default constructor (creates a new server set up on 127.0.0.1:8080 with default response to all requests)
        HttpServer();
        //creates a new HTTP server object at the specifed address and port
        HttpServer(std::string address, std::string port);
        //starts the http server accepting http requests
        bool start();
        //closes the server
        bool stop();
        //a function that handles the http response (should be overwritten when creating a child class for your server application)
        bool handleResponse(HttpRequest req, char** response);
        
    private:
        //stores the socket accosiated with the http server
        int sockFD

        //processes the http request and calls handleResponse to generate the apropriate http response
        bool processRequest();

    }

    class HttpRequest{
    public:


        char* MainHeader;
        char* SubHeaders;
        char* Content;

        char* raw;
    }

}

#endif