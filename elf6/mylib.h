#ifndef MYLIB_H
#define MYLIB_H

// 动态库提供的全局变量
extern int shared_variable;

// 动态库提供的函数
int add(int a, int b);
int multiply(int a, int b);
void print_message(const char* msg);

#endif

