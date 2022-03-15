
#include "format_elf.h"
#include <gelf.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include "vilya.h"

__attribute__((section(LOADER))) byte *find_section(byte *content, char const *target_name)
{
    char *current_section_name = NULL;
    GElf_Ehdr *ehdr = (GElf_Ehdr *)content;
    GElf_Shdr *shdr = (GElf_Shdr *)(content + ehdr->e_shoff);
    GElf_Shdr *stable = &shdr[ehdr->e_shstrndx];
    byte *stable_ptr = content + stable->sh_offset;

    for (int i = 0; i < ehdr->e_shnum; i++) {
        current_section_name = (char *)(stable_ptr + shdr[i].sh_name);
        if (!strcmp(current_section_name, target_name))
            return (byte *)&shdr[i];
    }
    return NULL;
}

/* Found algorithm here: https://stackoverflow.com/a/7666577 */
__attribute__((section(LOADER))) static uint32_t hash_section(byte *content, byte *target_section)
{
    GElf_Shdr *shdr = (GElf_Shdr *)target_section;
    const size_t end_addr = shdr->sh_addr + shdr->sh_size;
    uint32_t hash = 0;

    for (size_t ctr = shdr->sh_addr; ctr < end_addr; ctr++)
        hash = content[ctr] + (hash << 6) + (hash << 16) - hash;
    return hash;
}

__attribute__((section(LOADER))) void xor_bytes(byte *to_crypt, size_t size, uint32_t hash)
{
    for (size_t ctr = 0; ctr < size; ctr++)
        to_crypt[ctr] ^= hash;
}

__attribute__((section(LOADER))) static char *program_path(void)
{
    char *path = malloc(DEFAULT_BUFFER_SIZE);

    if (path != NULL) {
        if (readlink("/proc/self/exe", path, DEFAULT_BUFFER_SIZE) == -1) {
            free(path);
            path = NULL;
        }
    }
    return path;
}

__attribute__((section(LOADER))) uint32_t get_hash_from_program(void)
{
    char *filepath = program_path();
    int fd = open(filepath, O_RDONLY);
    struct stat st = {0};
    byte *binary_blob = NULL;
    byte *loader_section = NULL;
    uint32_t section_hash = 0;

    if (fd < 0 || stat(filepath, &st) < 0) {
        free(filepath);
        return 0;
    }
    free(filepath);
    binary_blob = malloc(sizeof(byte) * (size_t)(st.st_size + 1));
    if (!binary_blob || read(fd, binary_blob, (size_t)st.st_size) < 0 ||
    !(loader_section = find_section(binary_blob, LOADER)))
        return 0;
    close(fd);
    section_hash = hash_section(binary_blob, loader_section);
    free(binary_blob);
    return section_hash;
}

__attribute__((section(LOADER))) int load_entry(void)
{
    char *filepath = program_path();
    int fd = open(filepath, O_RDONLY);
    struct stat st = {0};
    byte *binary_blob = NULL;
    GElf_Shdr *text_section = NULL;
    uint32_t section_hash = 0;
    void (*text_entry)(void) = NULL;

    if (fd < 0 || stat(filepath, &st) < 0) {
        free(filepath);
        return 0;
    }
    binary_blob = malloc(sizeof(byte) * (size_t)(st.st_size + 1));
    if (!binary_blob || read(fd, binary_blob, (size_t)st.st_size) < 0) {
        free(filepath);
        return 0;
    }
    close(fd);
    free(filepath);
    text_section = (GElf_Shdr *)find_section(binary_blob, ".text");
    section_hash = get_hash_from_program();
    if (!text_section)
        return 0;
    xor_bytes(binary_blob + text_section->sh_addr, text_section->sh_size, section_hash);
    text_entry = (void (*)(void))(binary_blob + text_section->sh_addr);
    text_entry();
    return 1;
}
