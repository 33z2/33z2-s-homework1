#include "sort.h"
const SortAlgorithm ALGORITHMS[3] = {
    {"insertion", insertionSort, 1},
    {"merge", mergeSort, 1},
    {"heap", heapSort, 0}
};
