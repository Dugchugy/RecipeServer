#include "DugsServer.hpp"
#include <string>
#include <regex>
#include <fstream>
#include <sstream>
#include <iostream>
#include <filesystem>
//adds the needed json functionality
#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>
//adds the postgre sql library
#include <postgresql/libpq-fe.h>

//attempts to include filesystem
#ifndef __has_include
  static_assert(false, "__has_include not supported");
#else
#  if __cplusplus >= 201703L && __has_include(<filesystem>)
#    include <filesystem>
     namespace fs = std::filesystem;
#  elif __has_include(<experimental/filesystem>)
#    include <experimental/filesystem>
     namespace fs = std::experimental::filesystem;
#  elif __has_include(<boost/filesystem.hpp>)
#    include <boost/filesystem.hpp>
     namespace fs = boost::filesystem;
#  endif
#endif

//defines the path the server runs out of (temp fix)
#define RECIPE_PATH "/home/dugchugy/Documents/GitHub/RecipeServer"

using namespace HTTPServer;

namespace RecipeServer{

    //default RecipeServer Constructor, calls contsructor for 127.0.0.1:12345
    RecipeServer::RecipeServer(): RecipeServer("127.0.0.1", "12345"){}

    //recipes server construct just call the http server constructor with the pass port and IP
    RecipeServer::RecipeServer(std::string address, std::string port):HttpServer(address, port){

        /*
        PGDatabase = PQconnectdb("dbname=recipes host=127.0.0.1 user=dugchugy password=KJellbotn12!@");

        if (PQstatus(PGDatabase) == CONNECTION_BAD) {
            puts("We were unable to connect to the database");
            exit(0);
        }
        */
    }

    RecipeServer::~RecipeServer(){

        //closes the database connection
        //PQfinish(PGDatabase);

    }

    //handles generating the http respones from the request
    bool RecipeServer::handleResponse(const HTTPServer::HttpRequest &req, std::string &response){

        std::cout << "RecipeServer is handling request\n";

        //defines a varaible used to store the index of the first stapce in the request
        int spaceIndex = -1;

        //searchs the main header for the first space
        for(int i = 0; i < req.memLengths[1]; i++){
            //checks if this character is a space
            if(req.MainHeader[i] == ' '){
                //stores the index of the space
                spaceIndex = i;
                //exits the loop
                break;
            }
        }

        //checks if no space was found
        if(spaceIndex == -1){
            //responds that the request was poorly formatted and was not processed
            response = "HTTP/1.1 400 unnable to read response headers";
            std::cout << "unnable to find first space\n";
            return true;
        }

        int secondSpaceIndex = -1;

        //searchs the main header for the second space
        for(int i = spaceIndex + 1; i < req.memLengths[1]; i++){
            //checks if this character is a space
            if(req.MainHeader[i] == ' '){
                //stores the index of the space
                secondSpaceIndex = i;
                //exits the loop
                break;
            }
        }

        //checks if a second space was not found
        if(secondSpaceIndex == -1){
            //response that the request was poorly formatted and was not processed
            response = "HTTP/1.1 400 unnable to read response headers";
            std::cout << "unnalbe to find second space\n";
            return true;
        }

        //defiens a string used to store the request type
        std::string requestType = "";

        //reads through the main header again
        for(int i = 0; i < spaceIndex; i++){
            //adds each character to the request type
            requestType = requestType + req.MainHeader[i];
        }

        //creates a variable to store the request path
        std::string path = "";

        //reads through the main header to read the path
        for(int i = spaceIndex + 1; i < secondSpaceIndex; i++){
            //adds each character read to the path variable
            path = path + req.MainHeader[i];
        }

        //checks if the request type is GET
        if(requestType == "GET"){
            //runs the function to handle the get request and returns its return values
            return handleGetRequest(req, response, path);
        }

        if(requestType == "POST"){
            //runs the fun ction to handle the post request and returns its values
            return handlePostRequest(req, response, path);
        }


        //if the request type does not fit any of the defined request type options, a 405
        response = "HTTP/1.1 405 request type unknown";
        std::cout << "unknown request type\n";
        return true;

    }

