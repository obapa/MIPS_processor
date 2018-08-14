#include "processor.h" //for processor class
#include "IO.h" //header for ioFile struct
#include "ect.h"
#include <iostream>
#include <fstream>  //filestream
#include <sstream>  //stringstream
#include <string>   //strings
//TODO  branching issue
int main()
{
    std::string x;
    bool debug = true;

    processor MIPS;
    ioFile IO;

    IO = IO.handleIO();//creates ioFile object with all the file info
    MIPS = IO.loadInput(IO, MIPS);//loads data into memory and registers, returns from CODE

    int n = 1;//counts instruction cycle

    do{
        //std::cout << cycleNum(n++)<<std::endl;
        IO.print(cycleNum(n++));
        MIPS.WB();
        MIPS.MA();
        MIPS.EX();
        MIPS.ID();
        MIPS.IF();
        IO.print("\n");
    }while(!MIPS.noCom());//check that there are no more command loaded anywhere

    if(debug){
        IO.print("REGISTERS\n");
        for (int i = 0; i < 32; i++){
                if(MIPS.reg[i] != 0){//dont print 0s
                    IO.print("R"+iToS(i)+" "+iToS(MIPS.reg[i])+"\n");//print reg values
                }
        }
        IO.print("MEMORY\n");
        for (int i = 0; i < sizeof(MIPS.memory); i++){//its calling memset for somereason so giving error string
                if(MIPS.memory[i] != 0){//don't print 0s
                    IO.print(iToS(i-3)+" "+iToS((int)MIPS.memory[i])+"\n");
                    //std::cout<< "M" << i << ": " << (int)MIPS.memory[i] <<std::endl; //DEBUG print mem values
                }
        }
    }
    //for (int i = 0; i < MIPS.instr.size(); i++) {cout << MIPS.instr[i] <<endl;}

    return 0;
}
