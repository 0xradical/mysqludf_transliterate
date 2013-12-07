// This code and all comments, written by Daniel Trebbien, are hereby entered into the Public Domain by their author.

#pragma once
#ifndef LIB_MYSQLUDF_STR_STRING_UTILS_H
  #define LIB_MYSQLUDF_STR_STRING_UTILS_H 1
  #include <stddef.h>

  #ifdef __cplusplus
    extern "C" {
  #endif

  /**
   * Copies the string pointed to by \p src to the output buffer pointed to by \p dest, which has
   * space for \p dest_len bytes. If \p dest_len is 0, then the function simply returns
   * <code>strlen(src)</code>. Otherwise, at most <code>dest_len - 1</code> characters are
   * copied and \p dest is always NUL-terminated.
   *
   * \returns the length of the string pointed to by \p src. If the return value is greater than
   *    or equal to \p dest_len, then truncation occurred.
   */
  size_t x_strlcpy(char *__restrict dest, const char *__restrict src, size_t dest_len);

  #ifdef __cplusplus
  }
  #endif
#endif
