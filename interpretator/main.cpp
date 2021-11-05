#include <vector>
#include <fstream>
#include <sys/stat.h>
#include "interpretator.hpp"

using instrFunc = bool (Instruction::* )(State &) const;

//-----------------------------------------------------------------------------------------------------

static void binDump(const unsigned x) {

    for (int i = 31; i >= 0; --i)
        printf("%d", (x >> i) & 1);

    printf("\n");
}

//-----------------------------------------------------------------------------------------------------

static int readBin (physicalMemory &memory, char *name) {

    struct stat st;
    stat(name, &st);

    std::ifstream file;
    file.open(name);
    if (!file.is_open()) {
        std::cout << "error of open file!!!" << std::endl;
        return 1;
    }

    unsigned x = 0;
    char c;

    for (int i = 0; i < st.st_size; ++i) {

        file.get(c);

        x |= ((unsigned char)(c)) << (8 * ((3 - memory.sizeCode) % 4));
        ++memory.sizeCode;

        if (memory.sizeCode % 4 != 0)
            continue;
        
        memory.write(memory.sizeCode - 4, x);
        x = 0;
    }
    file.close();

    return 0;
}

//=====================================================================================================

int main (int argc, char *argv[]) {

    if (argc < 1) {
        std::cout << "input file name!!!" << std::endl;
        return 0;
    }

    State state = {};
    if (readBin (state.memory, argv[1]))
        return 0;

    for (state.pc = 0; state.pc < state.memory.sizeCode; ) {

        Instruction a (state.memory.readInstr(state.pc));

        if(a.getBad())
            return 0;
        
        instrFunc func = a.executor_;
        (a.*(func))(state);
    }

    std::cout << state.regs[1] << std::endl;

    return 0;
}