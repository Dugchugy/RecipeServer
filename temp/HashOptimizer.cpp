#include<iostream>
#include<fstream>
#include<math.h>
#include<string>
#include<cstdint>

unsigned int TrunkHash(uint64_t input, const int* digits){
    uint64_t midval = 0;

    for(int i = 0; i < 16; i++){
        midval += ((input/(int)(pow(10, digits[i])))%10) * pow(10, i);
    }

    unsigned int key = midval % 103;

    return key;
}

float rankOutput(int* results){
    int sum = 0;

    for(int i = 0; i < 103; i++){
        sum += abs(results[i] -1);
    }

    return (sum/103.0f);
}

uint64_t * readFile(){
    std::ifstream myfile("dataFile_100_16.txt");

    if(myfile.is_open()){
        uint64_t* inputs = new uint64_t[100];

        std::string line = "";
        int i = 0;

        while(getline(myfile, line)){
            inputs[i] = stoul(line);
            i++;

        }

        myfile.close();

        return inputs;
    }else{
        return nullptr;
    }
}

int main(){

    uint64_t* input;

    input = readFile();
    std::cout << "read the file" << std::endl;

    if(input == nullptr){
        std::cout << "failed to read the file" << std::endl;
        return -1;
    }

    //declares 2 arrays for stores the digit combos
    int* bestDigs = new int[16];

    int* curDigits = new int[16];

    float bestAvg = 1;

    //sets the ints to 20 (reads as ignored value in hash function)
    for(int i = 0; i < 16; i++){
        bestDigs[i] = 20;
        curDigits[i] = 20;
    }

    int* results;

    std::cout << "starting loop" << std::endl;

    //loops for all combinations of digits
    for(curDigits[0] = 0; curDigits[0] < 17; curDigits[0] ++){
        for(curDigits[1] = 0; curDigits[1] < 17; curDigits[1] ++){
            for(curDigits[2] = 0; curDigits[2] < 17; curDigits[2] ++){

                //sets results to an array of 103 0s
                results = new int[103];

                //sets results to 0
                for(int i =0; i < 103; i++){
                    results[i] = 0;
                }

                for(int i = 0; i < 100; i++){
                    //finds the key for the current input
                    unsigned int key = TrunkHash(input[i], curDigits);
                    //adds it to the results
                    results[key] += 1;
                }

                //ranks the result
                float avg = rankOutput(results);

                std::cout << "found average: " << avg << std::endl;

                if(avg < bestAvg){
                    //saves the current best values
                    bestAvg = avg;
                    for(int i = 0; i < 16; i++){
                        bestDigs[i] = curDigits[i];
                    }
                }

                delete[] results;

                results = nullptr;



            }
        }

    }

    std::cout << "best Average: " << bestAvg << std::endl;

    std::cout << "using digits ";

    for(int i =0; i <16; i++){
        std::cout<< bestDigs[i] << " ";
    }

    std::cout << std::endl;


    delete[] bestDigs;
    delete[] curDigits;
    delete[] input;



    

    return 1;
}