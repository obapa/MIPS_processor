#include "ex.h"

ex::ex(regi val){//constructor, copy value into IDEX/EXMA registers
    //load both registers with given values, EXMA will be updated from run()
    this->loadIDEX(val);
    this->loadEXMA(val);
}

void ex::loadIDEX(regi val){
    this->IDEX = val;
}

void ex::loadEXMA(regi val){
    this->EXMA = val;
}

regi ex::pullEXMA(){
    return this->EXMA;
}

void ex::run(){
    if(this->IDEX.type.compare("R") == 0){//R-type
        switch(this->IDEX.funct){
            case ADD: this->add(); break;
            case SUB: this->sub(); break;
            case SLT: this->slt(); break;
            default:;
        }
    }else {
        switch(this->IDEX.op){//I-type
            case LW: this->lw(); break;
            case SW: this->sw(); break;
            case ADdI: this->addi(); break;
            case BEQ: this->beq(); break;
            case BNE: this->bne(); break;
            default: break;
        }
    }
}

void ex::add(){//d=s+t
    this->EXMA.rD = this->IDEX.rS + this->IDEX.rT;
}

void ex::sub(){//d=s-t
    this->EXMA.rD = this->IDEX.rS - this->IDEX.rT;
}

void ex::slt(){//d=1 if s<t
    this->EXMA.rD = (this->IDEX.rS < this->IDEX.rT) ? 1 : 0;
}

void ex::addi(){//t = s + offset
    this->EXMA.rT = this->IDEX.rS + this->IDEX.offset;
}

void ex::lw(){//address = 4*offset + s
    this->EXMA.address = ((this->IDEX.rS) + (this->IDEX.offset<<2));
    this->EXMA.type = "lw";
}

void ex::sw(){//address = 4*offset + s
    this->EXMA.address = ((this->IDEX.rS) + (this->IDEX.offset<<2));
    this->EXMA.type = "sw";
}

void ex::beq(){//if s==t pc=offset + pc
    if (this->IDEX.rS == this->IDEX.rT){
        this->EXMA.pc = (this->IDEX.pc + (this->IDEX.offset));
        this->EXMA.type = "bY";//branch yes
    }else{
        this->EXMA.type = "bN";
    }
}

void ex::bne(){//if s!=t pc=offset + pc
    if (this->IDEX.rS != this->IDEX.rT){
        this->EXMA.pc = (this->IDEX.pc + (this->IDEX.offset));
        this->EXMA.type = "bY";//branch yes
    }else {
        this->EXMA.type = "bN";
    }
}
