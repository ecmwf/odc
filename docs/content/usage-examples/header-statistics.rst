Frame Headers
=============

Shown below is a simple program that outputs information extracted from the header of each frame in an ODB-2 data stream, without actually decoding the data.

.. tabs::

   .. group-tab:: C

      .. literalinclude:: ../../../tests/c_api/odc_header.c
         :language: c
         :class: copybutton


      To use this sample program, invoke it from the command line with a path to one or more ODB-2 files:

      .. code-block:: none

         ./odc-c-header example.odb span.odb

         File: example.odb
           Frame: 1, Row count: 3, Column count: 5
           Property: encoder => odc version 1.3.0
             Column: 1, Name: int_column, Type: integer, Size: 8
             Column: 2, Name: real_column, Type: real, Size: 8
             Column: 3, Name: str8_column, Type: string, Size: 8
             Column: 4, Name: str16_column, Type: string, Size: 16
             Column: 5, Name: bitfield_column, Type: bitfield, Size: 8
               Bitfield: 1, Name: flag_a, Offset: 0, Nbits: 1
               Bitfield: 2, Name: flag_b, Offset: 1, Nbits: 2
               Bitfield: 3, Name: flag_c, Offset: 3, Nbits: 3
               Bitfield: 4, Name: flag_d, Offset: 6, Nbits: 1

         File: span.odb
           Frame: 1, Row count: 3, Column count: 4
           Property: encoder => odc version 1.3.0
             Column: 1, Name: key1, Type: integer, Size: 8
             Column: 2, Name: key2, Type: integer, Size: 8
             Column: 3, Name: key3, Type: string, Size: 8
             Column: 4, Name: val1, Type: integer, Size: 8

           Frame: 2, Row count: 3, Column count: 4
           Property: encoder => odc version 1.3.0
             Column: 1, Name: key1, Type: integer, Size: 8
             Column: 2, Name: key2, Type: integer, Size: 8
             Column: 3, Name: key3, Type: string, Size: 8
             Column: 4, Name: val1, Type: integer, Size: 8

           Frame: 3, Row count: 3, Column count: 5
           Property: encoder => odc version 1.3.0
             Column: 1, Name: key1, Type: integer, Size: 8
             Column: 2, Name: key2, Type: integer, Size: 8
             Column: 3, Name: key3, Type: string, Size: 8
             Column: 4, Name: val1, Type: integer, Size: 8
             Column: 5, Name: val2, Type: real, Size: 8


   .. group-tab:: C++

      .. literalinclude:: ../../../tests/api/odc_header.cc
         :language: cpp
         :class: copybutton


      To use this sample program, invoke it from the command line with paths to one or more ODB-2 files:

      .. code-block:: none

         ./odc-cpp-header example.odb span.odb

         File: example.odb
           Frame: 1, Row count: 3, Column count: 5
           Property: encoder => odc version 1.3.0
             Column: 1, Name: int_column, Type: integer, Size: 8
             Column: 2, Name: real_column, Type: real, Size: 8
             Column: 3, Name: str8_column, Type: string, Size: 8
             Column: 4, Name: str16_column, Type: string, Size: 16
             Column: 5, Name: bitfield_column, Type: bitfield, Size: 8
               Bitfield: 1, Name: flag_a, Offset: 0, Nbits: 1
               Bitfield: 2, Name: flag_b, Offset: 1, Nbits: 2
               Bitfield: 3, Name: flag_c, Offset: 3, Nbits: 3
               Bitfield: 4, Name: flag_d, Offset: 6, Nbits: 1

         File: span.odb
           Frame: 1, Row count: 3, Column count: 4
           Property: encoder => odc version 1.3.0
             Column: 1, Name: key1, Type: integer, Size: 8
             Column: 2, Name: key2, Type: integer, Size: 8
             Column: 3, Name: key3, Type: string, Size: 8
             Column: 4, Name: val1, Type: integer, Size: 8

           Frame: 2, Row count: 3, Column count: 4
           Property: encoder => odc version 1.3.0
             Column: 1, Name: key1, Type: integer, Size: 8
             Column: 2, Name: key2, Type: integer, Size: 8
             Column: 3, Name: key3, Type: string, Size: 8
             Column: 4, Name: val1, Type: integer, Size: 8

           Frame: 3, Row count: 3, Column count: 5
           Property: encoder => odc version 1.3.0
             Column: 1, Name: key1, Type: integer, Size: 8
             Column: 2, Name: key2, Type: integer, Size: 8
             Column: 3, Name: key3, Type: string, Size: 8
             Column: 4, Name: val1, Type: integer, Size: 8
             Column: 5, Name: val2, Type: real, Size: 8


   .. group-tab:: Fortran

      .. literalinclude:: ../../../tests/f_api/odc_header.f90
         :language: fortran
         :class: copybutton


      To use this sample program, invoke it from the command line with paths to one or more ODB-2 files:

      .. code-block:: none

         ./odc-fortran-header example.odb span.odb

         File: example.odb
           Frame: 1, Row count: 3, Column count: 5
           Property: encoder => odc version 1.3.0
             Column: 1, Name: int_column, Type: integer, Size: 8
             Column: 2, Name: real_column, Type: real, Size: 8
             Column: 3, Name: str8_column, Type: string, Size: 8
             Column: 4, Name: str16_column, Type: string, Size: 16
             Column: 5, Name: bitfield_column, Type: bitfield, Size: 8
               Bitfield: 1, Name: flag_a, Offset: 0, Nbits: 1
               Bitfield: 2, Name: flag_b, Offset: 1, Nbits: 2
               Bitfield: 3, Name: flag_c, Offset: 3, Nbits: 3
               Bitfield: 4, Name: flag_d, Offset: 6, Nbits: 1

         File: span.odb
           Frame: 1, Row count: 3, Column count: 4
           Property: encoder => odc version 1.3.0
             Column: 1, Name: key1, Type: integer, Size: 8
             Column: 2, Name: key2, Type: integer, Size: 8
             Column: 3, Name: key3, Type: string, Size: 8
             Column: 4, Name: val1, Type: integer, Size: 8

           Frame: 2, Row count: 3, Column count: 4
           Property: encoder => odc version 1.3.0
             Column: 1, Name: key1, Type: integer, Size: 8
             Column: 2, Name: key2, Type: integer, Size: 8
             Column: 3, Name: key3, Type: string, Size: 8
             Column: 4, Name: val1, Type: integer, Size: 8

           Frame: 3, Row count: 3, Column count: 5
           Property: encoder => odc version 1.3.0
             Column: 1, Name: key1, Type: integer, Size: 8
             Column: 2, Name: key2, Type: integer, Size: 8
             Column: 3, Name: key3, Type: string, Size: 8
             Column: 4, Name: val1, Type: integer, Size: 8
             Column: 5, Name: val2, Type: real, Size: 8
