#include "processor.h"
#include "ect.h"//for binary conversion
#include <iostream>
#include <fstream>
#include <string.h>

using namespace std;

processor::processor(){
        memset(reg, 0 ,sizeof(reg));//initialize all registers to 0
        memset(memory, 0, sizeof(memory));
        this->instr.clear();//clear vector
        this->pc = 0;//set pc to 0
        this->cReg = 0;//don't clear registers IFID/IDEX
        this->instCnt = 0;//instruction count to be given to new instructions in IF
        this->IFSkip = 0;//says if branch is running, skip IF
        //clear the registers initially
        this->IFID.reset();
        this->IDEX.reset();
        this->EXMA.reset();
        this->MAWB.reset();
    }

//verifies that address is a factor of 4, splits data through 4 addresses if it is big enough
//I could of just made memory size 1000/4 and rejected wrong addresses but thats not in the spirit
//this will actually act as if the data if split into 4 different registers
void processor::memSet(int address, int data){
    if(address%4 != 0 || address >= sizeof(this->memory) || address < 0){//do not write data if invalid address given
        cout << "invalid address given: " << address <<""<< endl;
        if(address%4 != 0){
            cout << "address must be a multiple of 4" << endl;
        }else if ( address >= sizeof(this->memory) || address < 0){
            cout << "address range is [0:" << sizeof(this->memory)-1 << "]" << endl;
        }
        cout << "data \"" << data << "\" skipped" <<endl;
    }else{//set data bits to right places
        this->memory[address] = ((data >> 24) & 255);
        this->memory[address+1] = ((data >> 16) & 255);
        this->memory[address+2] = ((data >> 8) & 255);
        this->memory[address+3] = (data & 255);
    }
}

int processor::memPull(int address){
    //all the sweet sweet data, puts it back together
    return (this->memory[address] << 24) + (this->memory[address+1] << 16) + (this->memory[address+2] << 8) + (this->memory[address+3]);
}

void processor::IF(){
    if((this->IFID.stall == 0 || this->IFID.data.empty()) && this->IFSkip == 0){//only run if IFID is not stalled, or if the IFID data string is empty(initial), and not skipping
        this->IFID.reset();//reset IFID to blank
        this->IFID.skip = 0;//don't skip the step
        if(this->pc < instr.size()-1){//only get an instruction if there are instructions left, but don't end because branch could change pc after last fetch
            this->IFID.data = this->instr[this->pc];//pull command and save in buffer
            this->IFID.noCom = 0;//there are commands left to execute
            this->IFID.instCnt = ++this->instCnt;//increment the instruction number and save to IFID reg
            this->IFID.pc = ++this->pc;//increment the pc and save it as well as send it to IFID
            printOut("I"+iToS(this->IFID.instCnt)+"-IF ");
        }else{
            this->IFID.noCom = 1;//there are no more commands to load, wait till this goes through to WB make sure no final branch
            this->IFID.skip = 1;
        }
        this->cReg = 0;//once the counter hits IF IFID/IDEX have been cleared
    }
    //cout <<"IF"<<endl;
    //this->IFID.printR();//print to console DEBUG
}

