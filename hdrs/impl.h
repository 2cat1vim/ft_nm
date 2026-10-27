#ifndef IMPL_H
#define IMPL_H

/* LOOP ERROR CODE
 * Program err loop return code
*/
#define _NONE 0
#define _FILE 1
#define _FILE_FORMAT 2

#define _FILE_STR(s) \
	fprintf(stderr, "[ft_nm]: Cannot find the file `%s`\n", s);
#define _FILE_FORMAT_STR(s) \
	fprintf(stderr, "[ft_nm]: Cannot recognize the file `%s`\n", s);

/* INTEGER DEFS
 * When i need to simplify integer integrations
*/ 
#define MIN_ARGS 2
#define ZERO 0

/* ELF HELPER
 * Groups
*/
#define ARCH32 1
#define ARCH64 2
#define JUMP32 8
#define JUMP64 16

/* PRINT STR
 * When i need to pass str to my perr or pout
*/ 
#define ERR_MIN_ARGS \
	fprintf(stderr, "[ft_nm]: Specify at least one binary to list its symbols\n");

#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <ctype.h>
#include <elf.h>
#include <locale.h>

typedef struct
s_elf32 {
	Elf32_Ehdr *ehdr;
	Elf32_Shdr *shdr;
	Elf32_Sym  *sym;
} t_elf32;

typedef struct 
s_elf64 {
	Elf64_Ehdr *ehdr;
	Elf64_Shdr *shdr;
	Elf64_Sym  *sym;
} t_elf64;

typedef struct s_store {
	char 		*line;
	char 		type;
	Elf64_Addr	addr;
	struct s_store 	*next;
} t_store;

typedef struct
s_delf {
	t_store *store;
	char *name;
	char *view;
	void *map;
    unsigned int err;
    unsigned int type;
	struct stat st;
	t_elf32 elf32;
	t_elf64 elf64;
} t_delf;

typedef struct
s_hldr {
	size_t ldelf;
	t_delf *delf;
} t_hldr;

typedef 
void (*apply_delf)(t_delf*);

void nm_elf32(t_delf *_new);
void nm_elf64(t_delf *_new);
char get_char_from_type(unsigned char st_info, uint32_t st_shndx, unsigned int sh_type, unsigned int sh_flags);
void process_delf(const char *path, t_delf *_new);
apply_delf check_elf_magic(t_delf *_new);
bool is_sort_storage(t_store *store);
int	cmp_storage(const char *a, const char *b);
void sort_storage(t_store *store);
void swap_storage(t_store *a, t_store *b);
#endif
