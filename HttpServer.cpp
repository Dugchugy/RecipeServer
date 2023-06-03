#include "HttpServer.hpp"
#include <thread>
#include <string>
#include <cstring>
#include <sys/socket.h>

namespace HTTPServer
{
    //creates a default http server (a server on 127.0.0.1:12345)
    HttpServer::HttpServer() : HttpServer::HttpServer("127.0.0.1", 12345) {

    }

    HttpServer::HttpServer(char* address, char* port){

        //sets the handlerthread to null
        HandlerThread = NULL;

        //sets continue processing to false
        continueProcessing = false;

        //defines a structure to store the parameters used to find the address
        struct addrinfo hints;
        struct addrinfo *serverAddress;

        //wipes the structure to all 0 and them loads it with the desired parameters
        memset(&hints, 0, sizeof(hints))

        hints.ai_family = AF_UNSPEC;
        hints.ai_socktype = SOCK_STREAM;
        hints.ai_flags = AI_PASSIVE;

        //attempts to generate a valid address from the provided IP and port
        if(getaddrinfo(address, port, &hints, &serverAddress) != 0){
            throw "failed to find address";
        }

        //creates a new socket to use for the server usiing the data found by getting the address
        sockFD = socket(serverAddress->ai_family, serverAddress->ai_socktype, serverAddress->ai_protocol);

        //checks if the socket initiated correctly (if not, throws an error)
        if(sockFD < 0){
            throw "failed";
        } 

        //attempts to bind the socket to the address found in the ealier step
        if(bind(sockFD, serverAddress->ai_addr, serverAddress->ai_addrlen) < 0){
            throw "bind failed";
        }

        //setup is complete

        //discards the found address object (its no longer needed)
        freeaddrinfo(serverAddress);

    }

    bool HttpServer::start(){

        //checks if the handler thread is not null (server is already started)
        if(HandlerThread != NULL){
            //returns false (server has already started)
            return false;
        }

        //starts the http server listener with a maximum queue of 5 devices
        if(listen(sockFD, 5) < 0){
            throw "failed to open for listening";
        }

        //allows the handler loop to run
        continueProcessing = true;

        //starts a thread to handle the server interactions
        HandlerThread = std::thread(mainHandleLoop);

        return true;
    }

    bool HttpServer::stop(){

        //prevents the handler loop from continuing
        continueProcessing = false;

        //closes the server socket
        shutdown(sockFD, SHUT_RDWR);

        //waits for the current interation of the handling loop to finish
        HandlerThread.join();

        //sets the handler thread to null
        HandlerThread = NULL;

        return true;
    }

    bool HttpServer::mainHandleLoop(){
        try{
            //loops until the bool signaling to process requests is set to false
            while(continueProcessing){
                //creates an int to store the socket for the incoming request
                int newSocket;

                //creates a socketaddress object to store the address of the incoming socket (not used as http server doesn't filter addresses)
                struct sockaddr_storage incomingAddr;
                socklen_t incomingAddrSize = sizeof(incomingAddr);

                //attempts to establish a connection with a incoming connection request
                if((newSocket = accept(sockFD, (struct sockaddr*) &incomingAddr, &incomingAddrSize)) < 0){
                    throw "failed to accept connection";
                }

                //creates a char pointer to use to store the raw request data
                char* rawRequestData = NULL;

                //reads the incoming request from the user
                read(newSocket, rawRequestData, 4096);

                //creates a new http request
                HttpRequest req;

                //reads the request from the raw data
                req.parseSocketInput(rawRequestData);

                char* response = NULL;

                //generates the http response using the given algorithm
                handleResponse(req, response);

                //sends the response to the requester
                send(newSocket, response, strlen(response), 0);
                
                //closes the socket
                close(newSocket);
            }
        }catch(std::string s){
            //checks if this is the expected error called when the server shuts down
            if(!(s == "failed to accept connection" && (!continueProcessing))){
                //if not re-throws the error
                throw "failed to accept connection";
            }
        }

        return true
    }

    bool httpServer::handleResponse(HttpRequest req, (char*)& response){

        //generates a plaintext response that writes "hello world"
        std::string rawResp = "HTTP/1.1 200 OK\r\nContent-Type: test/plain\r\nContent-Length: 11\r\n\r\nhello world";

        //converts that responmse to a char array and returns it
        response = rawResp.c_str();

        return true;
    }

    HttpServer::~HttpServer(){
        //calls the stop server function
        stop();
    }
} // namespace HttpServer
