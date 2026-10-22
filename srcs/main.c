#include "../hdrs/impl.h"
#include <elf.h>

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

typedef struct
s_delf {
	char *name;
	void *map;
	struct stat st;
	unsigned int type;
	t_elf32 elf32;
	t_elf64 elf64;
	int err;
} t_delf;

typedef struct
s_hldr {
	size_t ldelf;
	t_delf *delf;
} t_hldr;


void nm_elf32(t_delf *new) {
	new->type = 32;
	new->elf32.ehdr = (Elf32_Ehdr *)new->map;
	new->elf32.shdr = (Elf32_Shdr *)(new->map + new->elf32.ehdr->e_shoff);
	for (size_t i = 0; i < new->elf32.ehdr->e_shnum; i++) {
		if (new->elf32.shdr[i].sh_type != SHT_SYMTAB) {
			continue;
		}
		new->elf32.sym = (Elf32_Sym *)(new->map + new->elf32.shdr[i].sh_offset);
		const char *strtab = (const char *)new->map + new->elf32.shdr[new->elf32.shdr[i].sh_link].sh_offset;
		for (size_t j = 1; j < (new->elf32.shdr[i].sh_size / sizeof(Elf32_Sym)); j++) {
			if (ELF32_ST_TYPE(new->elf32.sym[j].st_info) == STT_SECTION)
				continue;
			if (ELF32_ST_TYPE(new->elf32.sym[j].st_info) == STT_FILE)
				continue;
			printf("%08x %s\n",
		  		new->elf32.sym[j].st_value,
		   		strtab + new->elf32.sym[j].st_name);
		}
	}
}

void nm_elf64(t_delf *new) {
	new->type = 64;
}

void 
check_elf_magic(t_delf *new) {
	unsigned char *p = (unsigned char *)new->map;
	if (new->st.st_size < SELFMAG || (memcmp(p, ELFMAG, SELFMAG) != 0)) {
		new->err = _FILE_FORMAT;
	}
	switch (p[EI_CLASS]) {
	case ELFCLASS32:
		nm_elf32(new);
		break;
	case ELFCLASS64:
		nm_elf64(new);
		break;
	default:
		new->err = _FILE_FORMAT;
		break;
	}
}

void
add_delf(const char *path, t_delf *new) {
	new->name = strdup(path);
	FILE *f = fopen(path, "r");
	if (!f) {
		new->err = _FILE;
		return ;
	}
	new->err = false;
	if (fstat((fileno(f)),&new->st) < 0) {
		perror("fstat");
		exit(1);
	}
	new->map = mmap(NULL, new->st.st_size, PROT_READ, MAP_PRIVATE, (fileno(f)), 0);
	if (new->map == MAP_FAILED) {
		perror("map");
		exit(1);
	}
	check_elf_magic(new);
}

void
report_err(t_delf *d) {
	switch (d->err) {
	case _FILE:
		_FILE_STR(d->name);
		break;
	case _FILE_FORMAT:
		_FILE_FORMAT_STR(d->name);
		break;
	default:
		break;
	}
}


int
main(int ac, char **av) {
	if (ac < MIN_ARGS) {
		ERR_MIN_ARGS;
	}
	t_hldr *hldr = calloc(1, sizeof(t_hldr));
	if (!hldr) {
		return 1;
	}
	hldr->ldelf = ac - 1;
	hldr->delf = calloc(hldr->ldelf, sizeof(t_delf));
	if (!hldr->delf) {
		return 1;
	}
	for (size_t i = 1; i < (size_t)ac; i++) {
		add_delf(av[i], &hldr->delf[i - 1]);
	}
	for (size_t i = 0; i < hldr->ldelf; i++) {
		report_err(&hldr->delf[i]);
	}
}
