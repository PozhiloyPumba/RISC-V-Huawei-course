#include "interpretator.hpp"

#define DUMP

//=====================================================================================================

static void binDump(const unsigned x) {
    // printf("hi\n");
    for (int i = 31; i >= 0; --i) {
        printf("%d", (x >> i) & 1);
        // printf("%d = %d\n", i, (x >> i) & 1);
    }
    
    printf("\n");
}

//=====================================================================================================

void Instruction::RTypeFill(const unsigned x) {
    rd_ = getBits (x, 7, 11);
    rs1_ = getBits (x, 15, 19);
    rs2_ = getBits (x, 20, 24);
}

//-----------------------------------------------------------------------------------------------------

void Instruction::ITypeFill(const unsigned x) {
    rd_ = getBits (x, 7, 11);
    rs1_ = getBits (x, 15, 19);

    for (int i = 12; i < 32; ++i)
        imm_ |= getBits (x, 31, 31) << i;
    imm_ |= getBits (x, 20, 31);
}

//-----------------------------------------------------------------------------------------------------

void Instruction::STypeFill(const unsigned x) {
    rs1_ = getBits (x, 15, 19);
    rs2_ = getBits (x, 20, 24);

    for (int i = 12; i < 32; ++i)
        imm_ |= getBits (x, 31, 31) << i;
    imm_ |= getBits (x, 7, 11) | getBits (x, 25, 31) << 5;
}

//-----------------------------------------------------------------------------------------------------

void Instruction::BTypeFill(const unsigned x) {
    rs1_ = getBits (x, 15, 19);
    rs2_ = getBits (x, 20, 24);

    for (int i = 12; i < 32; ++i)
        imm_ |= getBits (x, 31, 31) << i;
    imm_ |= getBits (x, 8, 11) << 1 | getBits (x, 25, 30) << 5 | getBits (x, 7, 7) << 11;
}

//-----------------------------------------------------------------------------------------------------

void Instruction::UTypeFill(const unsigned x) {
    rd_ = getBits (x, 7, 11);
    imm_ |= getBits (x, 12, 31);
}

//-----------------------------------------------------------------------------------------------------

void Instruction::JTypeFill(const unsigned x) {
    rd_ = getBits (x, 7, 11);

    for (int i = 20; i < 32; ++i)
        imm_ |= getBits (x, 31, 31) << i;
    imm_ |= getBits (x, 12, 19) << 12 | getBits (x, 20, 20) << 11 | getBits (x, 21, 30) << 1;
}

//=====================================================================================================

int Instruction::getFormat (const unsigned x) {
        binDump(x);
        
        switch (getBits (x, 0, 6)) {
            case 0b0110111 : UTypeFill (x); return LUI_OPCODE;
            case 0b0010111 : UTypeFill (x); return AUIPC_OPCODE;
            case 0b1101111 : JTypeFill (x); return JAL_OPCODE;
            case 0b1100111 : ITypeFill (x); return JALR_OPCODE;
            case 0b1100011 : BTypeFill (x); return BRANCH_OPCODE;
            case 0b0000011 : ITypeFill (x); return LOAD_OPCODE;
            case 0b0100011 : STypeFill (x); return STORE_OPCODE;
            case 0b0010011 : ITypeFill (x); return OP_IMM_OPCODE;
            case 0b0110011 : RTypeFill (x); return OP_OPCODE;
            
            default:
                std::cout << "undefined opcode!" << std::endl;
                abort();
        }
        return -1;
    }

//-----------------------------------------------------------------------------------------------------

