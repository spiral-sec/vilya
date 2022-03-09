
#include <fcntl.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "elf.h"
#include "vilya.h"

int is_valid_elf(file_t *result, input_t *settings)
{
    int fd = open(settings->target, O_RDONLY);
    size_t length = (size_t)lseek(fd, 0, SEEK_END);
    struct stat st;

    result->arch = settings->requested;
    if (stat(settings->target, &st) < 0 || fd < 0 || length == (unsigned)-1)
        return 0;
    strncpy((char *)result->filename, settings->target, DEFAULT_BUFFER_SIZE);
    result->header = (Elf_Ehdr *)mmap(NULL, length, PROT_READ, MAP_SHARED, fd, 0);
    close(fd);
    if (!result->header)
        return 0;
    result->end_ptr = result->header + length;
    if (length < sizeof(Elf_Ehdr) || memcmp(result->header, ELF_HEADER, sizeof(ELF_HEADER) - 1))
        return 0;
    return 1;
}
