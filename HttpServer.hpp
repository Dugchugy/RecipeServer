#ifndef HTTPSERVER_H
#define HTTPSERVER_H

#include <sys/socket.h>
#include <string>
#include <thread>

namespace HTTPServer{

    class HttpRequest{
    public:

        //creates a new http request with no data in it
        HttpRequest();

        //creates a copy of the passed HttpRequest
        HttpRequest(const HttpRequest& ref);

        //destroys the http request object
        ~HttpRequest();

        //reads the raw data from a socket and uses it to generate http request data
        bool parseSocketInput(char* data);

        char* MainHeader;
        char* SubHeaders;
        char* Content;

        char* raw;
    };

    class HttpServer{
    public:
        //default constructor (creates a new server set up on 127.0.0.1:8080 with default response to all requests)
        HttpServer();
        //creates a new HTTP server object at the specifed address and port
        HttpServer(char* address, char* port);
        //a destructor that shuts down the server if the program ends before stop is called
        ~HttpServer();
        //starts the http server accepting http requests
        bool start();
        //closes the server
        bool stop();
        //a function that handles the http response (should be overwritten when creating a child class for your server application)
        bool handleResponse(HttpRequest req, char* &response);

        //a bool to signal whether or not the server should run
        bool continueProcessing;

        // a looping function that handles all the requests to the server
        void mainHandleLoop();
        
    private:
        //stores the socket accosiated with the http server
        int sockFD;

        bool running;

        //a thread that runs the main handler loop for the server
        std::thread HandlerThread;

        //processes the http request and calls handleResponse to generate the apropriate http response
        bool processRequest(int newSock);

    };

}

#endif