    //a seperate function to ahndle specifically the http get requests
    bool RecipeServer::handleGetRequest(const HTTPServer::HttpRequest & req, std::string &response, std::string path){
        
        //creates a varaible to store the index of the last . in the path
        int lastDotIndex = -1;

        //searchs the string from back to front for a dot
        for(int i = path.size() - 1; i > -1; i--){
            if(path[i] == '.'){
                //stroes the found index and stops the loop (the last dot has been found)
                lastDotIndex = i;
                break;
            }
        }  

        //defines a varaible to store the content type (uses application/octet-stream if file extention is not recognized)
        std::string ContentType = "application/octet-stream";

        //checks if no dots were found (path specifes a folder)
        //in this case, ti redirects the path to point to the index.htm file in that folder
        if(lastDotIndex == -1){

            //checks if the last charcter in the path is not a /
            if(path[path.size() -1] != '/'){
                //adds a / to the end of the path
                path = path + "/";
            }

            //changes the path to \WebsiteFiles\<path in site>\index.htm
            path = "./WebsiteFiles" + std::regex_replace(path, std::regex("/"), "/") + "index.htm";
            //since the file extention is known, content type is also known
            ContentType = "text/html";
        }else{
            //creates a file extention var and sets it to and empty string
            std::string fileExtention = "";
            
            //loops through the end of the file specifed
            for(int i = lastDotIndex; i < path.size(); i++){
                //adds chars from the end of the path
                fileExtention = fileExtention + path[i];
            }

            if(fileExtention == ".htm" || fileExtention == ".html"){
                ContentType = "text/html";
            }
            if(fileExtention == ".js"){
                ContentType =  "text/javascript";
            }
            if(fileExtention == ".css"){
                ContentType = "text/css";
            }

            //changes the path to \WebsiteFiles\<path to resource specifed>
            path = "./WebsiteFiles" + std::regex_replace(path, std::regex("/"), "/");
        }

        //creates a varaible to store the contents of a specifed file
        std::string FileContents = "";

        //attempts to read the file requested
        try{
            FileContents = dumpFile(path);

        }catch(std::ios_base::failure){
            //failed to read file. file likely doesn't exist
            //returns a 404 response to the requester
            response = "HTTP/1.1 404 File not found";
            std::cout << "file not found\n";
            return true;
        }

        //checks if nothing was read from the file
        if(FileContents.size() < 1){
            //file failed to read, sends 404 response
            response = "HTTP/1.1 404 File not found";
            std::cout << "file not found\n";
            return true;
        }

        std::cout << "returned file requested\n";

        //generates the response
        response = "HTTP/1.1 200 OK\r\n";
        response = response + "Content-Type: " + ContentType + "\r\n";
        response = response + "Content-Length: " + std::to_string(FileContents.size()) + "\r\n\r\n";
        response = response + FileContents;

        //returns true
        return true;

    
    }

    bool RecipeServer::handlePostRequest(const HTTPServer::HttpRequest & req, std::string &response, std::string path){

        //creates a string to store the content of the request
        std::string strContent = "";

        //loops through the raw request content
        for(int i = 0; i < req.memLengths[3]; i++){

            //reads each character of the content
            strContent = strContent + req.Content[i];

        }

        //creates a regex to find the null character
        std::regex Nullfinder("'\0'");

        //checks if this is a search for a recipe
        if(path == "/RecipeSearch"){

            std::cout << "\n====================\nSearching for recipes\n====================\n\n";

            //creates an empty json to store the request content in
            rapidjson::Document RecievedJson;

            //reads the request content as a json
            RecievedJson.Parse(strContent.c_str());

            //rapidjson::Value& kwords = RecievedJson["keywords"];

            //should filter the results with keywords and only send results, since no new recipies can currently be added, sends all results

            std::string RecipePath = std::string(RECIPE_PATH) + "/Recipes";

            //DEFines the string for the json to be returned
            std::string ReturnJson = "{\"SearchResults\":[";

            //marks that the first item to be added to the list should be treated differntly
            bool firstAddition = true;

            std::cout << "starting loop";

            for(const auto& recipe : fs::directory_iterator(RecipePath)){

                //checks if this is the first addition to the json
                if(firstAddition){
                    //marks that the first addition has been added
                    firstAddition = false;
                }else{
                    //adds a comma between the pervious entry and this one
                    ReturnJson += ",";
                }

                 //attempts to read the file requested
                try{
                    //adds the found file to the json to be returned
                    ReturnJson += dumpFile(recipe.path());

                }catch(std::ios_base::failure){
                    //failed to read file. file likely doesn't exist
                    //returns a 404 response to the requester
                    response = "HTTP/1.1 404 File not found";
                    std::cout << "file not found\n";
                    return true;
                } 

            }

            //ends off the json
            ReturnJson += "]}";

            //filters any extra null chars out of the ReturnJson
            std::regex_replace(ReturnJson, Nullfinder, "");

            //sends the json as a response
            response = "HTTP/1.1 200 OK\r\n";
            response = response + "Content-Type: application/json\r\n";
            response = response + "Content-Length: " + std::to_string(ReturnJson.size()) + "\r\n\r\n";
            response = response + ReturnJson;

            std::cout << "raw response: " << response << "\n";

            std::cout << "char value of char 118 of ReturnJson: " << int(ReturnJson[118]) << "\n";

            return true;

        }

        if(path == "/RecipePost"){

            //creates an empty json to store the request content in
            rapidjson::Document RecievedJson;

            //reads the request content as a json
            RecievedJson.Parse(strContent.c_str());

            std::string name = RecievedJson["title"].GetString();

            //writes the file
            writeFile(RECIPE_PATH + ("/Recipes/" + name), strContent);
            //returns OK
            response = "HTTP/1.1 200 OK";

            return true;

        }

        if(path == "/databaseReq"){

            return handleSQLRequest(req, response, path, strContent);
        }

        //if post reuqest doesn't fit any of the specifed paths, return that the format is not supported
        response = "HTTP/1.1 400 POST request format not supported";
        return true;

    }

