
#ifndef VILYA_H
#define VILYA_H

#include <elf.h>
#include <fcntl.h>
#include <gelf.h>
#include <libelf.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>

#ifndef DEFAULT_BUFFER_SIZE
#define DEFAULT_BUFFER_SIZE (512)
#endif /* DEFAULT_BUFFER_SIZE */

typedef unsigned char byte;

typedef struct target_file_s {
    char filename[DEFAULT_BUFFER_SIZE];
    GElf_Ehdr ehdr;
    GElf_Shdr *symbols;
    byte *section_names;
    GElf_Addr original_entry_point;

    byte *content;
    size_t content_size;
} file_t;

typedef struct user_input_s {
    int verbose : 1;
    char filepath[DEFAULT_BUFFER_SIZE];
} input_t;

// lexer.c
int has_valid_input(int, char *[], input_t *);

#ifndef LOG
#define LOG(f_, ...) dprintf(2, "[%s] ", timestamp()), printf((f_), ##__VA_ARGS__), printf("\n")
#endif /* LOG */

#ifndef LOG_IF
#define LOG_IF(predicate, f_, ...)                                                                \
    do                                                                                            \
        if (predicate) {                                                                          \
            dprintf(2, "[%s] ", timestamp()), printf((f_), ##__VA_ARGS__), printf("\n");          \
        }                                                                                         \
    while (0);
#endif /* LOG_IF */

// logs.c
char *timestamp(void);

#endif /* VILYA_H */
