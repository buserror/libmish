// small test unit for mish_argv_make
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
/*
 * Duplicate 'line', split it into words, store word pointers in an array,
 * NULL terminate it. Also return the number of words in the array in argc.
 *
 * The returned value is made of two malloc()ed blocks. use mish_argv_free
 * to free the memory.
 * It's OK to change almost any of the pointers APART from argv[0] which
 * contains the block containing a copy of the original 'line' parameters with
 * \0's replacing spaces.
 */
static char **
mish_argv_make(
		const char * line,
		int * argc )
{
	char *dup = strdup(line);
	int i = 0;
	char ** av = NULL;
	int skip = 0;
	int state = 0;
	char start;
	enum { s_newarg, s_startarg, s_copyq, s_skip, s_copynq };
	do {
		switch (state) {
			case s_newarg:
				av = realloc(av, (i + 2) * sizeof(char*));
				while (*dup == ' ')
					dup++;
				av[i++] = dup;
				state = s_startarg;
				break;
			case s_startarg:
				if (*dup == '"' || *dup == '\'') {
					start = *dup++;
					state = s_copyq;
				} else
					state = s_copynq;
				break;
			case s_copyq:
				if (*dup == '\\')
					state = s_skip;
				else if (*dup == start) {
					state = s_newarg;
					dup++;
					if (*dup) *dup++ = 0;
				} else if (*dup)
					dup++;
				break;
			case s_skip:
				dup++;
				state = s_copyq;
				break;
			case s_copynq:
				if (*dup == 0)
					break;
				if (*dup != ' ')
					dup++;
				else {
					state = s_newarg;
					if (*dup) *dup++ = 0;
				}
				break;
		}
	} while (*dup);
	av[i] = NULL;
	*argc = i;
	return av;
}

int main() {
	int argc = 0;
	char **argv = mish_argv_make("testing \"one escape two\"  ala ", &argc);

	printf("argc = %d\n", argc);
	for (int i = 0; argv[i]; i++)
		printf("%2d:'%s'\n", i, argv[i]);

}
