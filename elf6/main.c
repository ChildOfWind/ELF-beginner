#include <stdio.h>
#include "mylib.h"

int main() {
    printf("=== 动态链接演示程序 ===\n\n");
    
    // 1. 访问动态库中的全局变量
    printf("1. 访问共享变量:\n");
    printf("   shared_variable 的值: %d\n", shared_variable);
    printf("   shared_variable 的地址: %p\n\n", (void*)&shared_variable);
    
    // 2. 第一次调用动态库函数 (触发延迟绑定)
    printf("2. 第一次调用 add() - 触发延迟绑定:\n");
    int result1 = add(10, 20);
    printf("   结果: %d\n\n", result1);
    
    // 3. 第二次调用同一函数 (已经绑定，直接跳转)
    printf("3. 第二次调用 add() - 直接跳转:\n");
    int result2 = add(30, 40);
    printf("   结果: %d\n\n", result2);
    
    // 4. 调用另一个函数 (再次触发延迟绑定)
    printf("4. 第一次调用 multiply() - 触发延迟绑定:\n");
    int result3 = multiply(5, 6);
    printf("   结果: %d\n\n", result3);
    
    // 5. 调用第三个函数
    printf("5. 调用 print_message():\n");
    print_message("Hello from dynamic linking!");
    
    // 6. 修改共享变量
    printf("\n6. 修改共享变量:\n");
    shared_variable = 100;
    printf("   新值: %d\n", shared_variable);
    
    printf("\n=== 程序结束 ===\n");
    return 0;
}

