
#include "format_elf.h"
#include <gelf.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "vilya.h"

inline static uint64_t rrotate(uint64_t key)
{
    return (((key & 0xFF) << 56) | (key >> 8));
}

int encypher_text_section(file_t *elf, input_t *settings)
{
    GElf_Shdr *text_section = (GElf_Shdr *)find_section(elf->content, ".text");
    uint64_t stub_section_hash = 0x5d; // TODO
    size_t size = 0;

    if (!text_section) {
        LOG("Error: Could not find .text section");
        return -1;
    }
    size = text_section->sh_size;
    for (size_t ctr = text_section->sh_addr; ctr < size; ctr++) {
        elf->content[ctr] ^= stub_section_hash;
        stub_section_hash = rrotate(stub_section_hash);
    }
    return 0;
}

GElf_Phdr *get_last_ptload(file_t *elf)
{
    GElf_Phdr *phdr = (GElf_Phdr *)(elf->content + elf->ehdr.e_phoff);

    for (size_t ctr = elf->ehdr.e_shnum; ctr >= 0; ctr--) {
        if (phdr[ctr].p_type == PT_LOAD)
            return &phdr[ctr];
    }
    return NULL;
}

GElf_Shdr *get_last_section(file_t *elf, GElf_Phdr *pt_load)
{
    GElf_Shdr *result = NULL;
    GElf_Addr addr;

    if (!pt_load)
        return NULL;
    for (size_t ctr = 0; ctr < elf->ehdr.e_shnum; ctr++) {
        addr = elf->symbols[ctr].sh_addr + elf->symbols[ctr].sh_size;
        if (addr >= pt_load->p_vaddr + pt_load->p_memsz)
            result = &elf->symbols[ctr];
    }
    return result;
}

int create_section(file_t *elf, GElf_Shdr *last, GElf_Phdr *pt_load)
{
    GElf_Shdr *new_shdr = realloc(elf->symbols, (elf->ehdr.e_shnum + 1) * sizeof(GElf_Shdr));
    GElf_Phdr *phdr = (GElf_Phdr *)(elf->content + elf->ehdr.e_phoff);
    uint8_t *loader = NULL;
    uint64_t entry_addr;
    uint64_t entry_size;

    if (!new_shdr)
        return 0;

    new_shdr->sh_offset = pt_load->p_offset + pt_load->p_memsz;
    new_shdr->sh_addr = pt_load->p_vaddr + pt_load->p_memsz;

    /* TODO append LOADER section here */

    elf->ehdr.e_shnum += 1;
    return 1;
}

int append_stub(file_t *elf, input_t *settings)
{
    GElf_Phdr *pt_load = get_last_ptload(elf);
    GElf_Shdr *last_section = get_last_section(elf, pt_load);

    if (!pt_load || !last_section) {
        LOG("Could not find PT_LOAD section or last loaded section");
        return 0;
    }
    create_section(elf, last_section, pt_load);
    return 1;
}
