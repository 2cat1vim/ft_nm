#include <stdio.h>

void
perr(const char *str) {
	fprintf(stderr, "%s\n", str);
}

void
pout(const char *str) {
	fprintf(stdout, "%s\n", str);
}
