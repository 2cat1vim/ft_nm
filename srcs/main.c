#include "../hdrs/impl.h"

typedef struct
s_delf {
	char *name;
	FILE *f;
	int err;
} t_delf;

typedef struct
s_hldr {
	size_t ldelf;
	t_delf *delf;
} t_hldr;

void 
check_elf_header(t_delf *new) {
	char buf[(size_t)SELFMAG];
	size_t r = fread(buf, sizeof(*buf), (size_t)SELFMAG, new->f);
	if (r != (size_t)SELFMAG) {
		new->err = _FILE_FORMAT;
		return ;
	}
	if (strncmp(buf, ELFMAG, (size_t)SELFMAG) != 0) {
		new->err = _FILE_FORMAT;
		return ;
	}	
}

void
add_delf(const char *path, t_delf *new) {
	new->name = strdup(path);
	new->f = fopen(path, "r");
	if (!new->f) {
		new->err = _FILE;
		return ;
	}
	new->err = false;
	check_elf_header(new);
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
