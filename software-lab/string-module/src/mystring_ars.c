#include "mystring.h"

/*  Nikolaos Kontogeorgis - 4655
    HY255 - Assignment 2
    mystring_ars.c  */

/*  Returns length of string str.
    There is a checked runtime error for str to be NULL.  */
size_t ms_length(const char str[]) {
    size_t length = 0U;
    assert(str);            /* Works because NULL and FALSE are identical. */

    while (str[length])     /* Works because end-of-string and FALSE are identical. */
        length++;

    return length;
}

/*  Copies string source to string dest
    Returns a pointer to string dest
    There is a checked runtime error for source and dest to be NULL.  */
char *ms_copy(char dest[], const char source[]) {
    size_t i, l = ms_length(source);
    assert(source);
    assert(dest);

    for (i = 0U; i < l; i++)
        dest[i] = source[i];

    return dest;
}

/*  Copies max n characters from string source to string dest
    Returns a pointer to string dest
    There is a checked runtime error for source, dest and n to be NULL.  */
char *ms_ncopy(char dest[], const char source[], size_t n) {
    size_t i, l = ms_length(source);
    assert(source);
    assert(dest);

    if (l < n) {
        for (i = l; i < n; i++)
            dest[i] = '\0';
        n = l;
    }

    for (i = 0U; i < n; i++)
        dest[i] = source[i];

    return dest;
}

/*  Copies string source to the end of string dest
    Returns a pointer to string dest
    There is a checked runtime error for source and dest to be NULL.  */
char *ms_concat(char dest[], const char source[]) {
    size_t l1 = ms_length(dest), l2 = ms_length(source), i;
    assert(source);
    assert(dest);

    for (i = l1; i < l2 + l1; i++)
        dest[i] = source[i - l1];
    dest[l2 + l1] = '\0';

    return dest;
}

/*  Copies max n characters from string source to the end of string dest
    Returns a pointer to string dest
    There is a checked runtime error for source, dest and n to be NULL.  */
char *ms_nconcat(char dest[], const char source[], size_t n) {
    size_t l1 = ms_length(dest), l2 = ms_length(source), i;
    assert(source);
    assert(dest);

    if (n < l2)
        l2 = n;

    for (i = l1; i < l2 + l1; i++) {
        dest[i] = source[i - l1];
    }
    dest[l2 + l1] = '\0';

    return dest;
}

/*  Returns 0 if str1 and str2 are equal,
    -1 if str2 is 'greater', 1 if str1 is 'greater'
    There is a checked runtime error for str1 and str2 to be NULL.  */
int ms_compare(const char str1[], const char str2[]) {
    size_t i;
    int flag = 0;
    assert(str1);
    assert(str2);

    for (i = 0U; !flag; i++) {
        if (str1[i] < str2[i] || !str1[i])
            flag = -1;
        else if (str1[i] > str2[i] || !str2[i])
            flag = 1;
        if (flag == -1 && str1[i] == str2[i])
            return 0;
    }
    return flag;
}

/*  Returns 0 if first n characters of str1 and str2 are equal,
    -1 if first n characters of str2 are 'greater',
    1 if first n characters of str1 are 'greater'
    There is a checked runtime error for str1, str2 and n to be NULL.  */
int ms_ncompare(const char str1[], const char str2[], size_t n) {
    size_t i;
    int flag = 0;
    assert(str1);
    assert(str2);

    for (i = 0U; !flag && i < n; i++) {
        if (str1[i] < str2[i] || !str1[i])
            flag = -1;
        else if (str1[i] > str2[i] || !str2[i])
            flag = 1;
        if (flag == -1 && str1[i] == str2[i])
            return 0;
    }
    return flag;
}

/*  Searches for first occurence of string substr in string str
    Returns pointer to the first occurence or a NULL pointer if there is no occurence
    There is a checked runtime error for str and substr to be NULL.  */
char *ms_search(char str[], const char substr[]) {
    size_t i, j, l1, l2;
    int flag;
    assert(str);
    assert(substr);

    l1 = ms_length(str);
    l2 = ms_length(substr);

    for (i = 0U; i < l1 - l2; i++) {
        flag = 0;
        if (str[i] == substr[0]) {
            for (j = 1U; j < l2 && !flag; j++)
                if (str[i + j] != substr[j])
                    flag = 1;

            if (flag == 0)
                return &(str[i]);
        }
    }

    return NULL;
}