#ifndef PROCESSOR_H_INCLUDED
#define PROCESSOR_H_INCLUDED

#include <string> //memset
#include <sstream>
#include <vector>

#include "ex.h"//separate object to include all the different functions
#include "datatype.h"//contains regi struct

class processor{
    public:
    int reg[32];//32 registers just like it should be :,)
    __int8 memory[1000];//technically this shouldn't be part of the processor, char is 8 bit, and int would be cheating
    std::vector <std::string> instr;//buffer to hold list of instructions

    std::string oFile;//file to write output to

    void memSet(int address, int data);//splits the data into 4 8-bit addresses
    int memPull(int address);//puts the data back together

    void IF();//instruction fetch
    void ID();//instruction decode
    void EX();//execute instruction/address calculation
    void MA();//memory access
    void WB();//write back

    int noCom();//returns 1 if there are no commands anywhere in the processor, used for ending execution
    //print register values for DEBUG
    void printIFID();
    void printIDEX();
    void printEXMA();
    void printMAWB();

    processor();

    void reset();

    private:
        //internal registers for processor
        regi IFID;
        regi IDEX;
        regi EXMA;
        regi MAWB;

        void printOut(std::string text);

        //control signals
        int pc;//for the instruction vector
        int instCnt;//current instruction value
        int cReg;//if 1, clear registers IFID/IDEX because of branch
        int IFSkip;//stalls everything while branch is being calculated, 1 for active
};

#endif // PROCESSOR_H_INCLUDED
