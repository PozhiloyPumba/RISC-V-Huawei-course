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

    unsigned readInstr (size_t placeInMemory) const{

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

    uint8_t rd_, rs1_, rs2_;
    bool bad_ = false;
    regType imm_ = 0;

    int getFormat (const unsigned x);
    bool getInstr (const unsigned x);

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

    bool getBad () const{
        return bad_;
    }

    bool (Instruction::* executor_) (State &) const;

    bool luiFunc (State &state) const;
    bool auipcFunc (State &state) const;
    bool jalFunc (State &state) const;
    bool jalrFunc (State &state) const;

    bool beqFunc (State &state) const;
    bool bneFunc (State &state) const;
    bool bltFunc (State &state) const;
    bool bgeFunc (State &state) const;
    bool bltuFunc (State &state) const;
    bool bgeuFunc (State &state) const;

    bool lbFunc (State &state) const;
    bool lwFunc (State &state) const;

    bool sbFunc (State &state) const;
    bool swFunc (State &state) const;

    bool addiFunc (State &state) const;
    bool oriFunc (State &state) const;
    bool andiFunc (State &state) const;
    bool xoriFunc (State &state) const;

    bool addFunc (State &state) const;
    bool subFunc (State &state) const;
    bool orFunc (State &state) const;
    bool andFunc (State &state) const;
    bool xorFunc (State &state) const;
    bool sllFunc (State &state) const;
    bool srlFunc (State &state) const;

};

#endif