#include "datatype.h"
#include <iostream>

void regi::reset(){
    this->data = "";
    this->type = "";
    this->rD = 0;//$d
    this->rS = 0;//$s
    this->rT = 0;//$t
    this->op = 0;//opcode
    this->funct = 0;//function
    this->offset = 0;//offset
    this->address = 0;//save address of memory for lw/sw
    this->pc = 0;//program counter
    this->skip = 1;//skip initially
    this->noCom = 0;//there are initially commands to load
    this->instCnt = 0;//instruction count for output
    this->stall = 0;//tells processor if it should stall step
}

void regi::printR(){
    std::cout << "data: " << data << std::endl;
    std::cout << "type: " << type << std::endl;
    std::cout << "$D: " << rD<< std::endl;//$d
    std::cout << "$S: " << rS<< std::endl;//$s
    std::cout << "$T: " << rT << std::endl;//$t
    std::cout << "op: " << op << std::endl;//opcode
    std::cout << "funct: " << funct << std::endl;//function
    std::cout << "offset: " << offset << std::endl;//offset
    std::cout << "address: " << address << std::endl;//save address of memory for lw/sw
    std::cout << "pc: " << pc << std::endl;//program counter
    std::cout << "skip: " << skip << std::endl <<std::endl;//1 if write should be skipped
}
