
#include <elf.h>
#include <errno.h>
#include <fcntl.h>
#include <gelf.h>
#include <libelf.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include "utils.h"
#include "vilya.h"

static void gen_filename(char const *src, char result[DEFAULT_BUFFER_SIZE])
{
    char const *extension = STUB_SECTION;
    size_t length = strlen(src) + strlen(extension);

    if (length > DEFAULT_BUFFER_SIZE) {
        strncpy(result, "file.vilya", 11);
    } else {
        strncpy(result, src, strlen(src));
        strncat(result, extension, strlen(extension));
    }
}

static int read_elf(input_t *settings, file_t *file)
{
    int fd = open(settings->filepath, O_RDONLY);
    Elf *elf_obj = NULL;
    GElf_Ehdr ehdr = {0};

    elf_version(EV_CURRENT);
    if (fd < 0)
        return 0;
    elf_obj = elf_begin(fd, ELF_C_READ, NULL);
    if (!elf_obj || elf_kind(elf_obj) != ELF_K_ELF) {
        RZERO_IF(settings->verbose, "[!] %s is not an ELF file", settings->filepath);
    } else if (!gelf_getehdr(elf_obj, &ehdr))
        RZERO_IF(settings->verbose, "[!] could not load elf header");
    file->original_entry_point = ehdr.e_entry;
    LOG_IF(settings->verbose, "[+] Successfully loaded ELF file");
    LOG_IF(settings->verbose, "\tEntry point: %ld", file->original_entry_point);
    return 1;
}

int parse(input_t *settings, file_t *file)
{
    int fd = open(settings->filepath, O_RDONLY);
    ssize_t read_status = -1;
    struct stat st = {0};

    if (fd == -1 || stat(settings->filepath, &st) < 0)
        RZERO_IF(
        settings->verbose, "[!] could not open %s: %s", settings->filepath, strerror(errno));
    file->binary_dump_size = (size_t)st.st_size;
    if ((ssize_t)file->binary_dump_size <= 0)
        RZERO_IF(settings->verbose, "[!] filesize is under zero");
    file->binary_dump = malloc(sizeof(uint8_t) * file->binary_dump_size);
    if (!file->binary_dump)
        RZERO_IF(settings->verbose, "[!] could not allocate %ld bytes: %s", file->binary_dump_size,
        strerror(errno));
    read_status = read(fd, file->binary_dump, file->binary_dump_size);
    if (read_status < (ssize_t)file->binary_dump_size)
        RZERO_IF(settings->verbose, "[!] only read %ld bytes out of %ld", read_status,
        file->binary_dump_size);
    close(fd);
    gen_filename(settings->filepath, file->filename);
    LOG_IF(settings->verbose, "[+] loaded %s", settings->filepath);
    return read_elf(settings, file);
}
