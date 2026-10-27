#include "../hdrs/impl.h"

void
nm_elf32(t_delf *_new) {
	_new->elf32.ehdr = (Elf32_Ehdr *)_new->map;
	_new->elf32.shdr = (Elf32_Shdr *)(_new->map + _new->elf32.ehdr->e_shoff);
	t_store **tail = &_new->store;
	for (size_t i = 0; i < _new->elf32.ehdr->e_shnum; i++) {
		if (_new->elf32.shdr[i].sh_type != SHT_SYMTAB) {
			continue;
		}
		_new->elf32.sym = (Elf32_Sym *)(_new->map + _new->elf32.shdr[i].sh_offset);
		const char *strtab = (const char *)_new->map + _new->elf32.shdr[_new->elf32.shdr[i].sh_link].sh_offset;
		for (size_t j = 1; j < (_new->elf32.shdr[i].sh_size / sizeof(Elf32_Sym)); j++) {
			if (ELF32_ST_TYPE(_new->elf32.sym[j].st_info) == STT_SECTION)
				continue;
			if (ELF32_ST_TYPE(_new->elf32.sym[j].st_info) == STT_FILE)
				continue;
			char c = get_char_from_type(_new->elf32.sym[j].st_info, _new->elf32.sym[j].st_shndx);
			unsigned int def = _new->elf32.sym[j].st_shndx;
			t_store *node = calloc(1, sizeof(t_store));
		       	if (!node) {
				perror("calloc");
				exit(1);
			}	
			node->type = c;
			node->line = strdup(strtab + _new->elf32.sym[j].st_name);
			node->addr = (def == SHN_UNDEF ? 0 : _new->elf32.sym[j].st_value);
			*tail = node;
			tail = &node->next;
		}
	}
}

void
nm_elf64(t_delf *_new) {
	_new->elf64.ehdr = (Elf64_Ehdr *)_new->map;
	_new->elf64.shdr = (Elf64_Shdr *)(_new->map + _new->elf64.ehdr->e_shoff);
	t_store **tail = &_new->store;
	for (size_t i = 0; i < _new->elf64.ehdr->e_shnum; i++) {
		if (_new->elf64.shdr[i].sh_type != SHT_SYMTAB) {
			continue;
		}
		_new->elf64.sym = (Elf64_Sym *)(_new->map + _new->elf64.shdr[i].sh_offset);
		const char *strtab = (const char *)_new->map + _new->elf64.shdr[_new->elf64.shdr[i].sh_link].sh_offset;
		for (size_t j = 1; j < (_new->elf64.shdr[i].sh_size / sizeof(Elf64_Sym)); j++) {
			if (ELF32_ST_TYPE(_new->elf64.sym[j].st_info) == STT_SECTION)
				continue;
			if (ELF32_ST_TYPE(_new->elf64.sym[j].st_info) == STT_FILE)
				continue;
			char c = get_char_from_type(_new->elf64.sym[j].st_info, _new->elf64.sym[j].st_shndx);
			unsigned int def = _new->elf64.sym[j].st_shndx;
			t_store *node = calloc(1, sizeof(t_store));
		       	if (!node) {
				perror("calloc");
				exit(1);
			}	
			node->type = c;
			node->line = strdup(strtab + _new->elf64.sym[j].st_name);
			node->addr = (def == SHN_UNDEF ? 0 : _new->elf64.sym[j].st_value);
			*tail = node;
			tail = &node->next;
		}
	}
}

char
get_char_from_type(unsigned char st_info, uint32_t st_shndx) {
	char c = 'a';
	int (*capy)(int) = toupper; // STB_GLOBAL : default
	unsigned char bind = ELF32_ST_BIND(st_info);
	unsigned char type = ELF32_ST_TYPE(st_info);
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
	else if (st_shndx == SHN_UNDEF) {
		c = 'U';
	}
	return (char)capy(c);
}