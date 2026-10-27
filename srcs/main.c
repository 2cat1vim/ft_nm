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

typedef struct s_store {
	char 		*line;
	char 		type;
	Elf64_Addr	addr;

	struct s_store 	*next;
} t_store;

typedef struct
s_delf {
	t_store 	*store;

	char 		*name;
	char 		*view;
	void	 	*map;
	unsigned int	err;
	struct 		stat st;

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

char
get_char_from_type(Elf32_Sym *sym) {
	char c = 'a';
	int (*capy)(int) = toupper; // STB_GLOBAL : default
	unsigned char bind = ELF32_ST_BIND(sym->st_info);
	unsigned char type = ELF32_ST_TYPE(sym->st_info);
	if (bind == STB_LOCAL) {
		capy = tolower;
	}
	if (bind == STB_WEAK) {
		if (type == STT_OBJECT) {
			c = 'v';
		}
		else {
			c = 'w';
		}
	}
	else if (sym->st_shndx == SHN_UNDEF) {
		c = 'U';
	}
	return (char)capy(c);
}

void
nm_elf32(t_delf *new) {
	new->elf32.ehdr = (Elf32_Ehdr *)new->map;
	new->elf32.shdr = (Elf32_Shdr *)(new->map + new->elf32.ehdr->e_shoff);
	t_store **tail = &new->store;
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
			char c = get_char_from_type(&new->elf32.sym[j]);
			unsigned int def = new->elf32.sym[j].st_shndx;
			t_store *node = calloc(1, sizeof(t_store));
		       	if (!node) {
				perror("calloc");
				exit(1);
			}	
			node->type = c;
			node->line = strdup(strtab + new->elf32.sym[j].st_name);
			node->addr = (def == SHN_UNDEF ? 0 : new->elf32.sym[j].st_value);
			*tail = node;
			tail = &node->next;
		}
	}
}

void nm_elf64(t_delf *new) {
	(void)new;
}

apply_delf
check_elf_magic(t_delf *new) {
	unsigned char *p = (unsigned char *)new->map;
	if (new->st.st_size < SELFMAG || (memcmp(p, ELFMAG, SELFMAG) != 0)) {
		new->err = _FILE_FORMAT;
	}
	switch (p[EI_CLASS]) {
	case ELFCLASS32: return nm_elf32;
	case ELFCLASS64: return nm_elf64;
	default:
		new->err = _FILE_FORMAT;
		return NULL;
	}
}

void
process_one_delf(const char *path, t_delf *new) {
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
	apply_delf nm_fn = check_elf_magic(new);
	if (nm_fn) {
		nm_fn(new);
	}
}

void
report_one(t_delf *d, bool is_alone) {
	switch (d->err) {
	case _FILE:
		_FILE_STR(d->name);
		return ;
	case _FILE_FORMAT:
		_FILE_FORMAT_STR(d->name);
		return ;
	default:
		break ;
	}
	if (!is_alone) {
		printf("\n%s:\n", d->name);
	}
	t_store *store = d->store;
	while (store) {
		if (store->addr == 0) {
			printf("         %c %s\n",
				store->type,
				store->line);
		}
		else {
			printf("%08x %c %s\n",
				(unsigned int)store->addr,
				store->type,
				store->line);
		}
		store = store->next;
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
		process_one_delf(av[i], &hldr->delf[i - 1]);
	}
	for (size_t i = 0; i < hldr->ldelf; i++) {
		bool is_alone = false;
		if (hldr->ldelf == 1) {
			is_alone = true;
		}
		report_one(&hldr->delf[i], is_alone);
	}
}
