
#include <elf.h>
#include <fcntl.h>
#include <gelf.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include "utils.h"
#include "vilya.h"

static void prepare_packing(dynbin_t *packed, file_t *src)
{
    gen_filename(src->filename, packed->filename);
    packed->src_size = src->binary_dump_size;
    packed->src = malloc(sizeof(uint8_t) * packed->src_size);
    packed->out_size = src->binary_dump_size + 5;
    packed->out = malloc(sizeof(uint8_t) * packed->out_size);
    if (!packed->out || !packed->src)
        return;
    memcpy(packed->src, src->binary_dump, packed->src_size);
    memcpy(packed->out, src->binary_dump, packed->out_size);
    // TODO    packed->stub_content = find_section(, const char *section_name);
}

static off_t get_virtual_addr(file_t *src, size_t *new_offset)
{
    GElf_Ehdr *ehdr = (GElf_Ehdr *)src->binary_dump;
    GElf_Phdr *phdr = NULL;
    size_t phnum = ehdr->e_phnum;

    if ((off_t)(phnum + sizeof(GElf_Phdr) + ehdr->e_phoff) > src->binary_dump_size)
        return 0;
    phdr = (GElf_Phdr *)ehdr->e_phoff;
    for (size_t ctr = 0; ctr < phnum; ctr++) {
        if (phdr[ctr].p_type == PT_LOAD) {
            *new_offset = phdr[ctr].p_memsz;
            return (off_t)(phdr[ctr].p_vaddr);
        }
    }
    return 0;
}

static void destroy_dynbin(dynbin_t *bin)
{
    free(bin->out);
    free(bin->src);
}

static int load_code_cave(dynbin_t *target, input_t *settings)
{
    GElf_Ehdr *ehdr = (GElf_Ehdr *)target->out;
    off_t offset = ehdr->e_phoff + ehdr->e_shentsize + ehdr->e_shnum;
    size_t stub_size = sizeof(target->stub_content);

    offset = ALIGN_OFFSET(offset);
    LOG_IF(
    settings->verbose, "[+] Resizing file: +0x%08lx", offset + stub_size - target->out_size);
    target->out = realloc(target->out, offset + stub_size);
    if (!target->out) {
        destroy_dynbin(target);
        LOG("[!] Could not allocate memory");
        return 0;
    }
    target->out_size = offset + stub_size;
    return (int)offset;
}

static int update_htable(dynbin_t *target, input_t *settings, off_t vaddr, off_t cc_entry)
{
    GElf_Ehdr *ehdr = (GElf_Ehdr *)target->out;
    GElf_Phdr *htable = (GElf_Phdr *)(ehdr->e_phoff + target->out);
    size_t htable_len = ehdr->e_phnum;
    size_t entry_size = ehdr->e_phentsize;

    for (size_t ctr = 0; ctr < htable_len; ctr++) {
        if (htable->p_type == PT_NOTE) {
            htable->p_type = PT_LOAD;
            htable->p_offset = vaddr;
            htable->p_vaddr = cc_entry + vaddr + OFFSET_PLACE;
            htable->p_filesz = sizeof(target->stub_content) + 5;
            htable->p_memsz = sizeof(target->stub_content) + 5;
            htable->p_flags = PF_X | PF_R;
            return 1;
        }
        htable += entry_size;
    }
    return 0;
}

static int setup_payload(dynbin_t *target, off_t vaddr)
{
    GElf_Ehdr *ehdr = (GElf_Ehdr *)target->out;
    off_t offset = ehdr->e_entry;
    off_t jmp_addr = 0;

    return 0;
}

int pack(input_t *settings, file_t *src)
{
    dynbin_t packed = {0};
    off_t virtual_addr = 0;
    off_t code_cave_entry = 0;
    size_t new_offset = 0;

    prepare_packing(&packed, src);
    virtual_addr = get_virtual_addr(src, &new_offset);
    if (!virtual_addr && !new_offset) {
        LOG("[!] Could not find PT_LOAD segment");
        destroy_dynbin(&packed);
        return 0;
    }
    code_cave_entry = load_code_cave(&packed, settings);
    if (!code_cave_entry || !update_htable(&packed, settings, virtual_addr, code_cave_entry)) {
        LOG("[!] Could not load memory for code cave");
        return 0;
    }
    code_cave_entry += virtual_addr + OFFSET_PLACE;
    LOG_IF(settings->verbose, "[+] Overriding entrypoint 0x%08lx with 0x%08lx",
    ((GElf_Ehdr *)packed.out)->e_entry, code_cave_entry);
    ((GElf_Ehdr *)packed.out)->e_entry = code_cave_entry;

    // TODO load payload

    // TODO write file to disc

    return 1;
}
