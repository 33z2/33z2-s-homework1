#if !defined(_WIN32)
#define _POSIX_C_SOURCE 200809L
#endif
#include "data.h"
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#if defined(_WIN32)
#include <windows.h>
#else
#include <time.h>
#endif

enum { REPEATS = 7 };
static const size_t SIZES[] = {1000, 2000, 4000, 8000, 16000};

static double nowSeconds(void)
{
#if defined(_WIN32)
    LARGE_INTEGER count, frequency;
    if (!QueryPerformanceFrequency(&frequency) || !QueryPerformanceCounter(&count)) {
        fputs("High-resolution timer failed.\n", stderr); exit(EXIT_FAILURE);
    }
    return (double)count.QuadPart / (double)frequency.QuadPart;
#else
    struct timespec ts;
    if (clock_gettime(CLOCK_MONOTONIC, &ts) != 0) {
        perror("clock_gettime"); exit(EXIT_FAILURE);
    }
    return (double)ts.tv_sec + (double)ts.tv_nsec / 1e9;
#endif
}

static uint32_t inputSeed(size_t n, int pattern, int trial)
{
    return UINT32_C(20260930) ^ (uint32_t)n * UINT32_C(2654435761) ^
           (uint32_t)pattern * UINT32_C(2246822519) ^
           (uint32_t)trial * UINT32_C(3266489917);
}

static int demo(void)
{
    const Item original[] = {{4,0}, {1,1}, {3,2}, {2,3}, {5,4}};
    for (size_t k = 0; k < 3; ++k) {
        Item a[5]; memcpy(a, original, sizeof a);
        if (!ALGORITHMS[k].sort(a, 5, NULL)) return EXIT_FAILURE;
        printf("%-10s:", ALGORITHMS[k].name);
        for (size_t i = 0; i < 5; ++i) printf(" %d", a[i].key);
        putchar('\n');
    }
    puts("Stability example: input (2,A) (2,B) (1,C)");
    for (size_t k = 0; k < 3; ++k) {
        Item a[] = {{2,0}, {2,1}, {1,2}};
        if (!ALGORITHMS[k].sort(a, 3, NULL)) return EXIT_FAILURE;
        printf("%-10s:", ALGORITHMS[k].name);
        for (size_t i = 0; i < 3; ++i)
            printf(" (%d,%c)", a[i].key, (int)('A' + a[i].original_index));
        printf("  stable_observed=%d\n", isStable(a, 3));
    }
    return EXIT_SUCCESS;
}

static int benchmark(int counts)
{
    if (counts != !!SORT_COUNTING) {
        fputs("Wrong executable: use make bench (separate timing/counting builds).\n", stderr);
        return EXIT_FAILURE;
    }
    if (counts)
        puts("n,pattern,algorithm,seed,comparisons,moves,heap_buffer_bytes,valid,stable_observed");
    else
        puts("n,pattern,algorithm,trial,seed,elapsed_ms,valid,stable_observed");
    for (size_t si = 0; si < sizeof SIZES / sizeof SIZES[0]; ++si) {
        const size_t n = SIZES[si];
        Item *input = malloc(n * sizeof *input);
        Item *work = malloc(n * sizeof *work);
        Item *expected = malloc(n * sizeof *expected);
        unsigned char *seen = malloc(n);
        if (!input || !work || !expected || !seen) {
            fputs("Input allocation failed.\n", stderr);
            free(input); free(work); free(expected); free(seen); return EXIT_FAILURE;
        }
        for (int p = 0; p < 6; ++p) {
            /* 시간 측정 시에만 별도 seed의 워밍업 1회를 수행하고 통계에서 제외합니다. */
            int first = counts ? 0 : -1;
            int last = counts ? 1 : REPEATS;
            for (int trial = first; trial < last; ++trial) {
                const uint32_t seed = inputSeed(n, p, trial);
                makeInput(input, n, p, seed);
                memcpy(expected, input, n * sizeof *input);
                qsort(expected, n, sizeof *expected, compareItems);
                /* 실행 순서를 매번 바꾸며, 세 정렬에는 같은 원본 입력의 사본을 줍니다. */
                for (size_t order = 0; order < 3; ++order) {
                    size_t k = (order + (size_t)(trial + 1)) % 3;
                    memcpy(work, input, n * sizeof *work);  /* 측정 구간 밖 */
                    SortStats stats;
                    const double start = counts ? 0.0 : nowSeconds();
                    int ok = ALGORITHMS[k].sort(work, n, counts ? &stats : NULL);
                    const double elapsed = counts ? 0.0 : (nowSeconds() - start) * 1000.0;
                    int valid = ok && verifyResult(work, input, expected, n, seen);
                    int stable = isStable(work, n);
                    if (!valid || (ALGORITHMS[k].stable && !stable)) {
                        fprintf(stderr, "Validation failed: %s n=%zu %s trial=%d\n",
                                ALGORITHMS[k].name, n, PATTERNS[p], trial);
                        free(input); free(work); free(expected); free(seen);
                        return EXIT_FAILURE;
                    }
                    if (counts) {
                        printf("%zu,%s,%s,%" PRIu32 ",%" PRIu64 ",%" PRIu64 ",%zu,1,%d\n",
                               n, PATTERNS[p], ALGORITHMS[k].name, seed,
                               stats.comparisons, stats.moves, stats.heap_buffer_bytes, stable);
                    } else if (trial >= 0) {
                        printf("%zu,%s,%s,%d,%" PRIu32 ",%.9f,1,%d\n",
                               n, PATTERNS[p], ALGORITHMS[k].name, trial, seed, elapsed, stable);
                    }
                }
            }
        }
        free(input); free(work); free(expected); free(seen);
    }
    return EXIT_SUCCESS;
}

int main(int argc, char **argv)
{
    if (argc == 1 || (argc == 2 && strcmp(argv[1], "--demo") == 0)) return demo();
    if (argc == 2 && strcmp(argv[1], "--time") == 0) return benchmark(0);
    if (argc == 2 && strcmp(argv[1], "--counts") == 0) return benchmark(1);
    fputs("Usage: program [--demo | --time | --counts]\n", stderr);
    return EXIT_FAILURE;
}
