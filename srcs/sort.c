#include "../hdrs/impl.h"

void
swap_storage(t_store *a, t_store *b) {
	char		*line = a->line;
	char		type = a->type;
	Elf64_Addr	addr = a->addr;

	a->line = b->line;
	a->type = b->type;
	a->addr = b->addr;
	b->line = line;
	b->type = type;
	b->addr = addr;
}

bool
is_sort_storage(t_store *store)
{
	for (t_store *cur = store; cur && cur->next; cur = cur->next) {
		if (cmp_storage(cur->line, cur->next->line) > 0)
			return (false);
	}
	return (true);
}

int
cmp_storage(const char *a, const char *b)
{
	size_t	i = 0, j = 0;
	while (true) {
		while (a[i] && !isalnum(a[i]))
			i++;
		while (b[j] && !isalnum(b[j]))
			j++;
		if (!a[i] || !b[j])
			break ;
		int ca = tolower(a[i]);
		int cb = tolower(b[j]);
		if (ca != cb)
			return (ca - cb);
		i++;
		j++;
	}
	if (a[i] != b[j])
		return (a[i] ? 1 : -1);
	return (strcmp(a, b));
}
void
sort_storage(t_store *store)
{
	while (!is_sort_storage(store)) {
		for (t_store *cur = store; cur && cur->next; cur = cur->next) {
			if (cmp_storage(cur->line, cur->next->line) > 0)
				swap_storage(cur, cur->next);
		}
	}
}
