#include "ect.h"

#include <sstream>

//parse binary(string) to decimal(int)
int bToD(std::string bin){
    int x = 0;
    int temp;
    int first = 1;
    int neg = 0;

    std::stringstream(bin.substr(0,1)) >> temp;//load first bit to check for twos comp
    if(first){//check the first bit, for twos complement
        first = 0;
        if(temp == 1){
            neg = 1;
        }
    }
    for (int i = 0; i < bin.length()-1; i++){
        std::stringstream(bin.substr(bin.length()-i-1,1)) >> temp;//load raw binary into int
        if (!(temp == 0 || temp == 1)){
            temp = 0;//pretty big security flaw if not scrubbing inputs to only accept 1/0,
            //for instance the val "10030" would be converted to 22, can let people access different memory locations
        }
        (neg) ? (x -= ((temp - 1)*(temp - 1) << i)) : (x += (temp << i));//if neg do (val - 1)^2 to get the xor value
    }if(neg){
        x--;//make x negative and add one two make twos comp
    }


    return x;
}

std::string iToS(int n){
    std::stringstream ss;
    ss << n;
    return ss.str();
}

std::string cycleNum(int n){//returns string "c#n "
    return "c#"+iToS(n)+" ";
}
