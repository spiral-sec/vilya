
#ifndef FORMAT_ELF_H_
#define FORMAT_ELF_H_

#include <elf.h>
#include <stdbool.h>
#include "vilya.h"

#ifndef ELF_HEADER
#define ELF_HEADER ("\177ELF")
#endif /* ELF_HEADER */

// file.c
int open_file(file_t *result, input_t *settings);
int write_to_outfile(file_t *result, input_t *settings);

#endif /* FORMAT_ELF_H_ */