    bool RecipeServer::handleSQLRequest(const HTTPServer::HttpRequest & req, std::string &response, const std::string& path, const std::string& Content){

        //sets up the database connection
        PGconn* PGDatabase = PQconnectdb("dbname=recipes host=127.0.0.1 user=dugchugy password=KJellbotn12!@");

        //checks if the database conenction could be established
        if (PQstatus(PGDatabase) != CONNECTION_OK) {

            //returns that the databse connection could not be established
            response = "HTTP/1.1 500 Failed to connect to database";

            //prints out the connection status
            std::cout << "PG server response: " << PQstatus(PGDatabase) << std::endl;

            return true;
        }

        std::string command = Content.substr(0, Content.find(';'));

        std::cout << "executing: " << command.c_str() << std::endl;

        //forwards the requested query to the database
        PGresult *resp = PQexec(PGDatabase, command.c_str());

        std::cout << "PG response: " << PQresStatus(PQresultStatus(resp)) << std::endl;

        //checks if the response returned no dat but the commadn was processed successfully
        if(PQresultStatus(resp) == PGRES_COMMAND_OK){
            //responds that the request was successful but there is no data
            response = "HTTP/1.1 200 OK";
        
        //checks if the command returned data
        } else if(PQresultStatus(resp) == PGRES_TUPLES_OK) {

            //creates the json to use to return the response
            std::string jsonResponse = "{\"Results\": [";

            //reads how many rows and columns are in the response
            int rows = PQntuples(resp);
            int cols = PQnfields(resp);

            //creates a set of booleans to store the if the current row/column is the first
            bool firstRow = true;
            bool firstCol = true;

            //loops for each row
            for(int i = 0; i < rows; i++){

                //checks if this is the first row or if the , needs to be added
                if(firstRow){
                    jsonResponse += "[";
                }else{
                    jsonResponse += ", [";
                }

                //marks that the next column read will be the first column
                firstCol = true;

                //loops for each column
                for(int j = 0; j < cols; j++){

                    //checks if this is not the frist column
                    if(!firstCol){
                        //adds the , 
                        jsonResponse += ", ";
                    }

                    std::string valueStr = std::string(PQgetvalue(resp, i, j));

                    //checks if the value read is a json array or not
                    if(valueStr[0] == '['){

                        jsonResponse += valueStr;

                    }else{
                        //adds the value read from the database to the response
                        jsonResponse += "\"" + valueStr + "\"";
                    }

                    //marks that this is no longer the first column
                    firstCol = false;

                }

                jsonResponse += "]";

                //marks the this is no longer the first row
                firstRow = false;

            }

            //ends off the response json
            jsonResponse += "]}";

            //sends the json as a response
            response = "HTTP/1.1 200 OK\r\n";
            response = response + "Content-Type: application/json\r\n";
            response = response + "Content-Length: " + std::to_string(jsonResponse.size()) + "\r\n\r\n";
            response = response + jsonResponse;

            //comamnmd failed returns server error
        } else {

            response = "HTTP/1.1 500 DATABASE FAILED TO PROCESS QUERY";

            //prints the most recent error block from the connection
            std::cout << "error response: " << PQerrorMessage(PGDatabase) << std::endl;

        }

        //clears the data accosiated with the response object
        PQclear(resp);

        //sends the end comamnd
        resp = PQexec(PGDatabase, "END");
        PQclear(resp);

        //closes the database connection
        PQfinish(PGDatabase);

        return true;

    }

    bool writeFile(std::string path, std::string content){

        std::cout << "writing to the file " << path << "\n";

        //creates a file stream to do the writing
        std::ofstream fileStream;

        //opens the file and discards any content it may have had
        fileStream.open(path, std::ofstream::out | std::ofstream::trunc);

        //writes the content
        fileStream << content;

        //closes the file
        fileStream.close();

        return true;
    }

    std::string dumpFile(std::string path){

        std::string FileContents = "";

        std::cout << "reading from file " << path << "\n";

        //creates a file stream to attempt to read the specifed file
        std::ifstream fileStream;
        
        //attempts to open the specifed file
        fileStream.open(path);

        //creates a stream buffer to use
        std::stringstream FSStream;

        //reads fromt he file into the stream
        FSStream << fileStream.rdbuf();

        //copies the stream string into the fileContents string
        FileContents = FSStream.str();

        std::cout << "read " << FileContents.size() << " bytes from file\n";

        if (FileContents.size() == 0){
            std::cout << "file was empty. exiting";
            return "";
        }

        //closes the file
        fileStream.close();

        //checks if the last character is FileContents is a \0
        if(FileContents[FileContents.size() - 1] == '\0'){
            //removes the extra \0
            FileContents.replace(FileContents.size() - 1, 1, "");
        }

        //returns the file contents
        return FileContents;

    }

} // namepsace RecipeServer