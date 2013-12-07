mysqludf_transliterate
======================

A MySQL UDF for transliteration of strings

Introduction
------------

This is a very naïve solution to a very hard problem: transliteration of strings. The hard problem is dealing
with different encodings on the programming side. As of today,
I mostly deal with strings that are either encoded with
UTF-8 or ISO-8859-1 (latin1), since values of entities in my dbs are represented using Brazilian Portuguese. So the mapping I do in the code works fine for all cases in my domain.


Installation instructions
------------------------

  The compilation step works on my Mac OS X (Mountain Lion), it should work on yours too, provided you have all it takes to compile MySQL UDFs. You may have to fiddle with the pre-processing directives to make it work on other platforms.

  To compile:

      $ gcc $(mysql_config --cflags) -shared -fPIC -o transliterate.so transliterate.c


  This will create a .so file. Now copy this file into the MySQL plugins folder:

      $ cp transliterate.so /usr/local/mysql/lib/plugin

  This folder differs from host to host, to know where yours live,
  you can execute this query:

      SHOW VARIABLES LIKE 'plugin_dir';

  Finally, run these two queries to install the UDF:

      DROP FUNCTION IF EXISTS transliterate;
      CREATE FUNCTION transliterate RETURNS STRING SONAME "transliterate.so";


Using the function
------------------

  The transliterate function takes one, and only one argument, of type string (either constant or non-constant).

  Example:

      SELECT name, transliterate(name) AS trans_name FROM courses;

  Should return something like this:

      +--------------------------+--------------------------+
      | name                     | trans_name               |
      +--------------------------+--------------------------+
      | Turismo                  | Turismo                  |
      | Teatro                   | Teatro                   |
      | Sistemas para Internet   | Sistemas para Internet   |
      | Sistemas de Internet     | Sistemas de Internet     |
      | Sistemas de Informação   | Sistemas de Informacao   |
      | Serviço Social           | Servico Social           |
      | Segurança do Trabalho    | Seguranca do Trabalho    |
      | Redes de Computadores    | Redes de Computadores    |
      | Publicidade e Propaganda | Publicidade e Propaganda |
      | Psicologia               | Psicologia               |
      +--------------------------+--------------------------+

License
-------

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