void Instruction::getInstr (const unsigned x) {
    switch (getFormat(x)) {
        case LUI_OPCODE    : executor_ = &Instruction::addFunc;      break;
        case AUIPC_OPCODE  : executor_ = &Instruction::auipcFunc;    break;
        case JAL_OPCODE    : executor_ = &Instruction::jalFunc;      break;
        case JALR_OPCODE   : executor_ = &Instruction::jalrFunc;     break;
        case BRANCH_OPCODE :
            switch (getBits (x, 12, 14)) {
                case 0 :    executor_ = &Instruction::beqFunc;  break;
                case 1 :    executor_ = &Instruction::bneFunc;  break;
                case 4 :    executor_ = &Instruction::bltFunc;  break;
                case 5 :    executor_ = &Instruction::bgeFunc;  break;
                case 6 :    executor_ = &Instruction::bltuFunc; break;
                case 7 :    executor_ = &Instruction::bgeuFunc; break;

                default:
                    std::cout << "undefined branch Instruction!" << std::endl;
                    abort();
            }
            break;
        case LOAD_OPCODE   :
            switch (getBits (x, 12, 14)) {
                case 0 :    executor_ = &Instruction::lbFunc; break;
                case 2 :    executor_ = &Instruction::lwFunc; break;

                default:
                    std::cout << "undefined load Instruction!" << std::endl;
                    abort();
            }
            break;
        case STORE_OPCODE  :
            switch (getBits (x, 12, 14)) {
                case 0 :    executor_ = &Instruction::sbFunc; break;
                case 2 :    executor_ = &Instruction::swFunc; break;

                default:
                    std::cout << "undefined store Instruction!" << std::endl;
                    abort();
            }
            break;
        case OP_IMM_OPCODE :
            switch (getBits (x, 12, 14)) {
                case 0 :    executor_ = &Instruction::addiFunc; break;
                case 4 :    executor_ = &Instruction::xoriFunc; break;
                case 6 :    executor_ = &Instruction::oriFunc;  break;
                case 7 :    executor_ = &Instruction::andiFunc; break;
                
                default:
                    std::cout << "undefined op imm Instruction!" << std::endl;
                    abort();
            }
            break;
        case OP_OPCODE     :
            switch (getBits (x, 25, 31)) {
                case 0          :
                    switch (getBits (x, 12, 14)) {
                        case 0 : executor_ = &Instruction::addFunc; break;
                        case 1 : executor_ = &Instruction::sllFunc; break;
                        case 4 : executor_ = &Instruction::xorFunc; break;
                        case 5 : executor_ = &Instruction::srlFunc; break;
                        case 6 : executor_ = &Instruction::orFunc;  break;
                        case 7 : executor_ = &Instruction::andFunc; break;

                        default:
                            std::cout << "0 undefined op Instruction!" << std::endl;
                            abort();
                    }
                    break;
                case 0b0100000  :
                    switch (getBits (x, 12, 14)) {
                        case 0 : executor_ = &Instruction::subFunc; break;

                        default:
                            std::cout << "1 undefined op Instruction!" << std::endl;
                            abort();
                    }
                    break;
                
                default:
                    std::cout << "undefined op Instruction " << getBits (x, 25, 31) << std::endl;
                    abort();
            }
            break;

        default:
            std::cout << "undefined opcode!" << std::endl;
            abort();
    }
}

//=====================================================================================================

bool Instruction::luiFunc (State &state) {
    state.pc += 4;

    if (rd_ == 0)
        return 0;

    state.regs[rd_] = imm_ << 12;

    #ifdef DUMP
        std::cout << "LUI Instruction" << std::endl;
        std::cout << "  x" << rd_ << " = " << imm_ << " << 12" << std::endl;
        std::cout << "  x" << rd_ << " = " << state.regs[rd_] << std::endl;
    #endif

    return 0;
}

//-----------------------------------------------------------------------------------------------------

bool Instruction::auipcFunc (State &state) {

    state.pc += 4;
    if (rd_ == 0)
        return 0;

    state.regs[rd_] = imm_ << 12 + state.pc;

    #ifdef DUMP
        std::cout << "AUIPC Instruction" << std::endl;
        std::cout << "  x" << rd_ << " = " << imm_ << " << 12 + pc" << state.pc << std::endl;
        std::cout << "  x" << rd_ << " = " << state.regs[rd_] << std::endl;
    #endif

    return 0;
}

//-----------------------------------------------------------------------------------------------------

bool Instruction::jalFunc (State &state) {
    
    if (rd_ != 0)
        state.regs[rd_] = state.pc + 4;

    state.pc += imm_;

    #ifdef DUMP
        std::cout << "JAL Instruction" << std::endl;
        if (rd_ != 0)
            std::cout << "  x" << rd_ << " = pc + 4 ("  << state.regs[rd_] << ")" << std::endl;
        std::cout << "  pc = pc + imm_ (" << state.pc << ")" << std::endl;
    #endif

    return 0;
}

//-----------------------------------------------------------------------------------------------------

bool Instruction::jalrFunc (State &state) {
    if (rd_ != 0)
        state.regs[rd_] = state.pc + 4;
    
    state.pc = state.regs[rs1_] + imm_;
    state.pc &= 0xFFFFFFFE;

    #ifdef DUMP
            std::cout << "JALR Instruction" << std::endl;
            std::cout << "  x" << rd_ << " (" << state.regs[rd_] << ") == pc + 4 (" << state.regs[rd_] << ")" << std::endl;
            std::cout << " pc = x[" << rs1_ << "] + " << imm_ << " (" << state.pc << ")" << std::endl;
    #endif
    
    return 0;
}

