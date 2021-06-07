Archival Index
==============

Shown below is a simple program written in C++ that generates archival index from an ODB-2 file without decoding underlying data.

.. note::

   C and Fortran interface do not support a viewport into frame data without decoding it. In this case, recommended API is C++.


.. tabs::

   .. group-tab:: C++

      .. literalinclude:: ../../../tests/api/odc_index.cc
         :language: cpp
         :class: copybutton


      To use this sample program, invoke it from the command line with a path to an ODB-2 output file with constant data:

      .. code-block:: none

         ./odc-cpp-index span.odb

         Archival unit: offset=0 length=346
           Key: key1=1 key2=100 key3=foo
         Archival unit: offset=346 length=346
           Key: key1=2 key2=200 key3=bar
         Archival unit: offset=692 length=398
           Key: key1=3 key2=300 key3=baz
