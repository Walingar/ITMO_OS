#include <cstdio>
#include "sub.h"

int sub(int a, int b) {
    printf("LOG: Called function from shared library\n");
    return a - b;
}