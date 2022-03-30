
#include <gelf.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include "utils.h"
#include "vilya.h"

IN_SECTION(STUB) static char *get_program_name(void)
{
    char *result = malloc(sizeof(char) * DEFAULT_BUFFER_SIZE);

    if (!result || !readlink("/proc/self/exe", result, DEFAULT_BUFFER_SIZE))
        return NULL;
    return result;
}

IN_SECTION(STUB) void xor_bytes(uint8_t *to_xor, size_t size, uint32_t hash)
{
    for (size_t ctr = 0; ctr < size; ctr++)
        to_xor[ctr] ^= hash;
}

/* https://stackoverflow.com/a/7666577 */
IN_SECTION(STUB) uint32_t hash_bytes(uint8_t *bytes, size_t size)
{
    uint32_t hash = 0;

    for (size_t ctr = 0; ctr < size; ctr++)
        hash = bytes[ctr] + (hash << 6) + (hash << 16) - hash;
    return hash;
}

IN_SECTION(STUB) uint8_t *read_current_program(size_t *size)
{
    char *prog = get_program_name();
    int fd = open(prog, O_RDONLY);
    struct stat st = {0};
    uint8_t *program_bytes = NULL;

    if (fd == -1 || stat(prog, &st) < 0) {
        free(prog);
        return NULL;
    }
    free(prog);
    *size = st.st_size;
    program_bytes = malloc(sizeof(uint8_t) * st.st_size);
    if (!program_bytes || read(fd, program_bytes, st.st_size) < 0)
        return NULL;
    close(fd);
    return program_bytes;
}

IN_SECTION(STUB) static uint32_t load_hash(uint8_t *bytes)
{
    GElf_Shdr *stub_section = (GElf_Shdr *)find_section(bytes, STUB);

    if (!stub_section)
        return 0;
    return hash_bytes(bytes + stub_section->sh_offset, stub_section->sh_size);
}

IN_SECTION(STUB) void load_entrypoint(void)
{
    size_t size = 0;
    uint8_t *bytes = read_current_program(&size);
    GElf_Shdr *text_section = NULL;
    uint32_t hash = 0;
    void (*entrypoint)(void) = NULL;

    if (!bytes)
        return;
    text_section = (GElf_Shdr *)find_section(bytes, ".text");
    hash = load_hash(bytes);
    add_write_perms(bytes + text_section->sh_offset, size);
    xor_bytes(bytes + text_section->sh_offset, size, hash);
    remove_write_perms(bytes + text_section->sh_offset, size);
    entrypoint = (void (*)(void))(bytes + text_section->sh_offset);
    entrypoint();
}
