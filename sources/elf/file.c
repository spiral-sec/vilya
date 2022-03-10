
#include <elf.h>
#include <errno.h>
#include <fcntl.h>
#include <gelf.h>
#include <libelf.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "format_elf.h"
#include "vilya.h"

static int read_elf_content(int fd, file_t *file_handler)
{
    file_handler->byte_count = (size_t)lseek(fd, 0, SEEK_END);
    file_handler->bytes = malloc(sizeof(char) * file_handler->byte_count);
    lseek(fd, 0, SEEK_SET);
    if (!file_handler->bytes || read(fd, file_handler->bytes, file_handler->byte_count) < 0)
        return 0;
    return 1;
}

int read_elf(file_t *file_handler, input_t *settings)
{
    int fd = open(settings->filepath, O_RDONLY);
    Elf *elf = NULL;

    elf_version(EV_CURRENT);
    elf = (fd < 0) ? NULL : elf_begin(fd, ELF_C_READ, NULL);
    if (!elf || elf_kind(elf) != ELF_K_ELF) {
        LOG_IF(settings->verbose, "Invalid file: %s", file_handler->filename);
        return 0;
    } else if (!gelf_getehdr(elf, &file_handler->ehdr) || !read_elf_content(fd, file_handler))
        return 0;
    file_handler->original_entry_point = file_handler->ehdr.e_entry;
    close(fd);
    elf_end(elf);
    return 1;
}

int write_to_outfile(file_t *result, input_t *settings)
{
    int fd = 0;

    if (unlink(result->filename) < 0) {
        LOG_IF(settings->verbose, "[!] unlink failed: %s", strerror(errno));
        return 0;
    }
    fd = open(result->filename, O_CREAT | O_TRUNC | O_RDWR, S_IRWXU);
    if (fd == -1) {
        LOG_IF(settings->verbose, "[!] Could not create file descriptor for %s: %s",
        result->filename, strerror(errno));
        return 0;
    }
    close(fd);
    return 1;
}
