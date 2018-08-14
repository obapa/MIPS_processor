#include "IO.h"
#include <iostream>
#include <vector>

ioFile::ioFile(){
    this->fileName = new std::string[2];//make a 2 string array for input and output file names
    this->iFile = new std::ifstream;
    //this->oFile = new std::ofstream;
    this->iss = new std::stringstream;
}

void ioFile::print(std::string text){//write to output file
    std::ofstream oFile;//the only part of the code I am unhappy about, I cant write to an output file if its referenced from a pointer
    oFile.open (this->fileName[1].c_str(), std::ios::app | std::ios::out);//I also can't pass an ofstream in a class/structure so I need to use a pointer
    oFile << text;//instead I am just opening the file every time I want to use it, writing to it, then closing it
    oFile.close();
};

//opens input and output files, loads input file into into IO.iss then closes it
ioFile ioFile::handleIO(){

    ioFile IO;
    IO.fileName = inOutPrompts(0);//get filenames for inout
    //IO.fileName[0] = "input.txt";
    //IO.fileName[1] = "out.txt";
    ////////////////////////

    //begin opening files
    IO.iFile->open (IO.fileName[0].c_str(), std::ifstream::in | std::ios::binary);

    if(loop != 1){//TODO get rid of this check
        //clear output file
        std::ofstream oFile;
        oFile.open (this->fileName[1].c_str(), std::ios::out | std::ios::binary);
        oFile.close();
    }

    /*
    while (!IO.iFile->is_open()){//if input file isn't found, loop until valid is entered
        std::cout <<"\"" << IO.fileName[0] << "\" not found" <<std::endl;
        IO.fileName = inOutPrompts(1);
        IO.iFile->open (IO.fileName[0].c_str(), std::ios::in | std::ios::binary);
    }*/
    std::cout << "\"" << IO.fileName[0] << "\" opened" << std::endl;

    //copy input file data to stringstream for easier use
    *(IO.iss) << IO.iFile->rdbuf();//move input file to stringstream
    IO.iFile->close();//can close now that its been moved to string stream

    return IO;
}

//processes information from input file, preloads memory and registers
processor ioFile::loadInput(ioFile IO, processor proc){
    std::string a;//used to check for text from input, ie. REGISTERS
    std::string b;//to hold data from ss
    int pos;//position for saving to array in processor
    int data;//data for writing in memory

    proc.oFile = IO.fileName[1];//copy the output file name to the processor

    //start getting input info
    *IO.iss >> a;//load first value from input file
    while(IO.iss->rdbuf()->in_avail()){//run while there is data in ss
        if(a.compare("REGISTERS") == 0){
            *IO.iss >> b;//register name
            if(b.compare("MEMORY") == 0){//if we have reached the end of registers
                a = b;//copy value of b into a;
            }else{//if there are still registers
                std::stringstream (b.substr(1)) >> pos;//remove "R" from the b value
                *IO.iss >> proc.reg[pos];
            }
        }else if(a.compare("MEMORY") == 0){
            *IO.iss >> b;//address for memory
            if(b.compare("CODE") == 0){//if we have reached the end of the memory
                a = b;
            }else{
                std::stringstream (b) >> pos;//position for memory
                *IO.iss >> b;//data
                std::stringstream (b) >> data;//data for memory
                proc.memSet(pos,data);//pos to memSet to write to memory data is set in 8 bit spaces
            }
        }
        else if(a.compare("CODE") == 0){//copy CODE into instr vector
            std::string s;
            *IO.iss >> s;
            proc.instr.push_back(s);// << IO.iss->rdbuf();
        }
        else{
            std::cout << "invalid data in \"" << IO.fileName[0] <<"\"" << std::endl;
            *IO.iss>>a;
        }
    }
    return proc;//return the loaded processor
}

//handles getting file names,
//choice = 0 for both, choice = 1 for only input file name
std::string* ioFile::inOutPrompts(int choice){
    std::string* inOut = new std::string[2];//TODO probable clears output name if choice == 0
    std::string temp;
    if (loop == 1){
        getline(std::cin, temp);
    }
    std::cout << "Name of input file and output file separated by a space: \n";//input file name
    getline(std::cin, temp);
    std::stringstream ss(temp);
    ss >> inOut[0];
    ss >> inOut[1];
    /*if (choice == 0){//run if choice is 0, can skip if only want input filename
        std::cout << "Name of output file: ";//output file name
        getline (std::cin, inOut[1]);
    }*/

    return inOut;
}

void ioFile::reset(){
    ioFile();
    loop = 1;
}
