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

#ifdef HAVE_DLOPEN /* dynamic loading */

  #define LIBVERSION ("mysqludf_transliterate version 0.1")

  /* Function declarations */
  my_bool transliterate_init(UDF_INIT *, UDF_ARGS *, char *);
  void transliterate_deinit(UDF_INIT *);
  char *transliterate(UDF_INIT *, UDF_ARGS *, char *, unsigned long *, char *, char *);

  /* Function definitions */
  my_bool transliterate_init(UDF_INIT *initid, UDF_ARGS *args, char *message)
  {

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

    return 0;
  }

  void transliterate_deinit(UDF_INIT *initid)
  {
  }

  char *transliterate(UDF_INIT *initid, UDF_ARGS *args,
        char *result, unsigned long *length,
        char *null_value, char *error)
  {

    unsigned char *original = args->args[0];
    unsigned char *transliterated = (unsigned char *)(malloc(sizeof(unsigned char) * args->lengths[0]));
    int original_counter = 0, transliterated_counter = 0;

    if(original == NULL)
    {
      result = NULL;
      *length = 0;
      *null_value = 1;
      return result;
    }

    for(; original_counter < args->lengths[0]; original_counter++){
      if(original[original_counter] > 0x7F)
      {
        /* UTF-8 NFC */
        if(original[original_counter] == 0xC3)
        {

          if(original[original_counter+1] >= 0xA0 && original[original_counter+1] <= 0xA3)
          {
            transliterated[transliterated_counter] = 'a';
          }
          else if(original[original_counter+1] >= 0x80 && original[original_counter+1] <= 0x83)
          {
            transliterated[transliterated_counter] = 'A';
          }
          else if(original[original_counter+1] >= 0xA9 && original[original_counter+1] <= 0xAA)
          {
            transliterated[transliterated_counter] = 'e';
          }
          else if(original[original_counter+1] >= 0x89 && original[original_counter+1] <= 0x8A)
          {
            transliterated[transliterated_counter] = 'E';
          }
          else if(original[original_counter+1] == 0xAD)
          {
            transliterated[transliterated_counter] = 'i';
          }
          else if(original[original_counter+1] == 0x8D)
          {
            transliterated[transliterated_counter] = 'I';
          }
          else if(original[original_counter+1] >= 0xB3 && original[original_counter+1] <= 0xB5)
          {
            transliterated[transliterated_counter] = 'o';
          }
          else if(original[original_counter+1] >= 0x93 && original[original_counter+1] <= 0x95)
          {
            transliterated[transliterated_counter] = 'O';
          }
          else if(original[original_counter+1] >= 0xBA && original[original_counter+1] <= 0xBC)
          {
            transliterated[transliterated_counter] = 'u';
          }
          else if(original[original_counter+1] >= 0x9A && original[original_counter+1] <= 0x9C)
          {
            transliterated[transliterated_counter] = 'U';
          }
          else if(original[original_counter+1] == 0xA7)
          {
            transliterated[transliterated_counter] = 'c';
          }
          else if(original[original_counter+1] == 0x87)
          {
            transliterated[transliterated_counter] = 'c';
          }
          else
          {
            transliterated[transliterated_counter] = '?';
          }

          transliterated_counter += 1;
          original_counter += 1;
        }
        /* UTF-8 NFD: easier, just test if original[original_counter+1] == 0xCC */
        else if (original[original_counter+1] == 0xCC)
        {
          transliterated[transliterated_counter] = original[original_counter];
          transliterated_counter += 1;
          original_counter += 2; /* á = a + 0xCC + acute accent */
        }
        /* It's probably ISO-8859-1 */
        else if (original[original_counter] >= 0xC0)
        {

          if(original[original_counter] >= 0xE0 && original[original_counter] <= 0xE6)
          {
            transliterated[transliterated_counter] = 'a';
          }
          else if(original[original_counter] >= 0xC0 && original[original_counter] <= 0xC6)
          {
            transliterated[transliterated_counter] = 'A';
          }
          else if(original[original_counter] >= 0xE8 && original[original_counter] <= 0xEB)
          {
            transliterated[transliterated_counter] = 'e';
          }
          else if(original[original_counter] >= 0xC8 && original[original_counter] <= 0xCB)
          {
            transliterated[transliterated_counter] = 'E';
          }
          else if(original[original_counter] == 0xED)
          {
            transliterated[transliterated_counter] = 'i';
          }
          else if(original[original_counter] == 0xCD)
          {
            transliterated[transliterated_counter] = 'I';
          }
          else if(original[original_counter] >= 0xF2 && original[original_counter] <= 0xF6)
          {
            transliterated[transliterated_counter] = 'o';
          }
          else if(original[original_counter] >= 0xD2 && original[original_counter] <= 0xD6)
          {
            transliterated[transliterated_counter] = 'O';
          }
          else if(original[original_counter] >= 0xF9 && original[original_counter] <= 0xFC)
          {
            transliterated[transliterated_counter] = 'u';
          }
          else if(original[original_counter] >= 0xD9 && original[original_counter] <= 0xDC)
          {
            transliterated[transliterated_counter] = 'U';
          }
          else if(original[original_counter] == 0xE7)
          {
            transliterated[transliterated_counter] = 'c';
          }
          else if(original[original_counter] == 0xC7)
          {
            transliterated[transliterated_counter] = 'c';
          }
          else
          {
            transliterated[transliterated_counter] = '?';
          }

          transliterated_counter += 1;

        }
        else
        {
          transliterated[transliterated_counter] = original[original_counter];
          transliterated_counter += 1;
        }
      }
      else
      {
        transliterated[transliterated_counter] = original[original_counter];
        transliterated_counter += 1;
      }
    }

    memcpy(result, transliterated, transliterated_counter);
    *length = transliterated_counter;
    free(transliterated);

    return result;
  }

#endif /* HAVE_DLOPEN */
