
#ifndef VILYA_UTILS_H
#define VILYA_UTILS_H

#include "vilya.h"

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

#ifndef RZERO_IF
#define RZERO_IF(predicate, f_, ...)                                                              \
    do {                                                                                          \
        if (predicate) {                                                                          \
            dprintf(2, "[%s] ", timestamp()), printf((f_), ##__VA_ARGS__), printf("\n");          \
            return (0);                                                                           \
        }                                                                                         \
    } while (0);
#endif /* RZERO_IF */

// logs.c
char *timestamp(void);

// lexer.c
int has_valid_input(int, char *[], input_t *);

#endif
