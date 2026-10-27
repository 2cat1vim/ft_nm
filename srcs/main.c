#include "../hdrs/impl.h"

void
report(t_delf *_cur, bool _is_alone) {
	switch (_cur->err) {
	case _FILE:
		_FILE_STR(_cur->name);
		return ;
	case _FILE_FORMAT:
		_FILE_FORMAT_STR(_cur->name);
		return ;
	default:
		break ;
	}
	if (!_is_alone) {
		printf("\n%s:\n", _cur->name);
	}
	t_store *store = _cur->store;
	sort_storage(store);
	for (t_store *cur = store; cur && cur->next; cur = cur->next) {
		int jump = (_cur->type == ARCH32) ? JUMP32 : JUMP64;
		if (cur->addr == ZERO) {
			char space_jump[jump + 1];
			memset(space_jump, ' ', jump);
			space_jump[jump] = '\0';
			printf("%s %c %s\n",
				space_jump,
				cur->type,
				cur->line);
		}
		else {
			printf("%0*llx %c %s\n",
				jump,
				(unsigned long long)cur->addr,
				cur->type,
				cur->line);
		}
	}
}

int
main(int _ac, char **_av) {
	if (_ac < MIN_ARGS) {
		ERR_MIN_ARGS;
	}
	t_hldr *hldr = calloc(1, sizeof(t_hldr));
	if (!hldr) {
		return 1;
	}
	hldr->ldelf = _ac - 1;
	hldr->delf = calloc(hldr->ldelf, sizeof(t_delf));
	if (!hldr->delf) {
		return 1;
	}
	for (size_t i = 1; i < (size_t)_ac; i++) {
		process_delf(_av[i], &hldr->delf[i - 1]);
	}
	for (size_t i = 0; i < hldr->ldelf; i++) {
		bool is_alone = false;
		if (hldr->ldelf == 1) {
			is_alone = true;
		}
		report(&hldr->delf[i], is_alone);
	}
}
