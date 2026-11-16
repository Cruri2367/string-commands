#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>

void *memset_asm(void *, int, size_t);
void *memcpy_asm(void *, const void *, size_t);

#define SIZE ((size_t)100 * 1024 * 1024)
#define RUNS 10

typedef void *(*set_fn)(void *, int, size_t);
typedef void *(*copy_fn)(void *, const void *, size_t);

static double elapsed_ms(LARGE_INTEGER begin, LARGE_INTEGER end,
                         LARGE_INTEGER frequency) {
    return (double)(end.QuadPart - begin.QuadPart) * 1000.0 /
           (double)frequency.QuadPart;
}

static void benchmark_set(const char *name, set_fn fn, unsigned char *dst,
                          LARGE_INTEGER frequency) {
    double total = 0.0;
    double times[RUNS];

    for (int i = 0; i < RUNS; ++i) {
        LARGE_INTEGER begin, end;
        int value = (i & 1) ? 0x55 : 0xAA;

        QueryPerformanceCounter(&begin);
        fn(dst, value, SIZE);
        QueryPerformanceCounter(&end);

        times[i] = elapsed_ms(begin, end, frequency);
        total += times[i];
    }

    printf("\n%s\n", name);

    for (int i = 0; i < RUNS; ++i)
        printf("  run %2d: %9.3f ms\n", i + 1, times[i]);

    printf("  average: %9.3f ms\n", total / RUNS);
}

static void benchmark_copy(const char *name, copy_fn fn, unsigned char *dst,
                           const unsigned char *src,
                           LARGE_INTEGER frequency) {
    double total = 0.0;
    double times[RUNS];

    for (int i = 0; i < RUNS; ++i) {
        LARGE_INTEGER begin, end;

        QueryPerformanceCounter(&begin);
        fn(dst, src, SIZE);
        QueryPerformanceCounter(&end);

        times[i] = elapsed_ms(begin, end, frequency);
        total += times[i];
    }

    printf("\n%s\n", name);

    for (int i = 0; i < RUNS; ++i)
        printf("  run %2d: %9.3f ms\n", i + 1, times[i]);

    printf("  average: %9.3f ms\n", total / RUNS);
}

static int check_memset(const unsigned char *buffer,
                        unsigned char expected) {
    for (size_t i = 0; i < SIZE; ++i) {
        if (buffer[i] != expected)
            return 0;
    }

    return 1;
}

int main(void) {
    unsigned char *src = (unsigned char *)malloc(SIZE);
    unsigned char *dst = (unsigned char *)malloc(SIZE);
    LARGE_INTEGER frequency;

    if (!src || !dst) {
        puts("Could not allocate two 100 MiB buffers.");
        free(src);
        free(dst);
        return 1;
    }

    for (size_t i = 0; i < SIZE; ++i)
        src[i] = (unsigned char)(i * 17u + 3u);

    QueryPerformanceFrequency(&frequency);

    puts("Buffer: 100 MiB; 10 runs per function");
    puts("Timer: Windows QueryPerformanceCounter");

    puts("\n========== MEMSET ==========");

    benchmark_set("memset_asm", memset_asm, dst, frequency);

    printf("  memset_asm check: %s\n",
           check_memset(dst, 0x55) ? "OK" : "ERROR");

    benchmark_set("libc memset", memset, dst, frequency);

    printf("  libc memset check: %s\n",
           check_memset(dst, 0x55) ? "OK" : "ERROR");

    puts("\n========== MEMCPY ==========");

    benchmark_copy("memcpy_asm", memcpy_asm, dst, src, frequency);

    printf("  memcpy_asm check: ");
    puts(memcmp(dst, src, SIZE) == 0 ? "OK" : "ERROR");

    benchmark_copy("libc memcpy", memcpy, dst, src, frequency);

    printf("  libc memcpy check: ");
    puts(memcmp(dst, src, SIZE) == 0 ? "OK" : "ERROR");

    free(src);
    free(dst);

    return 0;
}
