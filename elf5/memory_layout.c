/*
 * 进程内存布局研究程序
 * 这个程序会持续运行，方便观察 /proc/<pid>/maps
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

// 不同内存区域的变量，用于观察内存布局

// 1. .text 段（代码段）
void function1() {
    printf("  [函数1] 地址: %p\n", (void*)function1);
}

void function2() {
    printf("  [函数2] 地址: %p\n", (void*)function2);
}

// 2. .data 段（已初始化的全局变量）
int initialized_global = 100;
char initialized_string[] = "Hello, World!";

// 3. .bss 段（未初始化的全局变量）
int uninitialized_global;
char uninitialized_array[1024];

// 4. .rodata 段（只读数据）
const char* readonly_string = "This is read-only";
const int readonly_value = 42;

int main() {
    // 输出进程ID
    pid_t pid = getpid();
    printf("========================================\n");
    printf("进程 PID: %d\n", pid);
    printf("========================================\n");
    printf("\n查看内存布局命令:\n");
    printf("  cat /proc/%d/maps\n", pid);
    printf("  cat /proc/%d/maps | grep -E 'heap|stack|memory_layout'\n", pid);
    printf("\n或在另一个终端运行:\n");
    printf("  ./show_maps.sh %d\n", pid);
    printf("========================================\n\n");

    // 5. 栈区（局部变量）
    int stack_var1 = 10;
    int stack_var2 = 20;
    char stack_array[100];

    // 6. 堆区（动态分配）
    int* heap_var1 = (int*)malloc(sizeof(int) * 10);
    int* heap_var2 = (int*)malloc(sizeof(int) * 100);
    char* heap_string = (char*)malloc(256);

    printf("【内存地址分布】\n\n");

    printf("1. 代码段 (.text):\n");
    printf("  [main函数] 地址: %p\n", (void*)main);
    function1();
    function2();
    printf("\n");

    printf("2. 只读数据段 (.rodata):\n");
    printf("  [字符串常量] 地址: %p, 值: \"%s\"\n", (void*)readonly_string, readonly_string);
    printf("  [只读整数]   地址: %p, 值: %d\n", (void*)&readonly_value, readonly_value);
    printf("\n");

    printf("3. 已初始化数据段 (.data):\n");
    printf("  [全局int]    地址: %p, 值: %d\n", (void*)&initialized_global, initialized_global);
    printf("  [全局字符串] 地址: %p, 值: \"%s\"\n", (void*)initialized_string, initialized_string);
    printf("\n");

    printf("4. 未初始化数据段 (.bss):\n");
    printf("  [全局int]    地址: %p, 值: %d\n", (void*)&uninitialized_global, uninitialized_global);
    printf("  [全局数组]   地址: %p\n", (void*)uninitialized_array);
    printf("\n");

    printf("5. 堆区 (heap) - 动态分配:\n");
    printf("  [malloc #1]  地址: %p\n", (void*)heap_var1);
    printf("  [malloc #2]  地址: %p\n", (void*)heap_var2);
    printf("  [malloc #3]  地址: %p\n", (void*)heap_string);
    printf("\n");

    printf("6. 栈区 (stack) - 局部变量:\n");
    printf("  [局部int #1] 地址: %p, 值: %d\n", (void*)&stack_var1, stack_var1);
    printf("  [局部int #2] 地址: %p, 值: %d\n", (void*)&stack_var2, stack_var2);
    printf("  [局部数组]   地址: %p\n", (void*)stack_array);
    printf("\n");

    printf("========================================\n");
    printf("程序将持续运行，按 Ctrl+C 退出\n");
    printf("========================================\n\n");

    // 持续运行，每10秒输出一次心跳
    int counter = 0;
    while (1) {
        printf("[心跳 #%d] PID: %d, 进程仍在运行... (在另一个终端查看 /proc/%d/maps)\n", 
               ++counter, pid, pid);
        sleep(10);
    }

    // 清理（实际不会执行到这里）
    free(heap_var1);
    free(heap_var2);
    free(heap_string);

    return 0;
}

