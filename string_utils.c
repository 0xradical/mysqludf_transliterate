/* This code and all comments, written by Daniel Trebbien, are hereby entered into the Public Domain by their author. */

#include <string.h>
#include "string_utils.h"

/* This implementation is machine-verified:
   https://github.com/dtrebbien/verlibc/blob/daea83474b36dc251751d100d2ba347d1aa906d6/src/string/strlcpy.c */
size_t x_strlcpy(char *__restrict dest, const char *__restrict src, size_t dest_len)
{
  if (dest_len == 0) {
    return strlen(src);
  } else {
    char *const dest_str_end = dest + dest_len - 1;
    char *__restrict d = dest;
    const char *__restrict s = src;

    while (d != dest_str_end) {
      if ((*d++ = *s++) == '\0') {
        return (d - 1 - dest);
      }
    }

    *d = '\0';
    return (dest_len - 1) + strlen(s);
  }
}
