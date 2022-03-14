
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

static int read_elf_content(int fd, file_t *file)
{
    file->content_size = (size_t)lseek(fd, 0, SEEK_END);
    file->content =
    mmap((void *)0, file->content_size, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0);
    return file->content != NULL;
}

int read_elf(file_t *file, input_t *settings)
{
    int fd = open(settings->filepath, O_RDONLY);
    Elf *elf = NULL;
    GElf_Shdr *shdr = NULL;

    elf_version(EV_CURRENT);
    elf = (fd < 0) ? NULL : elf_begin(fd, ELF_C_READ, NULL);
    if (!elf || elf_kind(elf) != ELF_K_ELF) {
        LOG_IF(settings->verbose, "Invalid file: %s", file->filename);
        return 0;
    } else if (!gelf_getehdr(elf, &file->ehdr) || !read_elf_content(fd, file))
        return 0;
    file->original_entry_point = file->ehdr.e_entry;
    elf_end(elf);
    close(fd);
    shdr = (GElf_Shdr *)file->content + file->ehdr.e_shoff;
    file->symbols = &shdr[file->ehdr.e_shstrndx];
    file->section_names = (byte *)file->content + file->symbols->sh_offset;
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
