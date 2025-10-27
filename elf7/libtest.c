#include <stdio.h>
#include "libtest.h"

void func_a(void) {
    printf("  [动态库] func_a 被调用\n");
}

void func_b(void) {
    printf("  [动态库] func_b 被调用\n");
}

void func_c(void) {
    printf("  [动态库] func_c 被调用\n");
}

int calculate(int x, int y) {
    printf("  [动态库] calculate(%d, %d) 被调用\n", x, y);
    return x * y + x - y;
}

