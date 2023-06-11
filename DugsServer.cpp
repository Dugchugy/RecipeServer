#include "DugsServer.hpp"
#include <string>
#include <regex>
#include <fstream>
#include <sstream>
#include <iostream>

using namespace HTTPServer;

namespace RecipeServer{

    //default RecipeServer Constructor, calls contsructor for 127.0.0.1:12345
    RecipeServer::RecipeServer(): RecipeServer("127.0.0.1", "12345"){}

    //recipes server construct just call the http server constructor with the pass port and IP
    RecipeServer::RecipeServer(std::string address, std::string port):HttpServer(address, port){}

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
            path = ".\\WebsiteFiles" + std::regex_replace(path, std::regex("/"), "\\") + "index.htm";
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
            path = ".\\WebsiteFiles" + std::regex_replace(path, std::regex("/"), "\\");
        }

        //creates a varaible to store the contents of a specifed file
        std::string FileContents = "";

        //attempts to read the file requested
        try{
            std::cout << "reading from file " << path;

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

            //closes the file
            fileStream.close();

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

} // namepsace RecipeServer