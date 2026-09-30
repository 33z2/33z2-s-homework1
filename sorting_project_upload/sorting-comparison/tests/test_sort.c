#include "data.h"
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static unsigned long checks = 0, failures = 0;
static void check(int condition, const char *label)
{
    ++checks;
    if (!condition) {
        ++failures;
        fprintf(stderr, "FAIL: %s\n", label);
    }
}

static void testCase(const int *keys, size_t n, const char *label)
{
    size_t capacity = n ? n : 1;
    Item *input = malloc(capacity * sizeof *input);
    Item *expected = malloc(capacity * sizeof *expected);
    Item *actual = malloc(capacity * sizeof *actual);
    unsigned char *seen = malloc(capacity);
    if (!input || !expected || !actual || !seen) exit(EXIT_FAILURE);
    for (size_t i = 0; i < n; ++i) input[i] = (Item){keys[i], (uint32_t)i};
    memcpy(expected, input, n * sizeof *input);
    qsort(expected, n, sizeof *expected, compareItems);
    for (size_t k = 0; k < 3; ++k) {
        memcpy(actual, input, n * sizeof *input);
        SortStats stats;
        int ok = ALGORITHMS[k].sort(actual, n, &stats);
        int valid = ok && verifyResult(actual, input, expected, n, seen);
        if (ALGORITHMS[k].stable) valid = valid && isStable(actual, n);
        check(valid, label);
    }
    free(input); free(expected); free(actual); free(seen);
}

int main(void)
{
    const int basic[][10] = {
        {0}, {4}, {2,1}, {1,2,3,4,5}, {5,4,3,2,1},
        {4,1,3,2,5}, {2,2,1,1,2}, {7,7,7,7,7},
        {-3,4,0,-9,2}, {INT_MAX,0,INT_MIN,-1,INT_MAX}
    };
    const size_t lengths[] = {0,1,2,5,5,5,5,5,5,5};
    for (size_t i = 0; i < 10; ++i) testCase(basic[i], lengths[i], "basic");

    /* -1, 0, 1로 이루어진 길이 0~7의 모든 배열을 전수 검사합니다. */
    for (size_t n = 0, possibilities = 1; n <= 7; ++n, possibilities *= 3) {
        for (size_t code = 0; code < possibilities; ++code) {
            int keys[7]; size_t value = code;
            for (size_t i = 0; i < n; ++i) { keys[i] = (int)(value % 3) - 1; value /= 3; }
            testCase(keys, n, "exhaustive ternary input");
        }
    }
    /* 난수 길이 전수와 병합 경계(2의 거듭제곱 전후)를 확인합니다. */
    for (size_t n = 0; n <= 128; ++n) {
        for (uint32_t seed = 1; seed <= 5; ++seed) {
            int keys[128]; uint32_t state = seed;
            for (size_t i = 0; i < n; ++i)
                keys[i] = (int)((nextRandom(&state) >> 16) % 31) - 15;
            testCase(keys, n, "random size sweep");
        }
    }
    const size_t edges[] = {255,256,257,511,512,513,999,1023,1024,1025};
    for (size_t e = 0; e < sizeof edges / sizeof edges[0]; ++e) {
        for (uint32_t seed = 1; seed <= 5; ++seed) {
            int keys[1025]; uint32_t state = seed;
            for (size_t i = 0; i < edges[e]; ++i)
                keys[i] = (int)(nextRandom(&state) % 20001) - 10000;
            testCase(keys, edges[e], "power of two boundaries");
        }
    }
    for (size_t k = 0; k < 3; ++k) {
        check(ALGORITHMS[k].sort(NULL, 0, NULL), "NULL empty input accepted");
        check(!ALGORITHMS[k].sort(NULL, 1, NULL), "NULL nonempty input rejected");
        Item a[] = {{2,0},{2,1},{1,2}};
        check(ALGORITHMS[k].sort(a, 3, NULL), "stability example sorts");
        check(isStable(a, 3) == ALGORITHMS[k].stable, "known stability counterexample");
    }
    /* 검사 함수가 중복·손상된 정렬 결과를 놓치지 않는지도 확인합니다. */
    {
        Item input[] = {{2,0},{1,1},{2,2}};
        Item expected[] = {{1,1},{2,0},{2,2}};
        Item invalid[] = {{1,1},{2,0},{2,0}};
        unsigned char seen[3];
        check(!verifyResult(invalid, input, expected, 3, seen), "duplicate tag rejected");
        invalid[2] = (Item){2,99};
        check(!verifyResult(invalid, input, expected, 3, seen), "out of range tag rejected");
        invalid[2] = (Item){3,2};
        check(!verifyResult(invalid, input, expected, 3, seen), "wrong key rejected");
        Item a[200];
        for (int p = 0; p < 6; ++p) {
            makeInput(a, 200, p, 20260930);
            int ok = 1;
            for (size_t i = 0; i < 200; ++i) {
                if (a[i].original_index != i) ok = 0;
                if (p == 1 && a[i].key != (int)i) ok = 0;
                if (p == 2 && a[i].key != (int)(200 - i)) ok = 0;
                if (p == 4 && (a[i].key < 0 || a[i].key > 15)) ok = 0;
                if (p == 5 && a[i].key != 7) ok = 0;
            }
            if (p == 3) {
                size_t inversions = 0;
                for (size_t i = 0; i < 200; ++i)
                    for (size_t j = i + 1; j < 200; ++j)
                        inversions += a[i].key > a[j].key;
                ok = ok && inversions <= 2;
            }
            check(ok, "input pattern generation");
        }
    }
#if SORT_COUNTING
    {
        Item a[20]; SortStats s;
        for (size_t i = 0; i < 20; ++i) a[i] = (Item){(int)i,(uint32_t)i};
        insertionSort(a, 20, &s);
        check(s.comparisons == 19 && s.moves == 38 && s.heap_buffer_bytes == 0,
              "insertion sorted counts");
        for (size_t i = 0; i < 20; ++i) a[i] = (Item){(int)(20-i),(uint32_t)i};
        insertionSort(a, 20, &s);
        check(s.comparisons == 190 && s.moves == 228, "insertion reverse counts");
        for (size_t i = 0; i < 4; ++i) a[i] = (Item){(int)i,(uint32_t)i};
        mergeSort(a, 4, &s);
        check(s.comparisons == 4 && s.moves == 16 && s.heap_buffer_bytes == 4*sizeof(Item),
              "merge counters and buffer size");
        for (size_t i = 0; i < 4; ++i) a[i] = (Item){7,(uint32_t)i};
        heapSort(a, 4, &s);
        check(s.comparisons == 6 && s.moves == 9 && s.heap_buffer_bytes == 0,
              "heap all equal counts");
    }
#endif
    printf("SORT_COUNTING=%d: %lu checks, %lu failures\n", SORT_COUNTING, checks, failures);
    return failures ? EXIT_FAILURE : EXIT_SUCCESS;
}
