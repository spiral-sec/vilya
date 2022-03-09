
#ifndef VILYA_H
#define VILYA_H

#include <elf.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>

/* TODO: support for i386, arm64, arm_eabi_thumb, arm_eabi */
typedef enum target_arch { X86_64, X86_64_X32, UNKNOWN } architecture_t;

#ifndef DEFAULT_BUFFER_SIZE
#define DEFAULT_BUFFER_SIZE (512)
#endif /* DEFAULT_BUFFER_SIZE */

#ifdef __LP64__
#define Elf_Ehdr Elf64_Ehdr
#define Elf_Shdr Elf64_Shdr
#define Elf_Sym Elf64_Sym
#else
#define Elf_Ehdr Elf32_Ehdr
#define Elf_Shdr Elf32_Shdr
#define Elf_Sym Elf32_Sym
#endif

#ifndef ELF_HEADER
#define ELF_HEADER ("\177ELF")
#endif /* ELF_HEADER */

typedef struct target_file_s {
    char filename[DEFAULT_BUFFER_SIZE];

    unsigned char *content;
    size_t content_len;
    architecture_t arch;

    Elf_Ehdr *header;
    void *end_ptr;
} file_t;

typedef struct user_input_s {
    int verbose : 1;
    char target[DEFAULT_BUFFER_SIZE];
    architecture_t requested;
} input_t;

// lexer.c
int has_valid_input(int, char *[], input_t *);

#ifndef LOG
#define LOG(f_, ...) dprintf(2, "[%s] ", timestamp()), printf((f_), ##__VA_ARGS__), printf("\n")
#endif /* LOG */

#ifndef LOG_IF
#define LOG_IF(enabled, f_, ...)                                                                  \
    if (enabled) {                                                                                \
        dprintf(2, "[%s] ", timestamp()), printf((f_), ##__VA_ARGS__), printf("\n");              \
    } else {                                                                                      \
    }
#endif /* LOG_IF_VERBOSE */

// logs.c
char *timestamp(void);

// ELF/
// file.c
int is_valid_elf(file_t *result, input_t *settings);

#endif /* VILYA_H */
