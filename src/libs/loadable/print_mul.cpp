#include <dlfcn.h>
#include <cstdlib>
#include <cstdio>

int main() {
    using extern_func = int (*)(int, int);

    void *extern_library = dlopen("./libloadable_lib.so", RTLD_LAZY);

    if (extern_library == nullptr) {
        printf("ERROR: %s\n", dlerror());
        exit(1);
    }

    auto mul = (extern_func) dlsym(extern_library, "mul");

    if (mul == nullptr) {
        printf("ERROR: %s\n", dlerror());
        dlclose(extern_library);
        exit(2);
    }

    printf("%d\n", mul(5, 15));
    dlclose(extern_library);
    return 0;
}