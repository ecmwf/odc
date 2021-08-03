Data Listing
============

Shown below is a simple program that decodes all the data in an ODB-2 file and outputs it to stdout.

.. tabs::

   .. group-tab:: C

      .. literalinclude:: ../../../tests/c_api/odc_ls.c
         :language: c
         :class: copybutton


      To use this sample program, invoke it from the command line with a path to an ODB-2 file:

      .. code-block:: none

         ./odc-c-ls example.odb

         1. int_column	2. real_column	3. str8_column	4. str16_column      	5. bitfield_column
         1       	1.200000	abcdefgh	abcdefghijklmnop	0000001
         2       	3.400000	01234567	0123456789012345	0001011
         3       	5.600000	string  	another string  	1101011


   .. group-tab:: C++

      .. literalinclude:: ../../../tests/api/odc_ls.cc
         :language: cpp
         :class: copybutton


      To use this sample program, invoke it from the command line with a path to an ODB-2 file:

      .. code-block:: none

         ./odc-cpp-ls example.odb

         1. int_column	2. real_column	3. str8_column	4. str16_column      	5. bitfield_column
         1       	1.2     	abcdefgh	abcdefghijklmnop	0000001
         2       	3.4     	01234567	0123456789012345	0001011
         3       	5.6     	string  	another string  	1101011

   .. group-tab:: Fortran

      .. literalinclude:: ../../../tests/f_api/odc_ls.f90
         :language: fortran
         :class: copybutton


      To use this sample program, invoke it from the command line with a path to an ODB-2 file:

      .. code-block:: none

         ./odc-fortran-ls example.odb

         1. int_column	2. real_column	3. str8_column	4. str16_column		5. bitfield_column
         1		1.2000		abcdefgh	abcdefghijklmnop	0000001
         2		3.4000		01234567	0123456789012345	0001011
         3		5.6000		string  	another string  	1101011
