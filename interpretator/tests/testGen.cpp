#include <iostream>
#include <fstream>


int main(int argc, char *argv[]) {

    FILE *inp = fopen(argv[1], "r");
    FILE *out = fopen(argv[2]  , "w");

    int count = 0, x = 0;
    fscanf(inp, "%d", &count);
    
    for (int i = 0; i < count; ++i) {
        fscanf(inp, "%x", &x);
        fprintf(out, "%c%c%c%c", (x >> 24) & 0xff, (x >> 16) & 0xff, (x >> 8) & 0xff, x & 0xff);
    }

    fclose(inp);
    fclose(out);
    return 0;
}