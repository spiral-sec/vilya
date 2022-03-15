
#include "format_elf.h"
#include <gelf.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "vilya.h"

static int encypher_text_section(file_t *elf, input_t *settings)
{
    GElf_Shdr *text_section = (GElf_Shdr *)find_section(elf->content, ".text");
    uint32_t stub_section_hash = get_hash_from_program();

    if (!text_section) {
        LOG("Error: Could not find .text section");
        return 0;
    }
    LOG_IF(settings->verbose, "[+] Found section .text at addr 0x%ld", text_section->sh_addr);
    xor_bytes(elf->content + text_section->sh_addr, text_section->sh_size, stub_section_hash);
    return 1;
}

static GElf_Phdr *get_last_ptload(file_t *elf)
{
    GElf_Phdr *phdr = (GElf_Phdr *)(elf->content + elf->ehdr.e_phoff);

    for (ssize_t ctr = elf->ehdr.e_shnum; ctr >= 0; ctr--) {
        if (phdr[ctr].p_type == PT_LOAD)
            return &phdr[ctr];
    }
    return NULL;
}

static GElf_Shdr *get_last_section(file_t *elf, GElf_Phdr *pt_load)
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

static int create_section(file_t *elf, GElf_Shdr *last, GElf_Phdr *pt_load)
{
    GElf_Shdr *new_shdr = realloc(elf->symbols, (elf->ehdr.e_shnum + 1) * sizeof(GElf_Shdr));
    GElf_Shdr *to_add = (GElf_Shdr *)find_section(elf->content, LOADER);

    if (!new_shdr)
        return 0;
    to_add->sh_offset = pt_load->p_offset + pt_load->p_memsz;
    to_add->sh_addr = pt_load->p_vaddr + pt_load->p_memsz;
    memmove((uint64_t)new_shdr + last + 2, (uint64_t)new_shdr + (uint16_t *)last + 1,
    sizeof(GElf_Shdr) * ((uint16_t)elf->ehdr.e_shnum - (uint64_t)last - 2));
    memcpy(new_shdr + (uint64_t)last + 1, &to_add, sizeof(GElf_Shdr));
    elf->ehdr.e_shnum += 1;

    /* TODO append newly created section */

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
    if (!create_section(elf, last_section, pt_load))
        return 0;
    LOG_IF(settings->verbose, "[+] successfully created %s section", LOADER);
    return encypher_text_section(elf, settings);
}
