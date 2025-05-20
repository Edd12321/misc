/* shub.hpp - Shell substitution in C++ */
#include <cstdio>
#include <cstdlib>

#define $(...) ({ \
	FILE *fp = popen(#__VA_ARGS__, "r"); \
	std::string ret; \
	if (fp) { \
		char buf[BUFSIZ + 1]; \
		while (fgets(buf, BUFSIZ, fp) != NULL) \
			ret += buf; \
		pclose(fp); \
	}; \
	ret; \
})
