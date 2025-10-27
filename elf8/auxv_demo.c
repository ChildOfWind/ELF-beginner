#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <sys/auxv.h>
#include <elf.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <link.h>

// 根据架构选择正确的类型
#if __WORDSIZE == 64
typedef Elf64_auxv_t auxv_t;
typedef Elf64_Phdr Phdr_t;
#else
typedef Elf32_auxv_t auxv_t;
typedef Elf32_Phdr Phdr_t;
#endif

// 辅助向量类型名称映射
const char* auxv_type_name(unsigned long type) {
    switch(type) {
        case AT_NULL:           return "AT_NULL";
        case AT_IGNORE:         return "AT_IGNORE";
        case AT_EXECFD:         return "AT_EXECFD";
        case AT_PHDR:           return "AT_PHDR";
        case AT_PHENT:          return "AT_PHENT";
        case AT_PHNUM:          return "AT_PHNUM";
        case AT_PAGESZ:         return "AT_PAGESZ";
        case AT_BASE:           return "AT_BASE";
        case AT_FLAGS:          return "AT_FLAGS";
        case AT_ENTRY:          return "AT_ENTRY";
        case AT_NOTELF:         return "AT_NOTELF";
        case AT_UID:            return "AT_UID";
        case AT_EUID:           return "AT_EUID";
        case AT_GID:            return "AT_GID";
        case AT_EGID:           return "AT_EGID";
        case AT_PLATFORM:       return "AT_PLATFORM";
        case AT_HWCAP:          return "AT_HWCAP";
        case AT_CLKTCK:         return "AT_CLKTCK";
        case AT_SECURE:         return "AT_SECURE";
        case AT_BASE_PLATFORM:  return "AT_BASE_PLATFORM";
        case AT_RANDOM:         return "AT_RANDOM";
        case AT_HWCAP2:         return "AT_HWCAP2";
        case AT_EXECFN:         return "AT_EXECFN";
        case AT_SYSINFO:        return "AT_SYSINFO";
        case AT_SYSINFO_EHDR:   return "AT_SYSINFO_EHDR";
        default:                return "UNKNOWN";
    }
}

// 辅助向量类型描述
const char* auxv_type_desc(unsigned long type) {
    switch(type) {
        case AT_NULL:           return "程序结束标记";
        case AT_IGNORE:         return "忽略的条目";
        case AT_EXECFD:         return "可执行文件的文件描述符";
        case AT_PHDR:           return "程序头表地址";
        case AT_PHENT:          return "程序头表条目大小";
        case AT_PHNUM:          return "程序头表条目数量";
        case AT_PAGESZ:         return "系统页大小";
        case AT_BASE:           return "解释器基地址";
        case AT_FLAGS:          return "标志";
        case AT_ENTRY:          return "程序入口点地址";
        case AT_NOTELF:         return "程序不是ELF格式";
        case AT_UID:            return "真实用户ID";
        case AT_EUID:           return "有效用户ID";
        case AT_GID:            return "真实组ID";
        case AT_EGID:           return "有效组ID";
        case AT_PLATFORM:       return "平台字符串";
        case AT_HWCAP:          return "硬件能力位";
        case AT_CLKTCK:         return "时钟频率";
        case AT_SECURE:         return "是否安全模式";
        case AT_BASE_PLATFORM:  return "基础平台字符串";
        case AT_RANDOM:         return "16字节随机数地址";
        case AT_HWCAP2:         return "硬件能力位(扩展)";
        case AT_EXECFN:         return "可执行文件名";
        case AT_SYSINFO:        return "系统调用入口地址";
        case AT_SYSINFO_EHDR:   return "vDSO地址";
        default:                return "未知类型";
    }
}

