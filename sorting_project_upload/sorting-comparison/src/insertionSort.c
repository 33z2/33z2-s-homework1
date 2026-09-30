#include "sort.h"

int insertionSort(Item *a, size_t n, SortStats *stats)
{
    if (!startSort(a, n, stats)) return 0;
    for (size_t i = 1; i < n; ++i) {
        Item current;
        moveItem(&current, &a[i], stats);
        size_t j = i;
        /* 같은 key는 이동시키지 않으므로 기존 순서가 유지됩니다. */
        while (j > 0 && greaterKey(a[j - 1].key, current.key, stats)) {
            moveItem(&a[j], &a[j - 1], stats);
            --j;
        }
        moveItem(&a[j], &current, stats);
    }
    return 1;
}
