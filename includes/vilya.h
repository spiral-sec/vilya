
#ifndef VILYA_H
#define VILYA_H

#include <elf.h>
#include <fcntl.h>
#include <gelf.h>
#include <libelf.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>

#ifndef DEFAULT_BUFFER_SIZE
#define DEFAULT_BUFFER_SIZE (512)
#endif /* DEFAULT_BUFFER_SIZE */

/*
    Reprensents new file we are trying to write.
    1st version will be a bit hacky, because we might need to
    recompute GElf_* objects everytime we want them, and work on our output,
    all of the time.
*/
typedef struct dynamic_binary_s {
    char filename[DEFAULT_BUFFER_SIZE];
    GElf_Addr stub_entrypoint;
    Elf *bin;

    uint8_t *output;
    size_t output_size;
} dynbin_t;

/*
   Reprensents target file in its pre-digested state.
   Should be set once, and never used to sync new data.
*/
typedef struct pre_digested_file_s {
    char filename[DEFAULT_BUFFER_SIZE];
    GElf_Addr original_entry_point;

    uint8_t *binary_dump;
    size_t binary_dump_size;
} file_t;

typedef struct user_input_s {
    int verbose : 1;
    char filepath[DEFAULT_BUFFER_SIZE];
    char packer_filepath[DEFAULT_BUFFER_SIZE];
} input_t;

#ifndef STUB_SECTION
#define STUB_SECTION (".vilya")
#endif

// parse.c
int parse(input_t *settings, file_t *file);

#endif /* VILYA_H */
