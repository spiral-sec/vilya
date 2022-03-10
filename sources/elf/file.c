
#include <elf.h>
#include <errno.h>
#include <fcntl.h>
#include <gelf.h>
#include <libelf.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "format_elf.h"
#include "vilya.h"

int open_file(file_t *result, input_t *settings)
{
    int fd = open(settings->target, O_RDONLY);
    Elf *elf = NULL;

    elf_version(EV_CURRENT);
    elf = elf_begin(fd, ELF_C_READ, NULL);
    if (!elf || elf_kind(elf) != ELF_K_ELF) {
        LOG_IF(settings->verbose, "Invalid file: %s", result->filename);
        return 0;
    } else if (!gelf_getehdr(elf, &result->ehdr)) {
        LOG("Could not read program header: %s", strerror(errno));
        return 0;
    }
    close(fd);
    elf_end(elf);
    return 1;
}

int write_to_outfile(file_t *result, input_t *settings)
{
    int fd = 0;

    if (unlink(result->filename) < 0) {
        LOG_IF(
        settings->verbose, "[!] unlinking %s failed: %s", result->filename, strerror(errno));
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
