#include <stdio.h>
#include "mylib.h"

// 动态库中的全局变量
int shared_variable = 42;

// 简单的加法函数
int add(int a, int b) {
    printf("[动态库] add函数被调用: %d + %d\n", a, b);
    return a + b;
}

// 简单的乘法函数
int multiply(int a, int b) {
    printf("[动态库] multiply函数被调用: %d * %d\n", a, b);
    return a * b;
}

// 打印消息函数
void print_message(const char* msg) {
    printf("[动态库] 消息: %s\n", msg);
}

