#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

char *make_message(const char *fmt, ...)
{
	int n;
	int size = 100;     /* Guess we need no more than 100 bytes */
	char *p, *np;
	va_list ap;

	p = malloc(size);
	if (p == NULL)
		return NULL;

	while (1) {

		/* Try to print in the allocated space */

		va_start(ap, fmt);
		n = vsnprintf(p, size, fmt, ap);
		va_end(ap);

		/* Check error code */

		if (n < 0) {
			free(p);
			return NULL;
		}

		/* If that worked, return the string */

		if (n < size)
			return p;

		/* Else try again with more space */

		size = n + 1;       /* Precisely what is needed */

		np = realloc(p, size);
		if (np == NULL) {
			free(p);
			return NULL;
		} else {
			p = np;
		}
	}
}


int main(int argc, char *argv[])
{
	printf("%s", make_message("%d, %s, %d\n", 1, "aaa"));
	return 0;
}
