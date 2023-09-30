#ifndef SL_TOOL_H
#define SL_TOOL_H

#include <stddef.h>

#define SL_ALLOC_CHECK(ptr) 																												\
	if (ptr == NULL) { 																														\
		fprintf(stderr, "[CRITICAL] %s:%llu: In function '%s': Memory allocation failed!\n", __FILE__, __LINE__, __PRETTY_FUNCTION__); 		\
		exit(1); 																															\
	}

char *SL_shift_args(int *argc, char ***argv);
char *SL_strndup(const char *s1, size_t n);

#endif // SL_TOOL_H