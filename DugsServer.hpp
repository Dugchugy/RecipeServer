#ifndef DUGS_SERVER_H
#define DUGS_SERVER_H

#include "HttpServer.hpp"
#include <string>

namespace RecipeServer{

    class RecipeServer: public HTTPServer::HttpServer{

    public:
        //constructs a new recipes server object
        RecipeServer();

        //constructs a new recipe server object with the given IP and port
        RecipeServer(std::string address, std::string port);

        //a function that handles the http response
        virtual bool handleResponse(const HTTPServer::HttpRequest &req, std::string &response);

        //a seperate function to ahndle specifically the http get requests
        bool handleGetRequest(const HTTPServer::HttpRequest & req, std::string &response, std::string path);

    }; //end of recipeServer class

}//end of RecipeServer namespace

#endif