//-----------------------------------------------------------------------------------------------------


bool Instruction::beqFunc (State &state) {

    state.pc += 4;
    if (state.regs[rs1_] == state.regs[rs2_]) {
        state.pc += imm_;

        #ifdef DUMP
            std::cout << "BEQ Instruction" << std::endl;
            std::cout << "  x" << rs1_ << " (" << state.regs[rs1_] << ") == x" << rs2_ << " (" << state.regs[rs2_] << ")" << std::endl;
            std::cout << "  move on " << imm_ << std::endl;
        #endif

        return 0;
    }


    #ifdef DUMP
        std::cout << "BEQ Instruction" << std::endl;
        std::cout << "  x" << rs1_ << " (" << state.regs[rs1_] << ") != x" << rs2_ << " (" << state.regs[rs2_] << ")" << std::endl;
        std::cout << "  don't move" << std::endl;
    #endif

    return 0;
}

//-----------------------------------------------------------------------------------------------------

bool Instruction::bneFunc (State &state) {

    state.pc += 4;

    if (state.regs[rs1_] != state.regs[rs2_]) {
        state.pc += imm_;

        #ifdef DUMP
            std::cout << "BNE Instruction" << std::endl;
            std::cout << "  x" << rs1_ << " (" << state.regs[rs1_] << ") != x" << rs2_ << " (" << state.regs[rs2_] << ")" << std::endl;
            std::cout << "  move on " << imm_ << std::endl;
        #endif

        return 0;
    }

    #ifdef DUMP
        std::cout << "BNE Instruction" << std::endl;
        std::cout << "  x" << rs1_ << " (" << state.regs[rs1_] << ") == x" << rs2_ << " (" << state.regs[rs2_] << ")" << std::endl;
        std::cout << "  don't move" << std::endl;
    #endif

    return 0;
}

//-----------------------------------------------------------------------------------------------------

bool Instruction::bltFunc (State &state) {

    state.pc += 4;

    if (state.regs[rs1_] < state.regs[rs2_]) {
        state.pc += imm_;

        #ifdef DUMP
            std::cout << "BLT Instruction" << std::endl;
            std::cout << "  x" << rs1_ << " (" << state.regs[rs1_] << ") < x" << rs2_ << " (" << state.regs[rs2_] << ")" << std::endl;
            std::cout << "  move on " << imm_ << std::endl;
        #endif

        return 0;
    }

    #ifdef DUMP
        std::cout << "BLT Instruction" << std::endl;
        std::cout << "  x" << rs1_ << " (" << state.regs[rs1_] << ") >= x" << rs2_ << " (" << state.regs[rs2_] << ")" << std::endl;
        std::cout << "  don't move" << std::endl;
    #endif

    return 0;
}

//-----------------------------------------------------------------------------------------------------

bool Instruction::bgeFunc (State &state) {

    state.pc += 4;

    if (state.regs[rs1_] >= state.regs[rs2_]) {
        state.pc += imm_;

        #ifdef DUMP
            std::cout << "BGE Instruction" << std::endl;
            std::cout << "  x" << rs1_ << " (" << state.regs[rs1_] << ") >= x" << rs2_ << " (" << state.regs[rs2_] << ")" << std::endl;
            std::cout << "  move on " << imm_ << std::endl;
        #endif

        return 0;
    }

    #ifdef DUMP
        std::cout << "BGE Instruction" << std::endl;
        std::cout << "  x" << rs1_ << " (" << state.regs[rs1_] << ") < x" << rs2_ << " (" << state.regs[rs2_] << ")" << std::endl;
        std::cout << "  don't move" << std::endl;
    #endif

    return 0;
}

//-----------------------------------------------------------------------------------------------------

bool Instruction::bltuFunc (State &state) {

    state.pc += 4;

    if (unsigned (state.regs[rs1_]) < unsigned (state.regs[rs2_])) {
        state.pc += imm_;

        #ifdef DUMP
            std::cout << "BLTU Instruction" << std::endl;
            std::cout << "  x" << rs1_ << " (" << unsigned (state.regs[rs1_]) << ") < x" << rs2_ << " (" << unsigned (state.regs[rs2_]) << ")" << std::endl;
            std::cout << "  move on " << imm_ << std::endl;
        #endif

        return 0;
    }

    #ifdef DUMP
        std::cout << "BNE Instruction" << std::endl;
        std::cout << "  x" << rs1_ << " (" << unsigned (state.regs[rs1_]) << ") >= x" << rs2_ << " (" << unsigned (state.regs[rs2_]) << ")" << std::endl;
        std::cout << "  don't move" << std::endl;
    #endif

    return 0;
}

