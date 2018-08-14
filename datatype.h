#ifndef DATATYPE_H_INCLUDED
#define DATATYPE_H_INCLUDED

#include <string>

struct regi {
    //include control signals?
    std::string data;
    std::string type;
    int rD;//$d
    int rS;//$s
    int rT;//$t
    int op;//opcode
    int funct;//function
    int offset;//offset
    int address;//save address of memory for lw/sw

    //control signals
    int pc;//program counter
    int instCnt;//count of instruction number
    int skip;//1 if write should be skipped
    int noCom;//1 if no more commands to execute
    int stall;//1 if should stall

    //functions
    void reset();//reset the register to initial values
    void printR();//print contents of REG for debug

};

#endif // DATATYPE_H_INCLUDED
