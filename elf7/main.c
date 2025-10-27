#include <stdio.h>
#include "libtest.h"

int main() {
    printf("程序开始执行\n\n");
    
    printf("调用 func_a:\n");
    func_a();
    
    printf("\n调用 func_b:\n");
    func_b();
    
    printf("\n调用 calculate:\n");
    int result = calculate(10, 5);
    printf("  结果: %d\n", result);
    
    printf("\n再次调用 func_a:\n");
    func_a();
    
    printf("\n程序结束\n");
    return 0;
}

