
#include "format_elf.h"
#include <gelf.h>
#include <string.h>
#include "vilya.h"

char *get_section_name(file_t *file, GElf_Shdr *shdr)
{
    return (char *)(file->section_names + shdr->sh_name);
}

inline static bool is_printable(byte c)
{
    return (c >= ' ' && c <= '~');
}

void display_shdr_content(GElf_Shdr *shdr, file_t *elf, input_t *settings)
{
    byte c = 0;

    LOG("Reading content of [%s]", get_section_name(elf, shdr));
    for (size_t ctr = shdr->sh_offset; shdr->sh_offset + shdr->sh_size > ctr; ctr++) {
        if (ctr > elf->content_size) {
            LOG_IF(settings->verbose, "Attempting to read section after end of file");
            return;
        }
        c = is_printable(elf->content[ctr]) ? elf->content[ctr] : '.';
        printf("%c", (char)c);
    }
    printf("\n");
}
