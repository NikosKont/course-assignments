#include <stddef.h>
#include <stdio.h>
#include <assert.h>

/* Nikolaos Kontogeorgis - 4655
	HY255 - Assignment 2
	mystring.h */


/* Returns length of string str.
	There is a checked runtime error for str to be NULL. */
size_t ms_length(const char str[]);


/* Copies string source to string dest
	Returns a pointer to string dest
	There is a checked runtime error for source and dest to be NULL. */
char *ms_copy(char dest[], const char source[]);


/* Copies max n characters from string source to string dest
	Returns a pointer to string dest
	There is a checked runtime error for source, dest and n to be NULL. */
char *ms_ncopy(char dest[], const char source[], size_t n);


/* Copies string source to the end of string dest
	Returns a pointer to string dest
	There is a checked runtime error for source and dest to be NULL. */
char *ms_concat(char dest[], const char source[]);


/* Copies max n characters from string source to the end of string dest
	Returns a pointer to string dest
	There is a checked runtime error for source, dest and n to be NULL. */
char *ms_nconcat(char dest[], const char source[], size_t n);


/* Returns 0 if str1 and str2 are equal,
	-1 if str2 is 'greater', 1 if str1 is 'greater'
	There is a checked runtime error for str1 and str2 to be NULL. */
int ms_compare(const char str1[], const char str2[]);


/* Returns 0 if first n characters of str1 and str2 are equal,
	-1 if first n characters of str2 are 'greater',
	 1 if first n characters of str1 are 'greater'
	There is a checked runtime error for str1, str2 and n to be NULL. */
int ms_ncompare(const char str1[], const char str2[], size_t n);


/* Searches for first occurence of string substr in string str
	Returns pointer to the first occurence or a NULL pointer if there is no occurence
	There is a checked runtime error for str and substr to be NULL. */
char *ms_search(char str[], const char substr[]);