#include "sort.h"
#include <stdlib.h>

/* [lo, hi)를 정렬합니다. 보조 배열은 최초 호출에서 한 번만 확보합니다. */
static void mergeRange(Item *a, Item *buffer, size_t lo, size_t hi,
                       SortStats *stats)
{
    if (hi - lo < 2) return;
    const size_t mid = lo + (hi - lo) / 2;
    mergeRange(a, buffer, lo, mid, stats);
    mergeRange(a, buffer, mid, hi, stats);

    for (size_t k = lo; k < hi; ++k)
        moveItem(&buffer[k], &a[k], stats);

    size_t left = lo, right = mid;
    for (size_t k = lo; k < hi; ++k) {
        if (left == mid) {
            moveItem(&a[k], &buffer[right++], stats);
        } else if (right == hi) {
            moveItem(&a[k], &buffer[left++], stats);
        } else if (greaterKey(buffer[left].key, buffer[right].key, stats)) {
            moveItem(&a[k], &buffer[right++], stats);
        } else {
            /* key가 같을 때 왼쪽 원소를 먼저 선택하여 안정성을 지킵니다. */
            moveItem(&a[k], &buffer[left++], stats);
        }
    }
}

int mergeSort(Item *a, size_t n, SortStats *stats)
{
    if (!startSort(a, n, stats)) return 0;
    if (n < 2) return 1;
    if (n > SIZE_MAX / sizeof(Item)) return 0;
    Item *buffer = malloc(n * sizeof(*buffer));
    if (buffer == NULL) return 0;
    if (stats != NULL) stats->heap_buffer_bytes = n * sizeof(*buffer);
    mergeRange(a, buffer, 0, n, stats);
    free(buffer);
    return 1;
}
