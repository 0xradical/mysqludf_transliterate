// This code and all comments, written by Daniel Trebbien, are hereby entered into the Public Domain by their author.

#pragma once
#ifndef LIB_MYSQLUDF_STR_CHAR_VECTOR_H
  #define LIB_MYSQLUDF_STR_CHAR_VECTOR_H 1
  #include <stddef.h>

  #ifdef __cplusplus
    extern "C" {
  #endif

  typedef struct st_char_vector st_char_vector;

  /** Allocate a new char vector.
   *
   * @returns a non-NULL pointer to a char vector if successful. This must be freed with char_vector_free().*/
  st_char_vector *char_vector_alloc();

  /** Destroys @p vec, freeing all memory that it has allocated.
   *
   * This function invalidates all iterators. */
  void char_vector_free(st_char_vector *vec);

  /** Obtains the current length of @p vec */
  size_t char_vector_length(const st_char_vector *vec);

  /** Obtains the internal pointer of @p vec.
   *
   * This pointer @c p and <code>p + 1</code>, ..., <code>p + char_vector_length(vec) - 1</code> are called <em>iterators</em>,
   * and they are mutable. */
  char* char_vector_get_ptr(st_char_vector *vec);

  /** Appends the first @p str_length chars of @p str to @p vec.
   *
   * This function invalidates all iterators.
   *
   * @returns 0 if there was no error. Otherwise, a non-zero error code is returned and @p vec is unaffected. */
  int char_vector_append(st_char_vector *vec, const char *str, size_t str_length);

  /** Appends the NUL-terminated string @p str to @p vec.
   *
   * This function invalidates all iterators.
   *
   * @returns 0 if there was no error. Otherwise, a non-zero error code is returned and @p vec is unaffected. */
  int char_vector_strcat(st_char_vector *vec, const char *str);

  #ifdef __cplusplus
    }
  #endif

#endif
