#ifndef IO_H_INCLUDED
#define IO_H_INCLUDED

#include <fstream>  //filestream
#include <sstream>  //stringstream
#include <string.h> //string

#include "processor.h"

//structure for ioFile type
struct ioFile{
    std::ifstream* iFile;
    std::string* fileName;
    std::stringstream* iss;//input string stream

    processor loadInput(ioFile IO, processor proc);//loads initial values to registers and memory
    ioFile handleIO(); //opens input/output file
    std::string* inOutPrompts(int choice);//handles getting file names

    ioFile();
    void print(std::string text);
    void reset();

    private:
        int loop = 0;//go to 1 if the second time running through, dont clear output file

};


#endif // IO_H_INCLUDED
