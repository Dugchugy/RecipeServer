#include "HttpServer.hpp"
#include <thread>
#include <string>
#include <cstring>
#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>
#include <iostream>
//#include <sys\types.h>

namespace HTTPServer
{
    //creates a default http server (a server on 127.0.0.1:12345)
    HttpServer::HttpServer() : HttpServer::HttpServer("127.0.0.1", "12345") {

    }

    HttpServer::HttpServer(std::string address, std::string port){

        //marks that the server is not running
        running = false;

        //sets continue processing to false
        continueProcessing = false;

        //defines a structure to store the parameters used to find the address
        struct addrinfo hints;
        struct addrinfo *serverAddress;

        //wipes the structure to all 0 and them loads it with the desired parameters
        memset(&hints, 0, sizeof(hints));

        hints.ai_family = AF_UNSPEC;
        hints.ai_socktype = SOCK_STREAM;
        hints.ai_flags = AI_PASSIVE;

        //attempts to generate a valid address from the provided IP and port
        if(getaddrinfo(address.c_str(), port.c_str(), &hints, &serverAddress) != 0){
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

        //checks if the server is running
        if(running){
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
        HandlerThread = std::thread(&HttpServer::mainHandleLoop, this);

        return true;
    }

    bool HttpServer::stop(){

        //prevents the handler loop from continuing
        continueProcessing = false;

        //closes the server socket
        close(sockFD);

        //waits for the current interation of the handling loop to finish
        HandlerThread.join();

        running = false;

        return true;
    }

    void HttpServer::mainHandleLoop(){
        try{
            //loops until the bool signaling to process requests is set to false
            while(continueProcessing){
                //creates an int to store the socket for the incoming request
                int newSocket;

                //creates a socketaddress object to store the address of the incoming socket (not used as http server doesn't filter addresses)
                struct sockaddr_storage incomingAddr;
                socklen_t incomingAddrSize = sizeof(incomingAddr);

                std::cout << "awaiting connection\n";

                //attempts to establish a connection with a incoming connection request
                if((newSocket = accept(sockFD, (struct sockaddr*) &incomingAddr, &incomingAddrSize)) < 0){
                    std::cout << "accept failed\n";
                    
                    throw "failed to accept connection";
                }

                //creates a char pointer to use to store the raw request data
                char* rawRequestData = NULL;

                std::cout << "reading request\n";

                //reads the incoming request from the user
                int dLen = read(newSocket, rawRequestData, 4096);

                std::cout << "read " << dLen << "bytes";

                //creates a new http request
                HttpRequest req;

                std::cout << "processing request\n";

                //reads the request from the raw data
                req.parseSocketInput(rawRequestData, dLen);

                char* response = NULL;

                std::cout << "generating response\n";

                //generates the http response using the given algorithm (allocates memory for response)
                handleResponse(req, response);

                std::cout << "sending response\n";

                //sends the response to the requester
                send(newSocket, response, strlen(response), 0);

                delete response;

                response = NULL;
                
                std::cout << "closing connection\n";

                //closes the socket
                close(newSocket);
            }
        }catch(std::string s){
            //checks if this is the expected error called when the server shuts down
            if(continueProcessing){
                //if not re-throws the error
                throw "failed to accept connection";
            }
        }
    }

    bool HttpServer::handleResponse(HttpRequest req, char* &response){

        //generates a plaintext response that writes "hello world"
        std::string rawResp = "HTTP/1.1 200 OK\r\nContent-Type: test/plain\r\nContent-Length: 11\r\n\r\nhello world";

        //allocates memory for the response
        response = new char[rawResp.size() + 1];

        //copies the raw char values of the raw response to the response str
        memcpy(response, rawResp.c_str(), rawResp.size() + 1);

        //prints the request
        std::cout << req;

        return true;
    }

    HttpServer::~HttpServer(){
        //calls the stop server function
        stop();
    }

    HttpRequest::HttpRequest(){
        //sets memLengths to 4 -1s
        memLengths = new int[4] {-1, -1, -1, -1};

        //initializes all the char arrays to null
        raw = NULL;
        MainHeader = NULL;
        SubHeaders = NULL;
        Content = NULL;
    }

    HttpRequest::~HttpRequest(){
        //checks if raw is defined and if not, deletes it
        if(raw != NULL){
            delete raw;
            raw = NULL;
        }

        //checks if mainHeader is defined and if not, deletes it
        if(MainHeader != NULL){
            delete MainHeader;
            MainHeader = NULL;
        }

        if(SubHeaders != NULL){
            delete SubHeaders;
            SubHeaders = NULL;
        }

        if(Content != NULL){
            delete Content;
            Content = NULL;
        }
    }

    HttpRequest::HttpRequest(const HttpRequest& ref){
        //allocates memory for the new memLengths
        memLengths = new int[4];

        //copies the memlengths
        memcpy(memLengths, ref.memLengths, sizeof(int) * 4);

        if(memLengths[0] < 1){

            raw = NULL;

        }else{

            raw = new char[memLengths[0]];

            memcpy(raw, ref.raw, sizeof(char) * memLengths[0]);

        }

        if(memLengths[1] < 1){

            MainHeader = NULL;

        }else{

            MainHeader = new char[memLengths[1]];

            memcpy(MainHeader, ref.MainHeader, sizeof(char) * memLengths[0]);

        }

        if(memLengths[2] < 1){

            SubHeaders = NULL;

        }else{

            SubHeaders = new char[memLengths[2]];

            memcpy(SubHeaders, ref.SubHeaders, sizeof(char) * memLengths[0]);

        }

        if(memLengths[3] < 1){

            Content = NULL;

        }else{

            Content = new char[memLengths[3]];

            memcpy(Content, ref.Content, sizeof(char) * memLengths[0]);

        }
    }

    bool HttpRequest::parseSocketInput(const char* data, int dLen){

        //resets the request object to an empty request object
        *this = HttpRequest();

        //defiens an int used to store where to read from
        int lastRead = 0;

        //creates bool to store whether or not the main and sub headers had been read
        bool FMHead = false;
        bool FSHead = false;

        std::cout << "reading chars\n";

        //loops through each char passed
        for(int i = 1; i < dLen; i++){

            //checks if this is the end of the line and first header hasn't been read
            //this reads the first header
            if((!FMHead) && data[i] == '\n' && data[i - 1] == '\r'){

                std::cout << "storing main header\n";

                //marks that the main headers has been found
                FMHead = true;

                //stores the length of the main header
                memLengths[1] = (dLen - lastRead) + 1;

                //allocates space to copy the main header
                MainHeader = new char[i + 1];

                //copies the data from the main header
                memcpy(MainHeader, data, sizeof(char) * (i + 1));

                //stores that the last read point was here
                lastRead = i;
            }

            //checks if this is the end of the headers and the start of the content
            //uses this to copy the sub headers
            if(FMHead && (!FSHead) && data[i] == '\n' && data[i - 1] == '\r' && data[i - 2] == '\n' && data[i - 3] == '\r'){

                std::cout << "storing subheaders\n";

                //marks that the sub headers have been found
                FSHead = true;

                //stores the length of the subheaders
                memLengths[2] = (dLen - lastRead) + 1;

                //allocates space to copy the sub headers
                SubHeaders = new char[(i - lastRead) + 1];

                //copies the data from the sub headers (start at the last read point and reads up to current point)
                memcpy(SubHeaders, &data[lastRead], ((i - lastRead) + 1));

                lastRead = i;
            }
        }

        //checks if the main header was not found (if not, something went wrong, throw error)
        if(!FMHead){
            std::cout << "unnabled to find main header\n";

            throw "no main header found";
        }else{
            //check if the subheaders were not found
            //if not, the request has no content so entirty of request should be read as subheaders
            if(!FSHead){

                std::cout << "storing subheaders (no Content)\n";

                //stores the length of the subheaders
                memLengths[2] = (dLen - lastRead) + 1;

                //allocates space to copy the sub headers
                SubHeaders = new char[(dLen - lastRead) + 1];

                //copies the data from the sub headers (start at the last read point and reads up to current point)
                memcpy(SubHeaders, &data[lastRead], ((dLen - lastRead) + 1));

            }else{//both main and sub headers were found, so the rest or the request is content

                std::cout << "storing Content";

                //stores the length of the content
                memLengths[3] = (dLen - lastRead) + 1;

                //allocates space to copy the sub headers
                Content = new char[(dLen - lastRead) + 1];

                //copies the data from the sub headers (start at the last read point and reads up to current point)
                memcpy(Content, &data[lastRead], ((dLen - lastRead) + 1));

            }
        }

        std::cout << "storing raw request\n";

        //store the length of the raw request
        memLengths[0] = dLen;

        //copies the entire request into raw
        memcpy(raw, data, dLen);

        std::cout << "finished processing\n";

        return true;

    }

    std::ostream& operator <<(std::ostream& os, const HttpRequest& req){

        os << "MainHeader:\n" << req.MainHeader;
        os << "SubHeaders:\n" << req.SubHeaders;
        os << "Content:\n" << req.Content;
        os << "\n\nRaw Request:\n" << req.raw << "\n";

        return os;
    }
} // namespace HttpServer
