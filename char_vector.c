// This code and all comments, written by Daniel Trebbien, are hereby entered into the Public Domain by their author.

#include <assert.h>
#include <errno.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#ifndef SIZE_MAX
  #define SIZE_MAX ((size_t) -1)
#endif

#undef MAX
#define MAX(a, b) ((a) > (b) ? (a) : (b))

#include "char_vector.h"

struct st_char_vector
{
  char *buf;
  size_t vec_capacity;
  size_t vec_length;
};

st_char_vector *char_vector_alloc()
{
  st_char_vector *vec = (st_char_vector *) malloc(sizeof (st_char_vector));

  if (vec == NULL)
    return NULL;

  vec->vec_capacity = 255;

  vec->buf = (char *) malloc(vec->vec_capacity);
  if (vec->buf == NULL)
  {
    free(vec);
    return NULL;
  }

  vec->vec_length = 0;

  return vec;
}

void char_vector_free(st_char_vector *vec)
{
  assert(vec->buf != NULL);
  free(vec->buf);
#ifndef NDEBUG
  vec->buf = NULL;
#endif
  free(vec);
}

size_t char_vector_length(const st_char_vector *vec)
{
  return vec->vec_length;
}

char* char_vector_get_ptr(st_char_vector *vec)
{
  return vec->buf;
}

int char_vector_append(st_char_vector *vec, const char *str, size_t str_length)
{
  if (SIZE_MAX - MAX(255, str_length) < vec->vec_capacity)
  {
    return E2BIG;
  }

  if (vec->vec_capacity < (vec->vec_length + str_length))
  {
    // Need to allocate more space
    size_t new_capacity = vec->vec_capacity + MAX(255, str_length);
    char *tmp = (char *) malloc(new_capacity);
    if (tmp == NULL)
    {
      return ENOMEM;
    }

    memcpy(tmp, vec->buf, vec->vec_length);

    vec->buf = tmp;
    vec->vec_capacity = new_capacity;
  }

  memmove(vec->buf + vec->vec_length, str, str_length); // Use memmove() because str may be a pointer within [vec->buf, vec->buf + vec->vec_length)
  vec->vec_length += str_length;

  return 0;
}

int char_vector_strcat(st_char_vector *vec, const char *str)
{
  return char_vector_append(vec, str, strlen(str));
}
