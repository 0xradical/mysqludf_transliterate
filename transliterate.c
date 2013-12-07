/*
  mysqludf_transliterate - A MySQL UDF to transliterate strings
  Copyright © 2013  Thiago Brandão <thiagobrandam@gmail.com>
  The MIT License (MIT)

  Copyright (c) 2013 Thiago Brandão Damasceno

  Permission is hereby granted, free of charge, to any person obtaining a copy of
  this software and associated documentation files (the "Software"), to deal in
  the Software without restriction, including without limitation the rights to
  use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
  the Software, and to permit persons to whom the Software is furnished to do so,
  subject to the following conditions:

  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
  FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
  COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
  IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
  CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include <ctype.h>
#include <limits.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <my_global.h>
#include <mysql.h>
#include <m_ctype.h>
#include <fcntl.h>
#include <unistd.h>

#include "char_vector.h"
#include "string_utils.h"

/*
#ifdef __WIN__
  #define DLLEXP __declspec(dllexport)
#else
  #define DLLEXP
#endif
*/

#ifdef __GNUC__
  /* From Check:  http://check.svn.sourceforge.net/viewvc/check/trunk/src/check.h.in?revision=HEAD
     License: LGPL 2.1 or later */
  #ifdef __GNUC_MINOR__
    #define GCC_VERSION_AT_LEAST(major, minor) \
      ((__GNUC__ > (major)) || \
      (__GNUC__ == (major) && __GNUC_MINOR__ >= (minor)))
  #else
    #define GCC_VERSION_AT_LEAST(major, minor) 0
  #endif /* __GNUC_MINOR__ */

  #if GCC_VERSION_AT_LEAST(2, 95)
    #define ATTRIBUTE_UNUSED __attribute__ ((unused))
  #else
    #define ATTRIBUTE_UNUSED
  #endif
#else
  #define ATTRIBUTE_UNUSED
#endif

#ifndef SIZE_MAX
  #define SIZE_MAX ((size_t) -1)
#endif

#ifdef HAVE_DLOPEN /* dynamic loading */

  #define LIBVERSION ("mysqludf_transliterate version 0.1")

  /* Function declarations */
  #ifdef  __cplusplus
    extern "C" {
  #endif

  my_bool str_transliterate_init(UDF_INIT *, UDF_ARGS *, char *);
  void str_transliterate_deinit(UDF_INIT *);
  char *str_transliterate(UDF_INIT *, UDF_ARGS *, char *, unsigned long *, char *, char *);

  #ifdef  __cplusplus
    }
  #endif

  /* Function definitions */
  my_bool str_transliterate_init(UDF_INIT *initid, UDF_ARGS *args, char *message)
  {
    st_char_vector *vec;

    /* Check if user provided 1 argument and is of type string */
    if (args->arg_count != 1)
    {
      snprintf(message, MYSQL_ERRMSG_SIZE, "wrong argument count: str_transliterate requires one string argument, got %d arguments", args->arg_count);
      return 1;
    }

    if (args->arg_type[0] != STRING_RESULT)
    {
      snprintf(message, MYSQL_ERRMSG_SIZE, "wrong argument type: str_transliterate requires one string argument. Expected type %d, got type %d.", STRING_RESULT, args->arg_type[0]);
      return 1;
    }

    vec = char_vector_alloc();

    if (vec == NULL)
    {
      x_strlcpy(message, "char_vector_alloc() failed", MYSQL_ERRMSG_SIZE);
      return 1;
    }

    initid->ptr = (char *) vec;

    initid->maybe_null = 1;

    return 0;
  }

  void str_transliterate_deinit(UDF_INIT *initid)
  {
    st_char_vector *vec = (st_char_vector *) initid->ptr;
    char_vector_free(vec);
  }

  #define STR_LENGTH(str) ((sizeof (str)) -1)
  #define STR_COMMA_LENGTH(str_lit) str_lit, STR_LENGTH(str_lit)


  char *str_transliterate(UDF_INIT *initid, UDF_ARGS *args,
        char *result, unsigned long *res_length,
        char *null_value, char *error)
  {

    st_char_vector *vec;

    char *original;

    /* int part_stack[14];
    int *part_ptr; */

    if (args->args[0] == NULL) {
      result = NULL;
      *res_length = 0;
      *null_value = 1;
      return result;
    }

    vec = (st_char_vector *) initid->ptr;

    original = *((char **) args->args[0]);

    // part_ptr = part_stack;

    // assert(char_vector_length(vec) == 0);

    // // check for negative values or zero
    // if (value < 0)
    // {
    //   char_vector_append(vec, STR_COMMA_LENGTH("negative "));
    //   value = -value;
    // }
    // else if (value == 0)
    // {
    //   char *tmp = "zero";
    //   *res_length = STR_LENGTH("zero");
    //   return tmp;
    // }

    // // splitting the number into its parts
    // for (; value; value /= 1000)
    //   *part_ptr++ = value % 1000;

    // while (part_ptr > part_stack)
    // {
    //   int p = *--part_ptr;
    //   const int pWasNonzero = p != 0;

    //   if (p >= 100)
    //   {
    //     char_vector_strcat(vec, ones[p / 100 - 1]);
    //     char_vector_append(vec, STR_COMMA_LENGTH(" hundred "));
    //     p %= 100;
    //   }

    //   if (p >= 20)
    //   {
    //     if (p % 10)
    //     {
    //       char_vector_strcat(vec, tens[p / 10 - 2]);
    //       char_vector_append(vec, STR_COMMA_LENGTH("-"));
    //       char_vector_strcat(vec, ones[p % 10 - 1]);
    //       char_vector_append(vec, STR_COMMA_LENGTH(" "));
    //     }
    //     else
    //     {
    //       char_vector_strcat(vec, tens[p / 10 - 2]);
    //       char_vector_append(vec, STR_COMMA_LENGTH(" "));
    //     }
    //   }
    //   else if (p > 0)
    //   {
    //     char_vector_strcat(vec, ones[p - 1]);
    //     char_vector_append(vec, STR_COMMA_LENGTH(" "));
    //   }

    //   if (pWasNonzero && part_ptr > part_stack)
    //   {
    //     char_vector_strcat(vec, powers[part_ptr - part_stack - 1]);
    //     char_vector_append(vec, STR_COMMA_LENGTH(" "));
    //   }
    // }

    // *res_length = char_vector_length(vec) - 1;
    *res_length = 2;
    // return char_vector_get_ptr(vec);
    return original;
  }

#endif /* HAVE_DLOPEN */
