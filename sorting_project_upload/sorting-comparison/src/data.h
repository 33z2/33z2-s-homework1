#ifndef DATA_H
#define DATA_H
#include "sort.h"
extern const char *const PATTERNS[6];
uint32_t nextRandom(uint32_t *state);
void makeInput(Item *a, size_t n, int pattern, uint32_t seed);
int compareItems(const void *x, const void *y);
int isStable(const Item *a, size_t n);
int verifyResult(const Item *a, const Item *input, const Item *expected,
                 size_t n, unsigned char *seen);
#endif
