#include "sort.h"

/* [0, count)는 힙 구간입니다. root에서 아래로 내려가며 최대 힙을 복구합니다. */
static void siftDown(Item *a, size_t root, size_t count, SortStats *stats)
{
    /* root<count/2이면 왼쪽 자식이 존재합니다. 곱셈 전 검사로 넘침도 피합니다. */
    while (root < count / 2) {
        size_t child = 2 * root + 1;
        if (child + 1 < count &&
            greaterKey(a[child + 1].key, a[child].key, stats))
            ++child;
        if (!greaterKey(a[child].key, a[root].key, stats)) break;
        swapItems(&a[root], &a[child], stats);
        root = child;
    }
}

int heapSort(Item *a, size_t n, SortStats *stats)
{
    if (!startSort(a, n, stats)) return 0;
    if (n < 2) return 1;

    /* 마지막 부모부터 루트까지: 아래에서 위로 최대 힙 구성. */
    for (size_t i = n / 2; i > 0; --i)
        siftDown(a, i - 1, n, stats);

    /* 최대값을 맨 뒤로 보내고 힙의 크기를 하나씩 줄입니다. */
    for (size_t end = n - 1; end > 0; --end) {
        swapItems(&a[0], &a[end], stats);
        siftDown(a, 0, end, stats);
    }
    return 1;
}
