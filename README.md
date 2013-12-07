mysqludf_transliterate
======================

A MySQL UDF for transliteration of strings

Instalation instructions
------------------------

      $ gcc $(mysql_config --cflags) -shared -fPIC -o transliterate.so transliterate.c char_vector.c string_utils.c


  This will create a .so file. Now copy this file into plugins:

      $ cp transliterate.so /usr/local/mysql/lib/plugin

  In MySQL, run this query:

      CREATE FUNCTION str_transliterate
      RETURNS STRING
      SONAME "transliterate.so";