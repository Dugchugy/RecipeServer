#ifndef DUGS_SERVER_H
#define DUGS_SERVER_H

#include "HttpServer.hpp"
#include <string>
//adds the postgre sql library
#include <postgresql/libpq-fe.h>

namespace RecipeServer{

    class RecipeServer: public HTTPServer::HttpServer{

    public:
        //constructs a new recipes server object
        RecipeServer();

        //constructs a new recipe server object with the given IP and port
        RecipeServer(std::string address, std::string port);

        //destructor
        ~RecipeServer();

        //a function that handles the http response
        virtual bool handleResponse(const HTTPServer::HttpRequest &req, std::string &response);

        //a seperate function to ahndle specifically the http get requests
        bool handleGetRequest(const HTTPServer::HttpRequest & req, std::string &response, std::string path);

        //a sperate function to handle the post requests made to the server (these are mostly used for manipulating the recipes)
        bool handlePostRequest(const HTTPServer::HttpRequest & req, std::string &response, std::string path);

        bool handleSQLRequest(const HTTPServer::HttpRequest & req, std::string &response, const std::string& path, const std::string& Content);

    }; //end of recipeServer class

    bool writeFile(std::string path, std::string content);

    std::string dumpFile(std::string path);

}//end of RecipeServer namespace

#endif