#include "mystring.h"

/*  Nikolaos Kontogeorgis - 4655
	HY255 - Assignment 2
	mystring_pts.c  */

/*	Returns length of string str.
	There is a checked runtime error for str to be NULL.  */
size_t ms_length(const char *str) {
	const char *strEnd = str;
	assert(str); 			/* Works because NULL and FALSE are identical. */

	while (*strEnd) 		/* Works because end-of-string and FALSE are identical. */
		strEnd++;

	return strEnd - str;
}

/*	Copies string source to string dest
	Returns a pointer to string dest
	There is a checked runtime error for source and dest to be NULL.  */
char *ms_copy(char *dest, const char *source) {
	char *start = dest;
	assert(source);
	assert(dest);

	do {
		*dest++ = *source;
	} while (*source++);

	return start;
}

/*	Copies max n characters from string source to string dest
	Returns a pointer to string dest
	There is a checked runtime error for source, dest and n to be NULL.  */
char *ms_ncopy(char *dest, const char *source, size_t n) {
	char *start = dest;
	size_t i = 0U;
	assert(source);
	assert(dest);

	while (*source && i++ < n)
		*dest++ = *source++;
	while (i++ < n)
		*dest++ = '\0';

	return start;
}

/*  Copies string source to the end of string dest
	Returns a pointer to string dest
	There is a checked runtime error for source and dest to be NULL.  */
char *ms_concat(char *dest, const char *source) {
	char *start = dest;
	assert(source);
	assert(dest);

	while (*dest++);

	ms_copy(dest - 1, source);

	return start;
}

/*  Copies max n characters from string source to the end of string dest
	Returns a pointer to string dest
	There is a checked runtime error for source, dest and n to be NULL.  */
char *ms_nconcat(char *dest, const char *source, size_t n) {
	char *start = dest;
	assert(source);
	assert(dest);

	while (*dest++);

	ms_ncopy(dest - 1, source, n);

	return start;
}

/*  Returns 0 if str1 and str2 are equal,
	-1 if str2 is 'greater', 1 if str1 is 'greater'
	There is a checked runtime error for str1 and str2 to be NULL.  */
int ms_compare(const char *str1, const char *str2) {
	int flag = 0;
	assert(str1);
	assert(str2);

	do {
		if (*str1 < *str2 || !*str1)
			flag = -1;
		else if (*str1 > *str2 || !*str2)
			flag = 1;
		if (*str1 == *str2 && flag == -1)
			return 0;
	} while (*str1++ == *str2++ && (!flag));

	return flag;
}

/*  Returns 0 if first n characters of str1 and str2 are equal,
	-1 if first n characters of str2 are 'greater',
	1 if first n characters of str1 are 'greater'
	There is a checked runtime error for str1, str2 and n to be NULL.  */
int ms_ncompare(const char *str1, const char *str2, size_t n) {
	size_t i = 0U;
	int flag = 0;
	assert(str1);
	assert(str2);

	if (n == 0U)
		return 0;

	do {
		if (*str1 < *str2 || !*str1)
			flag = -1;
		else if (*str1 > *str2 || !*str2)
			flag = 1;
		if (*str1 == *str2 && flag == -1)
			return 0;
	} while (*str1++ == *str2++ && i++ < n && (!flag));

	return flag;
}

/*  Searches for first occurence of string substr in string str
	Returns pointer to the first occurence or a NULL pointer if there is no occurence
	There is a checked runtime error for str and substr to be NULL.  */
char *ms_search(char *str, const char *substr) {
	char *start = str, *curr;
	assert(str);
	assert(substr);

	while (*start++) {
		if ((*start) == (*substr)) {
			str = start;
			curr = substr;
			while (*str++ == *curr++) {
				if (!(*curr))
					return start;
			}
		}
	}

	return NULL;
}