void processor::ID(){
    if(!this->IFID.data.empty()&& this->IFID.skip == 0){//skip the initial run through before everything has been loaded in
        int op = bToD("0" + this->IFID.data.substr(0,6));//add 0 so it doesnt get converter into twos complement accidently
        this->IFID.stall = 0;
        //--------begin stall check----------//
        if(!this->EXMA.data.empty()){//only run if there is data in EXMA
            if(this->EXMA.type.compare("R") == 0){//if com in EXMA is R type, compare to its rD
                if(this->IFID.data.substr(6,5).compare(this->EXMA.data.substr(16,5)) == 0 || //check cur.rS!=next.rD && (cur.rT!=next.rD if this is R-type)
                   ((op == 0 || op == 43) && this->IFID.data.substr(11,5).compare(this->EXMA.data.substr(16,5)) == 0)){//if current is R-type/SW because it reads dest reg
                        this->IFID.stall = 1;//if they are the same location then need to stall
                }
            }else {// otherwise com in EXMA is I-type, compare to rT
                if((this->IFID.data.substr(6,5).compare(this->EXMA.data.substr(11,5)) == 0 || //check cur.rS!=next.rT && (cur.rT!=next.rT if this is R-type)
                   ((op == 0 || op == 43) && this->IFID.data.substr(11,5).compare(this->EXMA.data.substr(11,5)) == 0))){//if current is R-type/SW uses what everyone else sees as the destination reg
                        this->IFID.stall = 1;//if they are the same location then need to stall
                }
            }
        }if(!this->MAWB.data.empty()){//only run if there is data in MAWB
            if(this->MAWB.type.compare("R") == 0){//if com in MAWB is R type, compare to its rD
                if(this->IFID.data.substr(6,5).compare(this->MAWB.data.substr(16,5)) == 0 || //check cur.rS!=next.rD && (cur.rT!=next.rD if this is R-type)
                   ((op == 0 || op == 43) && this->IFID.data.substr(11,5).compare(this->MAWB.data.substr(16,5)) == 0)){//if current type is R-type/SW
                        this->IFID.stall = 1;//if they are the same location then need to stall
                }
            }else {// otherwise com in MAWB is I-type, compare to rT
                if((this->IFID.data.substr(6,5).compare(this->MAWB.data.substr(11,5)) == 0 || //check cur.rS!=next.rT && (cur.rT!=next.rT if this is R-type)
                   ((op == 0 || op == 43) && this->IFID.data.substr(11,5).compare(this->MAWB.data.substr(11,5)) == 0))//if current type is R-type/SW
                   && (!this->MAWB.type.substr(0,1).compare("b")==0) ){//don't worry about collisions if it there is a branch in the future
                        this->IFID.stall = 1;//if they are the same location then need to stall
                }
            }
        }
        //---------end stall check---------------//
        if(this->IFID.stall == 0){//if not stalled
            this->IDEX = this->IFID;//copy over all values and then start overwriting
            this->IDEX.op = op;//copy op code value
            if(this->IDEX.op == 0){//R-type
                this->IDEX.type = "R";//set the registers to the proper values
                this->IDEX.rS = this->reg[bToD(this->IFID.data.substr(6,5))];//load value from register
                this->IDEX.rT = this->reg[bToD(this->IFID.data.substr(11,5))];//load value from register
                //this->IDEX.rD = bToD(this->IFID.data.substr(16,5));//TODO only need to take the address not the value////////don't think did anything
                this->IDEX.funct = bToD(this->IFID.data.substr(21,11));//set function value
            }else{//I-type
                this->IDEX.type = "I";//set the registers to the proper values
                this->IDEX.rS = this->reg[bToD(this->IFID.data.substr(6,5))];//load value from register
                this->IDEX.rT = this->reg[bToD(this->IFID.data.substr(11,5))];//TODO only need address not value//need for sw
                this->IDEX.offset = bToD(this->IFID.data.substr(16,16));//load offset
            }
            this->IFID.reset();//clear reg data came from
            printOut("I"+iToS(this->IDEX.instCnt)+"-ID ");
        //cout<<"ID"<<endl;
        //this->IDEX.printR();//DEBUG remove
        }else{//if we had stalled
            this->IDEX.reset();//clear IDEX register for the next cycle
            this->IDEX.skip = 1;//we stalled so skip next stages
            printOut("I"+iToS(this->IFID.instCnt)+"-stall");//print stall
        }
    }else if(this->IFID.noCom == 1){//pass the no command even if the stage is skipped
        this->IDEX.noCom = 1;
    }
}

void processor::EX(){//made a separate object to split up all the commands
    if(this->IDEX.skip == 0){
        ex E(this->IDEX);
        E.run();//get resultant EXMA register values
        this->EXMA = E.pullEXMA();
        this->IDEX.reset();//clear the register data came from
        //cout <<"EX"<<endl;
        //this->EXMA.printR();//DEBUG remove
        if(this->EXMA.type.compare("bY") == 0 || this->EXMA.type.compare("bN") == 0){//check if we are doing a branch
            this->IFSkip = 1;//skip these stages until branch is finished
            this->IFID.skip = 1;
            this->IDEX.skip = 1;
        }
        printOut("I"+iToS(this->EXMA.instCnt)+"-EX ");
    }else if(this->IDEX.noCom == 1){//pass the no command even if the stage is skipped
        this->EXMA.noCom = 1;
    }

}

