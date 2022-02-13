
#ifndef VILYA_H
#define VILYA_H

#include <elf.h>
#include <stdio.h>
#include <sys/types.h>

typedef enum target_arch { UNKNOWN, ELF } architecture_t;

#ifndef DEFAULT_BUFFER_SIZE
#define DEFAULT_BUFFER_SIZE (512)
#endif /* DEFAULT_BUFFER_SIZE */

typedef struct target_file_s {
    const char filename[DEFAULT_BUFFER_SIZE];

    unsigned char *content;
    size_t content_len;
    architecture_t arch;
} file_t;

typedef struct user_input_s {
    int verbose : 1;
} input_t;

// lexer.c
int has_valid_input(int, char *[], file_t *, input_t *);

#ifndef LOG
#define LOG(f_, ...) dprintf(2, "[%s] ", timestamp()), printf((f_), ##__VA_ARGS__), printf("\n")
#endif /* LOG */

#ifndef LOG_IF_VERBOSE
#define LOG_IF_VERBOSE(enabled, f_, ...)                                                          \
    if (enabled) {                                                                                \
        dprintf(2, "[%s] ", timestamp()), printf((f_), ##__VA_ARGS__), printf("\n");              \
    }
#endif /* LOG_IF_VERBOSE */

// logs.c
char *timestamp(void);

#endif /* VILYA_H */
