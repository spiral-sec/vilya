
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "elf.h"
#include "vilya.h"

int is_valid_elf(const char *filepath, struct stat st, file_t *result)
{
    int fd = open(filepath, O_RDONLY);
    size_t length = 0;

    return 0;
}