void processor::MA(){
    if(this->EXMA.skip == 0){
        this->MAWB = this->EXMA;//copy contents of register and then start overwriting
        if(this->EXMA.type.compare("bY") == 0){//branch yes command
            this->pc = this->EXMA.pc;//set processor pc to the pc saved in EXMA reg
            //this->MAWB.skip = 1;//no value is being written into MAWB so we don't want it to write anything, pipeline so run anyways, but does nothing
            //this->cReg = 1;//clear contents from IFID/IDEX registers for branch, apparently not doing any forwarding so there is nothing there
        }else if(this->EXMA.type.compare("bN") == 0){//branch no command
            this->IFID.skip = 0;//continue on from the ID since we didn't branch
            this->IFID.stall = 0;
            //this->MAWB.skip = 1;//skip last stage but do nothing else, jk its pipelined so don't skip
        }else if(this->EXMA.type.compare("lw") == 0){//load word
            this->MAWB.rT = this->memPull(this->EXMA.address);//load mem value into rT
            this->MAWB.skip = 0;//don't skip
            this->MAWB.type = "I";//remind it who you are
        }else if(this->EXMA.type.compare("sw") == 0){//store word
            this->memSet(this->EXMA.address,this->EXMA.rT);//save rT to memory address
            //this->MAWB.skip = 1;//skip register write, not for pipelined
        }else if(this->EXMA.type.compare("I") == 0){//I type (only sub)
            this->MAWB.rT = this->EXMA.rT;//WB will save value of rT if type is I
            this->MAWB.type = this->EXMA.type;//save the I-type
            this->MAWB.skip = 0;//don't skip WB
        }else {//for all r-types
            this->MAWB.rD = this->EXMA.rD;//WB will save value of rD if r-type
            this->MAWB.type = this->EXMA.type;//save the R-type
            this->MAWB.skip = 0;//don't skip WB
        }
    printOut("I"+iToS(this->EXMA.instCnt)+"-MEM ");
    if (this->EXMA.type.compare("bY") == 0 || this->EXMA.type.compare("bN") == 0){//check if branch has completed
        this->IFSkip = 0;//branch has finished so stop skipping IF
    }
    this->EXMA.reset();//clear reg data came from
    //cout<<"MA"<<endl;
    //this->MAWB.printR();//DEBUG
    }else if(this->EXMA.noCom == 1){//pass the no command even if the stage is skipped
        this->MAWB.noCom = 1;
    }

}

void processor::WB(){
    if(this->MAWB.skip == 0){//run if the skip is not set
        if(this->MAWB.type.compare("I") == 0){//if I-type save rT
            this->reg[bToD(this->MAWB.data.substr(11,5))] = this->MAWB.rT;//save rT into register given initially
        }else if (this->MAWB.type.compare("R") == 0){//if R-type save rD
            this->reg[bToD(this->MAWB.data.substr(16,5))] = this->MAWB.rD;//save rD into register given initially
        }
        if (reg[0] != 0) reg[0] = 0;//make sure register 0 stays 0
        printOut("I"+iToS(this->MAWB.instCnt)+"-WB ");//correct value is still saved because newest MA hasn't been ran
        this->MAWB.reset();//reset the register data came from
    }
}

int processor::noCom(){
    if (this->IFID.noCom == 1 && this->EXMA.noCom == 1 && this->MAWB.noCom == 1){//if there are no commands in the initial or final registers
        return 1;//there are no commands left to be ran
    }else {
        return 0;//otherwise there are still commands to be ran
    }
}

void processor::printOut(std::string text){
    std::ofstream oFile;//the only part of the code I am unhappy about, I cant write to an output file if its referenced from a pointer
    oFile.open (this->oFile.c_str(), std::ios::app | std::ios::out);//I also can't pass an ofstream in a class/structure so I need to use a pointer
    oFile << text;//instead I am just opening the file every time I want to use it, writing to it, then closing it
    oFile.close();
}

//prints register contents to the console, used for debugging
void processor::printIFID(){//change to write to output file
    cout << this->IFID.data;
}

void processor::printIDEX(){
    if (IDEX.type.compare("I") == 0){
        cout << IDEX.funct << " " << IDEX.rD << " " << IDEX.rS << " " <<IDEX.rT << endl;
    }else if(IDEX.type.compare("R") == 0){
        cout << IDEX.op << " " << IDEX.rS << " " << IDEX.rT << " " << IDEX.offset << endl;
    }
}

void processor::printEXMA(){
    cout << "$d: " << this->EXMA.rD << endl;
}

void processor::reset(){
    processor();
}
