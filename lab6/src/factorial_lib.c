#include "factorial_lib.h"
#include <stdlib.h>
#include <errno.h>
#include <pthread.h>

uint64_t MultModulo(uint64_t a, uint64_t b, uint64_t mod) {
    uint64_t result = 0;
    a = a % mod;
    while (b > 0) {
        if (b % 2 == 1)
            result = (result + a) % mod;
        a = (a * 2) % mod;
        b /= 2;
    }
    return result % mod;
}

uint64_t Factorial(const struct FactorialArgs *args) {
    uint64_t ans = 1;
    for (uint64_t i = args->begin; i <= args->end; i++) {
        ans = MultModulo(ans, i, args->mod);
    }
    return ans;
}

void *ThreadFactorial(void *args) {
    struct FactorialArgs *fargs = (struct FactorialArgs *)args;
    uint64_t *result = malloc(sizeof(uint64_t));
    if (result != NULL) {
        *result = Factorial(fargs);
    }
    return (void *)result;
}

bool ConvertStringToUI64(const char *str, uint64_t *val) {
    char *end = NULL;
    unsigned long long i = strtoull(str, &end, 10);
    if (errno == ERANGE) {
        return false;
    }
    if (errno != 0) {
        return false;
    }
    *val = i;
    return true;
}