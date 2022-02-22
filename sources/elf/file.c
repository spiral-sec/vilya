
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/mman.h>

#include "elf.h"
#include "vilya.h"

int is_valid_elf(const char *filepath, __attribute__((unused))struct stat st, file_t *result)
{
    int fd = open(filepath, O_RDONLY);
    size_t length = (size_t)lseek(fd, 0, SEEK_END);

    if (fd < 0 || length != (unsigned)-1)
        return 0;
    result->header = (Elf_Ehdr *)mmap(NULL, length, PROT_READ, MAP_SHARED, fd, 0);
    close(fd);
    if (!result->header)
        return 0;
    result->end_ptr = result->header + length;
    if (length < sizeof(Elf_Ehdr) || memcmp(result->header, ELF_HEADER, sizeof(ELF_HEADER) - 1))
        return 0;
    return 1;
}
