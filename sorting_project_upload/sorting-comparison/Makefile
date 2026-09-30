CC = gcc
CFLAGS = -std=c17 -O2 -Wall -Wextra -Wpedantic -Werror
CPPFLAGS = -Isrc
PYTHON = python3
COMMON = src/sort.c src/insertionSort.c src/mergeSort.c src/heapSort.c src/data.c
HEADERS = src/sort.h src/data.h

.PHONY: all run test bench sanitize clean
all: run
build:
	mkdir -p build
build/benchmark.out: src/main.c $(COMMON) $(HEADERS) | build
	$(CC) $(CFLAGS) $(CPPFLAGS) -DSORT_COUNTING=0 src/main.c $(COMMON) -o $@
build/counts.out: src/main.c $(COMMON) $(HEADERS) | build
	$(CC) $(CFLAGS) $(CPPFLAGS) -DSORT_COUNTING=1 src/main.c $(COMMON) -o $@
build/test_sort.out: tests/test_sort.c $(COMMON) $(HEADERS) | build
	$(CC) $(CFLAGS) $(CPPFLAGS) -DSORT_COUNTING=1 tests/test_sort.c $(COMMON) -o $@
build/test_fast.out: tests/test_sort.c $(COMMON) $(HEADERS) | build
	$(CC) $(CFLAGS) $(CPPFLAGS) -DSORT_COUNTING=0 tests/test_sort.c $(COMMON) -o $@
run: build/benchmark.out
	./build/benchmark.out --demo
test: build/test_sort.out build/test_fast.out
	./build/test_sort.out
	./build/test_fast.out
bench: build/benchmark.out build/counts.out
	mkdir -p results
	./build/benchmark.out --time > results/raw_timings.csv
	./build/counts.out --counts > results/counts.csv
	$(PYTHON) tools/summarize.py
	$(PYTHON) tools/plot_svg.py
	$(PYTHON) tools/environment.py > results/environment.txt
sanitize: | build
	$(CC) -std=c17 -O1 -g -Wall -Wextra -Wpedantic -Werror $(CPPFLAGS) \
		-fsanitize=address,undefined -fno-omit-frame-pointer -DSORT_COUNTING=1 \
		tests/test_sort.c $(COMMON) -o build/test_sanitize.out
	ASAN_OPTIONS=detect_leaks=1 ./build/test_sanitize.out
clean:
	rm -rf build
