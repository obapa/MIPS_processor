#ifndef EX_H_INCLUDED
#define EX_H_INCLUDED

#include "datatype.h"

//begin macro defines for function codes
#define ADD 32//add
#define SUB 34//sub
#define SLT 42//slt
//begin macros or opcodes
#define ADdI 8//add immediate
#define LW 35//load word
#define SW 43//store word
#define BEQ 4//branch if equal
#define BNE 5//branch if not equal

class ex{
    public:
        void loadIDEX(regi val);//load values into EXMA register
        regi pullEXMA();
        void run();//have it perform EX stage

        ex(){};
        ex(regi val);

    private:
        regi IDEX;
        regi EXMA;

        void loadEXMA(regi val);
        void add();
        void sub();
        void slt();
        void addi();
        void lw();
        void sw();
        void beq();
        void bne();
};

#endif // EX_H_INCLUDED

