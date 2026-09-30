#include "data.h"
#include <string.h>

const char *const PATTERNS[6] = {
    "random", "sorted", "reverse", "nearly_sorted", "few_unique", "all_equal"
};

/* 환경이 바뀌어도 같은 입력을 만드는 32비트 LCG. 암호용 난수가 아닙니다. */
uint32_t nextRandom(uint32_t *state)
{
    *state = *state * UINT32_C(1664525) + UINT32_C(1013904223);
    return *state;
}

void makeInput(Item *a, size_t n, int pattern, uint32_t seed)
{
    for (size_t i = 0; i < n; ++i) {
        int key;
        switch (pattern) {
        case 1: key = (int)i; break;
        case 2: key = (int)(n - i); break;
        case 3: key = (int)i; break;
        case 4: key = (int)((nextRandom(&seed) >> 16) % 16); break;
        case 5: key = 7; break;
        default: key = (int)(nextRandom(&seed) % UINT32_C(1000001)); break;
        }
        a[i] = (Item){key, (uint32_t)i};
    }
    if (pattern == 3 && n > 1) {
        /* n/100번 인접한 두 key를 교환. 최소 n=1000이면 10회입니다. */
        for (size_t k = 0; k < n / 100; ++k) {
            size_t i = nextRandom(&seed) % (n - 1);
            int tmp = a[i].key;
            a[i].key = a[i + 1].key;
            a[i + 1].key = tmp;
        }
    }
    /* original_index는 생성이 완료된 입력의 위치입니다. */
}

int compareItems(const void *x, const void *y)
{
    const Item *a = x, *b = y;
    if (a->key != b->key) return (a->key > b->key) - (a->key < b->key);
    return (a->original_index > b->original_index) -
           (a->original_index < b->original_index);
}

int isStable(const Item *a, size_t n)
{
    for (size_t i = 1; i < n; ++i)
        if (a[i - 1].key == a[i].key &&
            a[i - 1].original_index > a[i].original_index) return 0;
    return 1;
}

int verifyResult(const Item *a, const Item *input, const Item *expected,
                 size_t n, unsigned char *seen)
{
    if (n == 0) return 1;
    memset(seen, 0, n);
    for (size_t i = 0; i < n; ++i) {
        const size_t id = a[i].original_index;
        if (a[i].key != expected[i].key || id >= n || seen[id] ||
            input[id].key != a[i].key) return 0;
        seen[id] = 1;
    }
    return 1;
}
