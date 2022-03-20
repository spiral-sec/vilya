
#include <gelf.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>
#include "utils.h"
#include "vilya.h"

static void get_page_data(
uint8_t *ptr, uint8_t *end_ptr, uintptr_t *page_start, size_t *page_start_size)
{
    size_t page_size = sysconf(_SC_PAGESIZE);

    *page_start = (uintptr_t)ptr & -page_size;
    *page_start_size = end_ptr - (uint8_t *)*page_start;
}

int add_write_perms(uint8_t *section_ptr, size_t section_size)
{
    uint8_t *end_of_section = section_ptr + section_size;
    size_t p_start_size = 0;
    uintptr_t p_start = 0;
    int perms = PROT_READ | PROT_WRITE | PROT_EXEC;

    get_page_data(section_ptr, end_of_section, &p_start, &p_start_size);
    if (!mprotect((void *)p_start, p_start_size, perms))
        return 0;
    return 1;
}

int remove_write_perms(uint8_t *section_ptr, size_t section_size)
{
    uint8_t *end_of_section = section_ptr + section_size;
    size_t p_start_size = 0;
    uintptr_t p_start = 0;
    int perms = PROT_READ | PROT_EXEC;

    get_page_data(section_ptr, end_of_section, &p_start, &p_start_size);
    if (!mprotect((void *)p_start, p_start_size, perms))
        return 0;
    return 1;
}

uint8_t *find_section(uint8_t *elf, char const *section_name)
{
    GElf_Ehdr *ehdr = (GElf_Ehdr *)elf;
    GElf_Shdr *shdr = NULL;
    GElf_Shdr *str_table = NULL;
    uint8_t *sec_table_ptr = NULL;
    char *curr_section_name = NULL;

    if (!ehdr)
        return NULL;
    // FIXME: For now, we assume the rest of the binary is valid
    shdr = (GElf_Shdr *)(elf + ehdr->e_shoff);
    str_table = &shdr[ehdr->e_shstrndx];
    sec_table_ptr = elf + str_table->sh_offset;
    for (size_t ctr = 0; ctr < ehdr->e_shnum; ctr++) {
        curr_section_name = (char *)(sec_table_ptr + shdr[ctr].sh_name);
        if (!strcmp(curr_section_name, section_name))
            return (uint8_t *)&shdr[ctr];
    }
    return NULL;
}

inline static bool is_printable(uint8_t c)
{
    return (c >= ' ' && c <= '~');
}

char *get_section_name(uint8_t *elf, uint8_t *shdr_ptr)
{
    GElf_Ehdr *ehdr = (GElf_Ehdr *)elf;
    GElf_Shdr *shdr = (!ehdr) ? NULL : (GElf_Shdr *)(elf + ehdr->e_shoff);
    GElf_Shdr *str_table = (!shdr) ? NULL : &shdr[ehdr->e_shstrndx];
    uint8_t *sec_table_ptr = (!str_table) ? NULL : elf + str_table->sh_offset;
    GElf_Shdr *shdr__ = (GElf_Shdr *)shdr_ptr;

    if (!shdr__)
        return NULL;
    return (!sec_table_ptr) ? NULL : (char *)(str_table + shdr__->sh_name);
}

void dump_shdr(uint8_t *elf, size_t elf_len, uint8_t *shdr_ptr)
{
    GElf_Shdr *shdr = (GElf_Shdr *)shdr_ptr;
    uint8_t c = 0;

    LOG("shdr %s@%ld:", get_section_name(elf, shdr_ptr), shdr->sh_offset);
    // TODO dump shdr data here
    for (size_t ctr = shdr->sh_offset; shdr->sh_offset + shdr->sh_size < ctr; ctr++) {
        if (ctr > elf_len)
            break;
        c = is_printable(elf[ctr]) ? elf[ctr] : '.';
        printf("%c", (char)c);
    }
    printf("\n");
}

void dump_ehdr(uint8_t *elf, size_t elf_len)
{
    // TODO
}
