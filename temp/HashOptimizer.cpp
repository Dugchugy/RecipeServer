#include<iostream>
#include<fstream>
#include<math.h>
#include<string>
#include<cstdint>

uint64_t* powsOf10 = nullptr;

uint64_t ui64Pow10(int b){
    if(powsOf10 == nullptr){
        powsOf10 = new uint64_t[18];

        uint16_t t = 1;
        uint64_t ten = 10;

        for(int i = 0; i < 18; i++){
            powsOf10[i] = t;

            std::cout << "10^" << i << " = " << t << std::endl;

            t = (uint64_t) t * (uint64_t) ten;
        }
    }

    return powsOf10[b];
}

unsigned int TrunkHash(uint64_t input, const int* digits){
    uint64_t midval = 0;

    for(int i = 0; i < 16; i++){
        midval += ((input/ui64Pow10(digits[i]))%10) * ui64Pow10(i);
    }

    unsigned int key = midval % 103;

    return key;
}

int rankOutput(int* results){
    uint64_t sum = 0;

    for(int i = 0; i < 103; i++){
        sum += abs(results[i] -1);
    }

    return sum;
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

    int bestSum = 1000;

    //sets the ints to 20 (reads as ignored value in hash function)
    for(int i = 0; i < 16; i++){
        bestDigs[i] = 17;
        curDigits[i] = 17;
    }

    int* results;

    std::cout << "starting loop" << std::endl;

    //loops for all combinations of digits
    for(curDigits[0] = 0; curDigits[0] < 17; curDigits[0] ++){

        std::cout << std::endl << "outerCycle " << curDigits[0] << std::endl;
        
        std::cout << "best Sum: " << bestSum << std::endl;

        std::cout << "using digits ";

        for(int i =0; i <16; i++){
            std::cout<< bestDigs[i] << " ";
        }

        std::cout << std::endl;

        for(curDigits[1] = 0; curDigits[1] < 17; curDigits[1] ++){
            for(curDigits[2] = 0; curDigits[2] < 17; curDigits[2] ++){
                for(curDigits[3] = 0; curDigits[3] < 17; curDigits[3] ++){
                    for(curDigits[4] = 0; curDigits[4] < 17; curDigits[4] ++){
                        /*for(curDigits[5] = 0; curDigits[5] < 17; curDigits[5] ++){
                            /*for(curDigits[6] = 0; curDigits[6] < 17; curDigits[6] ++){
                                for(curDigits[7] = 0; curDigits[7] < 17; curDigits[7] ++){
                                for(curDigits[8] = 0; curDigits[8] < 17; curDigits[8] ++){
                                    for(curDigits[9] = 0; curDigits[9] < 17; curDigits[9] ++){
                                        for(curDigits[10] = 0; curDigits[10] < 17; curDigits[10] ++){
                                            for(curDigits[11] = 0; curDigits[11] < 17; curDigits[11] ++){
                                                for(curDigits[12] = 0; curDigits[12] < 17; curDigits[12] ++){
                                                    for(curDigits[13] = 0; curDigits[13] < 17; curDigits[13] ++){
                                                        for(curDigits[14] = 0; curDigits[14] < 17; curDigits[14] ++){
                                                            for(curDigits[15] = 0; curDigits[15] < 17; curDigits[15] ++){*/

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
                                                                int sum = rankOutput(results);

                                                                //std::cout << "found average: " << avg << std::endl;

                                                                if(sum < bestSum){
                                                                    //saves the current best values
                                                                    bestSum = sum;
                                                                    for(int i = 0; i < 16; i++){
                                                                        bestDigs[i] = curDigits[i];
                                                                    }
                                                                }

                                                                delete[] results;

                                                                results = nullptr;
                                                           /*}
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                                }
                            }*/
                        //}
                    }
                }



            }
        }

    }

    std::cout << "best Sum: " << bestSum << std::endl;

    std::cout << "using digits ";

    for(int i =0; i <16; i++){
        std::cout<< bestDigs[i] << " ";
    }

    std::cout << std::endl;


    delete[] bestDigs;
    delete[] curDigits;
    delete[] input;

    if(powsOf10 != nullptr){
        delete[] powsOf10;
    }



    

    return 1;
}