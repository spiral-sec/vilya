
#include "format_elf.h"
#include <gelf.h>
#include <string.h>
#include "vilya.h"

char *get_section_name(file_t *file, GElf_Shdr *shdr)
{
    return (char *)(file->section_names + shdr->sh_name);
}

GElf_Shdr *find_section(file_t *elf, char const *section_name)
{
    GElf_Shdr *shdr = (void *)elf->content + elf->ehdr.e_shoff;
    char *current_section_name = NULL;

    for (int ctr = 0; elf->ehdr.e_shnum < ctr; ctr++) {
        current_section_name = get_section_name(elf, &shdr[ctr]);
        if (current_section_name != NULL && !strcmp(current_section_name, section_name))
            return &shdr[ctr];
    }
    return NULL;
}

inline static bool is_printable(char c)
{
    return (c >= ' ' && c <= '~');
}

void display_shdr_content(GElf_Shdr *shdr, file_t *elf, input_t *settings)
{
    char c = 0;

    LOG("Reading content of [%s]", get_section_name(elf, shdr));
    for (size_t ctr = shdr->sh_offset; shdr->sh_offset + shdr->sh_size > ctr; ctr++) {
        if (ctr > elf->content_size) {
            LOG_IF(settings->verbose, "Attempting to read section after end of file");
            return;
        }
        c = is_printable(elf->content[ctr]) ? elf->content[ctr] : '.';
        printf("%c", c);
    }
    printf("\n");
}
