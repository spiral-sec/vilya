
#include <gelf.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "vilya.h"
#include "utils.h"

static void prepare_packing(dynbin_t *packed, file_t *src)
{
    gen_filename(src->filename, packed->filename);
    packed->output_size = src->binary_dump_size;
    packed->output = malloc(sizeof(uint8_t *) * packed->output_size);
    if (!packed->output)
        return;
    memmove(packed->output, src->binary_dump, packed->output_size);
}

GElf_Shdr *copy_shdr(uint8_t *ehdr, char const *target)
{
    GElf_Shdr *shdr = (GElf_Shdr *)find_section(ehdr, target);
    GElf_Shdr *result = malloc(sizeof(GElf_Shdr));

    if (!result)
        return NULL;
    result->sh_addr = shdr->sh_addr;
    result->sh_addralign = shdr->sh_addralign;
    result->sh_entsize = shdr->sh_entsize;
    result->sh_flags = shdr->sh_addr;
    result->sh_info = shdr->sh_info;
    result->sh_type = shdr->sh_type;
    result->sh_size = shdr->sh_size;
    return result;
}

int pack(input_t *settings, file_t *src)
{
    dynbin_t packed = {0};
    GElf_Shdr *stub_shdr = NULL;
    GElf_Shdr *seed_shdr = NULL;

    prepare_packing(&packed, src);
    stub_shdr = copy_shdr(packed.output, STUB);
    seed_shdr = copy_shdr(packed.output, HASH_SEED);

    /* TODO append sections to ehdr then to binary output */

    return 1;
}
