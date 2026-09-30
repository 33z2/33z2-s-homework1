#ifndef SORT_H
#define SORT_H

#include <stddef.h>
#include <stdint.h>

#ifndef SORT_COUNTING
#define SORT_COUNTING 1
#endif

/* key만 정렬 기준으로 사용합니다. original_index는 안정성 검사용입니다. */
typedef struct {
    int key;
    uint32_t original_index;
} Item;

typedef struct {
    uint64_t comparisons;       /* 실제 원소의 key를 비교한 횟수 */
    uint64_t moves;             /* Item 한 개를 복사한 횟수; 교환은 3회 */
    size_t heap_buffer_bytes;   /* 정렬 내부의 추가 동적 배열; 스택 제외 */
} SortStats;

typedef int (*SortFunction)(Item *a, size_t n, SortStats *stats);
typedef struct {
    const char *name;
    SortFunction sort;
    int stable;
} SortAlgorithm;

/* 성공 1, 잘못된 인수 또는 메모리 할당 실패 0. NULL,0은 빈 배열입니다. */
int insertionSort(Item *a, size_t n, SortStats *stats);
int mergeSort(Item *a, size_t n, SortStats *stats);
int heapSort(Item *a, size_t n, SortStats *stats);

extern const SortAlgorithm ALGORITHMS[3];

/* 시간 측정용 빌드는 SORT_COUNTING=0으로 모든 카운터 증가를 제거합니다. */
static inline int greaterKey(int x, int y, SortStats *stats)
{
#if SORT_COUNTING
    if (stats != NULL) ++stats->comparisons;
#else
    (void)stats;
#endif
    return x > y;
}

static inline void moveItem(Item *to, const Item *from, SortStats *stats)
{
    *to = *from;
#if SORT_COUNTING
    if (stats != NULL) ++stats->moves;
#else
    (void)stats;
#endif
}

static inline void swapItems(Item *a, Item *b, SortStats *stats)
{
    Item temp;
    moveItem(&temp, a, stats);
    moveItem(a, b, stats);
    moveItem(b, &temp, stats);
}

static inline int startSort(Item *a, size_t n, SortStats *stats)
{
    if (stats != NULL) *stats = (SortStats){0, 0, 0};
    return a != NULL || n == 0;
}
#endif
