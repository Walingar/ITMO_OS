#include <cstdio>
#include "mul.h"

extern "C"
int mul(int a, int b) {
    printf("LOG: Called function from loadable library\n");
    return a * b;
}