#include <stdio.h>
#include <stdlib.h>

void throw_error(const char *error_str)
{
	printf("Error: %s\n", error_str);
	exit(EXIT_FAILURE);
}
