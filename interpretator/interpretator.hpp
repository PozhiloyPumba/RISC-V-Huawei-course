#ifndef INTERPRETATOR_HPP
#define INTERPRETATOR_HPP

#include <iostream>

const int physMemorySize = 1024 * 1024;
using regType = int32_t;

template <typename T = unsigned>
T getBits (T x, int beg, int end) {         // count from end of data
    return (x << (sizeof(T) * 8 - end - 1)) >> (sizeof(T) * 8 - end - 1 + beg);
}

enum opcode {
    LUI_OPCODE         = 0,
    AUIPC_OPCODE       = 1,
    JAL_OPCODE         = 2,
    JALR_OPCODE        = 3,
    BRANCH_OPCODE      = 4,
    LOAD_OPCODE        = 5,
    STORE_OPCODE       = 6,
    OP_IMM_OPCODE      = 7,
    OP_OPCODE          = 8,
};


//=====================================================================================================

class physicalMemory {

private:
    char physicalMem_[physMemorySize] {};

public:
    int sizeCode = 0;

    void write (int placeInMemory, regType reg) {

        *(regType *)(physicalMem_ + placeInMemory) = reg;
    }

//-----------------------------------------------------------------------------------------------------

    void write (int placeInMemory, unsigned reg) {

        *((unsigned *)(physicalMem_ + placeInMemory)) = reg;
    }

//-----------------------------------------------------------------------------------------------------

    void read (size_t placeInMemory, regType &reg) const{

        reg = *(regType *)(physicalMem_ + placeInMemory);
    }

//-----------------------------------------------------------------------------------------------------

    unsigned readInstr (size_t placeInMemory) {

        return *((unsigned *)(physicalMem_ + placeInMemory));
    }
};

//=====================================================================================================

class State {

private:

public:
    regType regs[32] = {};
    
    physicalMemory memory;

    int pc = 0;
};

//=====================================================================================================

class Instruction {

private:

    size_t rd_, rs1_, rs2_;
    regType imm_ = 0;

    int getFormat (const unsigned x);
    void getInstr (const unsigned x);

    void RTypeFill(const unsigned x);
    void ITypeFill(const unsigned x);
    void STypeFill(const unsigned x);
    void BTypeFill(const unsigned x);
    void UTypeFill(const unsigned x);
    void JTypeFill(const unsigned x);

public:

    Instruction(const unsigned x){
        getInstr (x);
    }

    bool (Instruction::* executor_) (State &);

    bool luiFunc (State &state);
    bool auipcFunc (State &state);
    bool jalFunc (State &state);
    bool jalrFunc (State &state);

    bool beqFunc (State &state);
    bool bneFunc (State &state);
    bool bltFunc (State &state);
    bool bgeFunc (State &state);
    bool bltuFunc (State &state);
    bool bgeuFunc (State &state);

    bool lbFunc (State &state);
    bool lwFunc (State &state);

    bool sbFunc (State &state);
    bool swFunc (State &state);

    bool addiFunc (State &state);
    bool oriFunc (State &state);
    bool andiFunc (State &state);
    bool xoriFunc (State &state);

    bool addFunc (State &state);
    bool subFunc (State &state);
    bool orFunc (State &state);
    bool andFunc (State &state);
    bool xorFunc (State &state);
    bool sllFunc (State &state);
    bool srlFunc (State &state);
    //TODO: sra

};

#endif