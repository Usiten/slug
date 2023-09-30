#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include "tool.h"

char *SL_shift_args(int *argc, char ***argv)
{
    assert(*argc > 0);
    char *result = **argv;
    (*argv) += 1;
    (*argc) -= 1;
    return result;
}

char *SL_strndup(const char *s1, size_t n)
{
    char *dup = malloc(n+1);
    memcpy(dup, s1, n);
    dup[n] = '\0';

    return dup;
};