#include <csignal>
#include <cstdlib>
#include <cstring>
#include <bits/siginfo.h>
#include <cstdio>
#include <ucontext.h>
#include <cmath>
#include <algorithm>
#include <climits>
#include <bits/sigset.h>
#include <sys/ucontext.h>
#include <csetjmp>

inline const char *registerToString(int reg) {
    switch (reg) {
        case 0:
            return "REG_R8";
        case 1:
            return "REG_R9";
        case 2:
            return "REG_R10";
        case 3:
            return "REG_R11";
        case 4:
            return "REG_R12";
        case 5:
            return "REG_R13";
        case 6:
            return "REG_R14";
        case 7:
            return "REG_R15";
        case 8:
            return "REG_RDI";
        case 9:
            return "REG_RSI";
        case 10:
            return "REG_RBP";
        case 11:
            return "REG_RBX";
        case 12:
            return "REG_RDX";
        case 13:
            return "REG_RAX";
        case 14:
            return "REG_RCX";
        case 15:
            return "REG_RSP";
        case 16:
            return "REG_RIP";
        case 17:
            return "REG_EFL";
        case 18:
            return "REG_CSGSFS";
        case 19:
            return "REG_ERR";
        case 20:
            return "REG_TRAPNO";
        case 21:
            return "REG_OLDMASK";
        case 22:
            return "REG_CR2";
        default:
            return "REG_UNKNOWN";
    }
}

static jmp_buf jbuf;

void handler_sigsegv_address(int signum, siginfo_t *siginfo, void *context) {
    if (siginfo->si_signo == SIGSEGV) {
        siglongjmp(jbuf, 1);
    }
}

void address_dump(long long address) {
    sigset_t signal_set;
    sigemptyset(&signal_set);
    sigaddset(&signal_set, SIGSEGV);
    sigprocmask(SIG_UNBLOCK, &signal_set, NULL);


    struct sigaction act;
    memset(&act, 0, sizeof(act));
    act.sa_sigaction = handler_sigsegv_address;
    act.sa_flags = SA_SIGINFO;
    if (sigaction(SIGSEGV, &act, NULL) < 0) {
        perror("sigaction");
        exit(-1);
    }
    const auto *p = reinterpret_cast< const char *>( address );
    if (setjmp(jbuf) == 0) {
        printf("ADDRESS_0x%-10p\t%x\n", reinterpret_cast<void *>(address), int(p[0]));
    } else {
        printf("ADDRESS_0x%-10p\t(bad)\n", reinterpret_cast<void *>(address));
    }
}

void memory_dump(void *address) {
    printf("MEMORY DUMP\n");
    long long from = std::max((long long) 0, (long long) ((char *) address - 20 * sizeof(char)));
    long long to = std::min(LONG_LONG_MAX, (long long) ((char *) address + 20 * (sizeof(char))));
    for (long long i = from; i < to; i += sizeof(char)) {
        address_dump(i);
    }
}

void registers_dump(ucontext_t *context) {
    printf("REGISTERS\n");
    for (int reg = 0; reg < NGREG; reg++) {
        printf("%-15s\t0x%x\n", registerToString(reg), (unsigned int) context->uc_mcontext.gregs[reg]);
    }
}

void handler_sigsegv(int signum, siginfo_t *siginfo, void *context) {
    if (siginfo->si_signo == SIGSEGV) {
        printf("ERROR. Segmentation fault. Tried to access %p. ", siginfo->si_addr);
        switch (siginfo->si_code) {
            case SEGV_MAPERR:
                printf("Address not mapped to object.\n");
                break;
            case SEGV_ACCERR:
                printf("Invalid permissions for mapped object.\n");
                break;
            default:
                printf("Unknown error.\n");
        }
        printf("LOG. Context.\n");
        registers_dump((ucontext_t *) context);
        memory_dump(siginfo->si_addr);
    }
    exit(1);
}

void fall1() {
    char *c = const_cast<char *>("FOOOOOO");
    printf("String starts from:%p\n", static_cast<void *>(c));
    c[10] = 'z';
}

void fall2() {
    int *c = reinterpret_cast<int *>(0x20);
    *c = 10;
}

int main(int argc, char **argv) {
    struct sigaction act;
    memset(&act, 0, sizeof(act));
    act.sa_sigaction = handler_sigsegv;
    act.sa_flags = SA_SIGINFO;
    if (sigaction(SIGSEGV, &act, nullptr) < 0) {
        perror("sigaction");
        exit(-1);
    }
    fall1();
    // uncomment it to see bad addresses in dump and comment previous one
    // fall2();
    return 0;
}