// 方法1: 通过 /proc/self/auxv 读取
void read_auxv_from_proc() {
    printf("\n========================================\n");
    printf("方法1: 从 /proc/self/auxv 读取\n");
    printf("========================================\n");
    
    int fd = open("/proc/self/auxv", O_RDONLY);
    if (fd < 0) {
        perror("打开 /proc/self/auxv 失败");
        return;
    }
    
    auxv_t auxv;
    int count = 0;
    
    while (read(fd, &auxv, sizeof(auxv)) == sizeof(auxv)) {
        if (auxv.a_type == AT_NULL) {
            printf("[%2d] 类型: %-20s (%-2lu) 值: 0x%016lx  | %s\n",
                   count, auxv_type_name(auxv.a_type), auxv.a_type,
                   auxv.a_un.a_val, auxv_type_desc(auxv.a_type));
            break;
        }
        
        printf("[%2d] 类型: %-20s (%-2lu) 值: 0x%016lx  | %s\n",
               count, auxv_type_name(auxv.a_type), auxv.a_type,
               auxv.a_un.a_val, auxv_type_desc(auxv.a_type));
        
        // 显示字符串类型的详细信息
        if (auxv.a_type == AT_PLATFORM || auxv.a_type == AT_EXECFN || 
            auxv.a_type == AT_BASE_PLATFORM) {
            printf("     └─> 字符串内容: \"%s\"\n", (char*)auxv.a_un.a_val);
        }
        
        // 显示随机数
        if (auxv.a_type == AT_RANDOM) {
            unsigned char *random = (unsigned char*)auxv.a_un.a_val;
            printf("     └─> 随机数据: ");
            for (int i = 0; i < 16; i++) {
                printf("%02x ", random[i]);
            }
            printf("\n");
        }
        
        count++;
    }
    
    close(fd);
}

// 方法2: 使用 getauxval() 获取特定值
void read_auxv_with_getauxval() {
    printf("\n========================================\n");
    printf("方法2: 使用 getauxval() 获取关键值\n");
    printf("========================================\n");
    
    unsigned long types[] = {
        AT_PHDR, AT_PHENT, AT_PHNUM, AT_PAGESZ, AT_BASE,
        AT_ENTRY, AT_UID, AT_EUID, AT_GID, AT_EGID,
        AT_PLATFORM, AT_HWCAP, AT_CLKTCK, AT_RANDOM,
        AT_EXECFN, AT_SYSINFO_EHDR, AT_SECURE
    };
    
    int num_types = sizeof(types) / sizeof(types[0]);
    
    for (int i = 0; i < num_types; i++) {
        unsigned long val = getauxval(types[i]);
        if (val != 0 || types[i] == AT_SECURE) {
            printf("%-20s = 0x%016lx", auxv_type_name(types[i]), val);
            
            if (types[i] == AT_PAGESZ) {
                printf("  (%lu bytes)", val);
            } else if (types[i] == AT_CLKTCK) {
                printf("  (%lu Hz)", val);
            } else if (types[i] == AT_PLATFORM || types[i] == AT_EXECFN) {
                printf("  (\"%s\")", (char*)val);
            }
            
            printf("\n");
        }
    }
}

// 方法3: 直接从栈上访问 auxv (通过main参数)
void read_auxv_from_stack(char **envp) {
    printf("\n========================================\n");
    printf("方法3: 从栈上直接访问 auxv\n");
    printf("========================================\n");
    
    // 找到环境变量的结尾（NULL）
    char **env = envp;
    while (*env != NULL) {
        env++;
    }
    env++; // 跳过 NULL
    
    // 现在 env 指向 auxv 数组
    auxv_t *auxv = (auxv_t *)env;
    
    printf("auxv 在栈上的地址: %p\n", (void*)auxv);
    printf("每个 auxv_t 结构大小: %lu 字节\n\n", sizeof(auxv_t));
    
    int count = 0;
    while (auxv[count].a_type != AT_NULL) {
        printf("[%2d] 地址: %p | 类型: %-20s (%-2lu) | 值: 0x%016lx\n",
               count, (void*)&auxv[count],
               auxv_type_name(auxv[count].a_type),
               auxv[count].a_type,
               auxv[count].a_un.a_val);
        count++;
    }
    
    // 打印 AT_NULL
    printf("[%2d] 地址: %p | 类型: %-20s (%-2lu) | 值: 0x%016lx\n",
           count, (void*)&auxv[count],
           auxv_type_name(auxv[count].a_type),
           auxv[count].a_type,
           auxv[count].a_un.a_val);
}

