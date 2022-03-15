
#ifndef FORMAT_ELF_H_
#define FORMAT_ELF_H_

#include <elf.h>
#include <gelf.h>
#include <stdbool.h>
#include "vilya.h"

#ifndef ELF_HEADER
#define ELF_HEADER ("\177ELF")
#endif /* ELF_HEADER */

// file.c
int read_elf(file_t *file_handler, input_t *settings);
int write_to_outfile(file_t *result, input_t *settings);

// sections.c
char *get_section_name(file_t *elf, GElf_Shdr *shdr);
byte *find_section(byte *elf, char const *section_name);
void display_shdr_content(GElf_Shdr *shdr, file_t *elf, input_t *settings);

// stub_manager.c
int append_stub(file_t *file, input_t *settings);

#endif /* FORMAT_ELF_H_ */