//-----------------------------------------------------------------------------------------------------

bool Instruction::bgeuFunc (State &state) {

    state.pc += 4;

    if (unsigned (state.regs[rs1_]) >= unsigned (state.regs[rs2_])) {
        state.pc += imm_;

        #ifdef DUMP
            std::cout << "BGEU Instruction" << std::endl;
            std::cout << "  x" << rs1_ << " (" << unsigned (state.regs[rs1_]) << ") >= x" << rs2_ << " (" << unsigned (state.regs[rs2_]) << ")" << std::endl;
            std::cout << "  move on " << imm_ << std::endl;
        #endif

        return 0;
    }

    #ifdef DUMP
        std::cout << "BGEU Instruction" << std::endl;
        std::cout << "  x" << rs1_ << " (" << unsigned (state.regs[rs1_]) << ") < x" << rs2_ << " (" << unsigned (state.regs[rs2_]) << ")" << std::endl;
        std::cout << "  don't move" << std::endl;
    #endif

    return 0;
}

//-----------------------------------------------------------------------------------------------------

bool Instruction::lbFunc (State &state) {

    state.memory.read(state.regs[rs1_] + imm_, state.regs[rd_]);

    state.regs[rd_] >>= 24;
    for(int i = 8; i < 32; ++i)
        state.regs[rd_] |= getBits (state.regs[rd_], 7, 7);

    return 0;
}

//-----------------------------------------------------------------------------------------------------

bool Instruction::lwFunc (State &state) {

    state.memory.read(state.regs[rs1_] + imm_, state.regs[rd_]);

    return 0;
}

//-----------------------------------------------------------------------------------------------------


bool Instruction::sbFunc (State &state) {

    state.memory.write(state.regs[rs1_] + imm_, regType(state.regs[rs2_] & 0xFF));

    return 0;
}

//-----------------------------------------------------------------------------------------------------

bool Instruction::swFunc (State &state) {

    state.memory.write(state.regs[rs1_] + imm_, state.regs[rs2_]);
    return 0;
}

//-----------------------------------------------------------------------------------------------------


bool Instruction::addiFunc (State &state) {
    state.pc += 4;

    if(rd_ == 0)
        return 0;
    
    state.regs[rd_] = state.regs[rs1_] + imm_;

    #ifdef DUMP
        std::cout << "ADDI Instruction" << std::endl;
        std::cout << "  x" << rd_ << " = x" << rs1_ << " (" << state.regs[rs1_] << ") + " << imm_ << std::endl;
        std::cout << "  x" << rd_ << " = " << state.regs[rd_] << std::endl;
    #endif

    return 0;
}

//-----------------------------------------------------------------------------------------------------

bool Instruction::oriFunc (State &state) {
    state.pc += 4;
    
    if(rd_ == 0)
        return 0;
    
    state.regs[rd_] = state.regs[rs1_] | imm_;

    #ifdef DUMP
        std::cout << "ORI Instruction" << std::endl;
        std::cout << "  x" << rd_ << " = x" << rs1_ << " (" << state.regs[rs1_] << ") | " << imm_ << std::endl;
        std::cout << "  x" << rd_ << " = " << state.regs[rd_] << std::endl;
    #endif

    return 0;
}

//-----------------------------------------------------------------------------------------------------

bool Instruction::andiFunc (State &state) {
    state.pc += 4;
    
    if(rd_ == 0)
        return 0;
    
    state.regs[rd_] = state.regs[rs1_] & imm_;

    #ifdef DUMP
        std::cout << "ANDI Instruction" << std::endl;
        std::cout << "  x" << rd_ << " = x" << rs1_ << " (" << state.regs[rs1_] << ") & " << imm_ << std::endl;
        std::cout << "  x" << rd_ << " = " << state.regs[rd_] << std::endl;
    #endif

    return 0;
}

//-----------------------------------------------------------------------------------------------------

bool Instruction::xoriFunc (State &state) {
    state.pc += 4;
    
    if(rd_ == 0)
        return 0;
    
    state.regs[rd_] = state.regs[rs1_] ^ imm_;

    #ifdef DUMP
        std::cout << "XORI Instruction" << std::endl;
        std::cout << "  x" << rd_ << " = x" << rs1_ << " (" << state.regs[rs1_] << ") ^ " << imm_ << std::endl;
        std::cout << "  x" << rd_ << " = " << state.regs[rd_] << std::endl;
    #endif

    return 0;
}

//-----------------------------------------------------------------------------------------------------


