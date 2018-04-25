#include <cstdio>
#include "add.h"

int add(int a, int b) {
    printf("LOG: Called function add from static library\n");
    return a + b;
}