
#include "format_elf.h"
#include <gelf.h>
#include <stdint.h>
#include <string.h>
#include "vilya.h"

__attribute__((section(LOADER))) byte *find_section(byte *content, char const *target_name)
{
    char *current_section_name = NULL;
    GElf_Ehdr *ehdr = (GElf_Ehdr *)content;
    GElf_Shdr *shdr = (GElf_Shdr *)(content + ehdr->e_shoff);
    GElf_Shdr *stable = &shdr[ehdr->e_shstrndx];
    const char *stable_ptr = (const char *)content + stable->sh_offset;

    for (int i = 0; i < ehdr->e_shnum; i++) {
        current_section_name = (char *)(stable_ptr + shdr[i].sh_name);
        if (!strcmp(current_section_name, target_name))
            return (byte *)&shdr[i];
    }
    return NULL;
}

/* Found algorithm here: https://stackoverflow.com/a/7666577 */
__attribute__((section(LOADER))) uint32_t hash_section(byte *content, byte *target_section)
{
    GElf_Shdr *shdr = (GElf_Shdr *)target_section;
    uint32_t hash = HASH_SEED;

    for (size_t ctr = shdr->sh_offset; ctr < shdr->sh_size; ctr++)
        hash = ((hash << 5) + hash) + content[ctr];
    return hash;
}

__attribute__((section(LOADER))) byte *__xor(byte *to_crypt, size_t size, uint32_t hash)
{
    for (size_t ctr = 0; ctr < size; ctr++)
        to_crypt[ctr] ^= hash;
    return to_crypt;
}

__attribute__((section(LOADER))) int load_entry(char const *filepath)
{
    /*
        TODO

        - load and get hash from current section
        - decrypt .text section
        - jump to .text section
    */

    return 0;
}