bool Instruction::addFunc (State &state) {
    state.pc += 4;
    
    if(rd_ == 0)
        return 0;
    
    state.regs[rd_] = state.regs[rs1_] + state.regs[rs2_];

    #ifdef DUMP
        std::cout << "ADD Instruction" << std::endl;
        std::cout << "  x" << rd_ << " = x" << rs1_ << " (" << state.regs[rs1_] << ") + x" << rs2_ << " (" << state.regs[rs2_] << ")" << std::endl;
        std::cout << "  x" << rd_ << " = " << state.regs[rd_] << std::endl;
    #endif

    return 0;
}

//-----------------------------------------------------------------------------------------------------

bool Instruction::subFunc (State &state) {
    state.pc += 4;
    
    if(rd_ == 0)
        return 0;
    
    state.regs[rd_] = state.regs[rs1_] - state.regs[rs2_];

    #ifdef DUMP
        std::cout << "SUB Instruction" << std::endl;
        std::cout << "  x" << rd_ << " = x" << rs1_ << " (" << state.regs[rs1_] << ") - x" << rs2_ << " (" << state.regs[rs2_] << ")" << std::endl;
        std::cout << "  x" << rd_ << " = " << state.regs[rd_] << std::endl;
    #endif

    return 0;
}

//-----------------------------------------------------------------------------------------------------

bool Instruction::orFunc (State &state) {
    state.pc += 4;
    
    if(rd_ == 0)
        return 0;
    
    state.regs[rd_] = state.regs[rs1_] | state.regs[rs2_];

    #ifdef DUMP
        std::cout << "OR Instruction" << std::endl;
        std::cout << "  x" << rd_ << " = x" << rs1_ << " (" << state.regs[rs1_] << ") | x" << rs2_ << " (" << state.regs[rs2_] << ")" << std::endl;
        std::cout << "  x" << rd_ << " = " << state.regs[rd_] << std::endl;
    #endif

    return 0;
}

//-----------------------------------------------------------------------------------------------------

bool Instruction::andFunc (State &state) {
    state.pc += 4;
    
    if(rd_ == 0)
        return 0;
    
    state.regs[rd_] = state.regs[rs1_] & state.regs[rs2_];

    #ifdef DUMP
        std::cout << "AND Instruction" << std::endl;
        std::cout << "  x" << rd_ << " = x" << rs1_ << " (" << state.regs[rs1_] << ") & x" << rs2_ << " (" << state.regs[rs2_] << ")" << std::endl;
        std::cout << "  x" << rd_ << " = " << state.regs[rd_] << std::endl;
    #endif

    return 0;
}

//-----------------------------------------------------------------------------------------------------

bool Instruction::xorFunc (State &state) {
    state.pc += 4;
    
    if(rd_ == 0)
        return 0;
    
    state.regs[rd_] = state.regs[rs1_] ^ state.regs[rs2_];

    #ifdef DUMP
        std::cout << "XOR Instruction" << std::endl;
        std::cout << "  x" << rd_ << " = x" << rs1_ << " (" << state.regs[rs1_] << ") ^ x" << rs2_ << " (" << state.regs[rs2_] << ")" << std::endl;
        std::cout << "  x" << rd_ << " = " << state.regs[rd_] << std::endl;
    #endif

    return 0;
}

//-----------------------------------------------------------------------------------------------------

bool Instruction::sllFunc (State &state) {
    state.pc += 4;
    
    if(rd_ == 0)
        return 0;
    
    state.regs[rd_] = unsigned (state.regs[rs1_]) << unsigned (state.regs[rs2_]);

    #ifdef DUMP
        std::cout << "SLL Instruction" << std::endl;
        std::cout << "  x" << rd_ << " = x" << rs1_ << " (" << state.regs[rs1_] << ") << x" << rs2_ << " (" << state.regs[rs2_] << ")" << std::endl;
        std::cout << "  x" << rd_ << " = " << state.regs[rd_] << std::endl;
    #endif

    return 0;
}

//-----------------------------------------------------------------------------------------------------

bool Instruction::srlFunc (State &state) {
    state.pc += 4;
    
    if(rd_ == 0)
        return 0;
    
    state.regs[rd_] = unsigned (state.regs[rs1_]) >> unsigned (state.regs[rs2_]);

    #ifdef DUMP
        std::cout << "SRL Instruction" << std::endl;
        std::cout << "  x" << rd_ << " = x" << rs1_ << " (" << state.regs[rs1_] << ") >> x" << rs2_ << " (" << state.regs[rs2_] << ")" << std::endl;
        std::cout << "  x" << rd_ << " = " << state.regs[rd_] << std::endl;
    #endif

    return 0;
}