// 显示栈布局信息
void show_stack_layout(int argc, char **argv, char **envp) {
    printf("\n========================================\n");
    printf("栈内存布局分析\n");
    printf("========================================\n");
    
    printf("argc 地址:        %p  值: %d\n", (void*)&argc, argc);
    printf("argv 地址:        %p  值: %p\n", (void*)&argv, (void*)argv);
    printf("argv[0] 地址:     %p  值: %p (\"%s\")\n", 
           (void*)&argv[0], (void*)argv[0], argv[0]);
    
    // 打印所有参数
    for (int i = 0; i < argc; i++) {
        printf("  argv[%d]:        %p -> \"%s\"\n", i, (void*)argv[i], argv[i]);
    }
    printf("  argv[%d]:        %p (NULL)\n", argc, (void*)argv[argc]);
    
    printf("\nenvp 地址:        %p  值: %p\n", (void*)&envp, (void*)envp);
    
    // 打印部分环境变量
    int env_count = 0;
    for (char **env = envp; *env != NULL && env_count < 5; env++, env_count++) {
        printf("  envp[%d]:       %p -> \"%s\"\n", env_count, (void*)*env, *env);
    }
    if (envp[env_count] != NULL) {
        printf("  ... (更多环境变量) ...\n");
    }
    
    // 找到环境变量结尾
    char **env = envp;
    while (*env != NULL) {
        env++;
        env_count++;
    }
    printf("  envp[%d]:       %p (NULL)\n", env_count, (void*)*env);
    
    env++; // 指向 auxv
    printf("\nauxv 开始地址:    %p\n", (void*)env);
    
    // 计算距离
    long distance = (char*)env - (char*)&argc;
    printf("\nauxv 距离 argc:   %ld 字节\n", distance);
}

// 分析程序头表
void analyze_program_headers() {
    printf("\n========================================\n");
    printf("程序头表分析 (通过 auxv 获取)\n");
    printf("========================================\n");
    
    Phdr_t *phdr = (Phdr_t *)getauxval(AT_PHDR);
    unsigned long phent = getauxval(AT_PHENT);
    unsigned long phnum = getauxval(AT_PHNUM);
    
    printf("程序头表地址:     %p\n", (void*)phdr);
    printf("程序头条目大小:   %lu 字节\n", phent);
    printf("程序头条目数量:   %lu\n\n", phnum);
    
    for (unsigned long i = 0; i < phnum; i++) {
        printf("[%2lu] 类型: ", i);
        switch(phdr[i].p_type) {
            case PT_NULL:       printf("PT_NULL      "); break;
            case PT_LOAD:       printf("PT_LOAD      "); break;
            case PT_DYNAMIC:    printf("PT_DYNAMIC   "); break;
            case PT_INTERP:     printf("PT_INTERP    "); break;
            case PT_NOTE:       printf("PT_NOTE      "); break;
            case PT_SHLIB:      printf("PT_SHLIB     "); break;
            case PT_PHDR:       printf("PT_PHDR      "); break;
            case PT_TLS:        printf("PT_TLS       "); break;
            case PT_GNU_EH_FRAME: printf("PT_GNU_EH_FRAME"); break;
            case PT_GNU_STACK:  printf("PT_GNU_STACK "); break;
            case PT_GNU_RELRO:  printf("PT_GNU_RELRO "); break;
            default:            printf("0x%08x   ", phdr[i].p_type); break;
        }
        printf(" | VirtAddr: 0x%016lx | FileSize: 0x%08lx | MemSize: 0x%08lx\n",
               phdr[i].p_vaddr, phdr[i].p_filesz, phdr[i].p_memsz);
    }
}

int main(int argc, char **argv, char **envp) {
    printf("╔════════════════════════════════════════════════════════════════╗\n");
    printf("║          ELF 辅助向量 (Auxiliary Vector) 研究程序              ║\n");
    printf("╚════════════════════════════════════════════════════════════════╝\n");
    
    // 显示基本信息
    printf("\nPID: %d\n", getpid());
    printf("可执行文件: %s\n", argv[0]);
    printf("架构位数: %d-bit\n", (int)(sizeof(void*) * 8));
    printf("auxv_t 结构大小: %lu 字节\n", sizeof(auxv_t));
    
    // 方法1: 从 /proc/self/auxv 读取
    read_auxv_from_proc();
    
    // 方法2: 使用 getauxval()
    read_auxv_with_getauxval();
    
    // 方法3: 从栈上访问
    read_auxv_from_stack(envp);
    
    // 显示栈布局
    show_stack_layout(argc, argv, envp);
    
    // 分析程序头表
    analyze_program_headers();
    
    return 0;
}

