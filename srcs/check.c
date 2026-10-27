#include "../hdrs/impl.h"

apply_delf
check_elf_magic(t_delf *_new) {
	unsigned char *p = (unsigned char *)_new->map;
	if (_new->st.st_size < SELFMAG || (memcmp(p, ELFMAG, SELFMAG) != 0)) {
		_new->err = _FILE_FORMAT;
	}
	switch (p[EI_CLASS]) {
	case ELFCLASS32: return (_new->type = ARCH32, nm_elf32);
	case ELFCLASS64: return (_new->type = ARCH64, nm_elf64);
	default:
		_new->err = _FILE_FORMAT;
		return NULL;
	}
}

void
process_delf(const char *_pth, t_delf *_new) {
	_new->name = strdup(_pth);
	FILE *f = fopen(_pth, "r");
	if (!f) {
		_new->err = _FILE;
		return ;
	}
	_new->err = false;
	if (fstat((fileno(f)),&_new->st) < 0) {
		perror("fstat");
		exit(1);
	}
	_new->map = mmap(NULL, _new->st.st_size, PROT_READ, MAP_PRIVATE, (fileno(f)), 0);
	if (_new->map == MAP_FAILED) {
		perror("map");
		exit(1);
	}
	apply_delf nm_fn = check_elf_magic(_new);
	if (nm_fn) {
		nm_fn(_new);
	}
}