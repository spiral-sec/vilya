
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
    uint8_t *src;
    uint8_t *out;
    size_t out_size;
    size_t src_size;
    uint8_t *stub_content;
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

#ifndef STUB
#define STUB (".vilya")
#endif

#ifndef HASH_SEED
#define HASH_SEED (".vilya.seed")
#endif

#define OFFSET_PLACE (0x800000)

// parse.c
int parse(input_t *settings, file_t *file);
void gen_filename(char const *src, char result[DEFAULT_BUFFER_SIZE]);

// sections.c
uint8_t *find_section(uint8_t *header, char const *section_name);
int remove_write_perms(uint8_t *section_ptr, size_t section_size);
int add_write_perms(uint8_t *section_ptr, size_t section_size);
void dump_shdr(uint8_t *elf, size_t elf_len, uint8_t *shdr_ptr);
void dump_ehdr(uint8_t *elf, size_t elf_len);
char *get_section_name(uint8_t *elf, uint8_t *shdr_ptr);

// unload.c
void load_entrypoint(void);
uint8_t *read_current_program(size_t *size);
uint32_t hash_bytes(uint8_t *bytes, size_t size);
void xor_bytes(uint8_t *to_xor, size_t size, uint32_t hash);

// pack.c
int pack(input_t *settings, file_t *file);

#endif /* VILYA_H */
