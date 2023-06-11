#include "DugsServer.hpp"
#include <iostream>
#include <unistd.h>

int main(){
    //creates an instance of the recipe server object
    RecipeServer::RecipeServer recServer("raspberrypi", "80");

    recServer.start();

    std::cout << "server started\n";

    int stopInt = -1;

    while(true){

        sleep(1);

        std::cout << "enter (1) to stop the server: ";
        std::cin >> stopInt;

        if(stopInt == 1){
            break;
        }

    }

    std::cout << "server stopped\n";

    recServer.stop();

    return(0);
}
