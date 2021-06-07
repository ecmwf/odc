First Steps
===========

.. index:: Initialisation

Initialisation
--------------

If **odc** library is included in an environment without `eckit`_ toolkit already present, an initialise function must be called beforehand.

.. tabs::

   .. group-tab:: C

      .. code-block:: c

         #include "odc/api/odc.h"

         int main() {
             odc_initialise_api();
             return 0;
         }

      .. note::

         Make sure to reference the linked library when compiling:

         .. code-block:: shell

            gcc -lodccore odc_test.c


   .. group-tab:: C++

      .. code-block:: cpp

         #include "eckit/runtime/Main.h"
         #include "odc/api/Odb.h"

         using namespace eckit;
         using namespace odc::api;

         int main(int argc, char* argv[]) {
             Main::initialise(argc, argv);
             return 0;
         }

      .. note::

         Make sure to reference the linked libraries when compiling:

         .. code-block:: shell

            g++ -std=c++11 -leckit -lodccore Odb_test.cc


   .. group-tab:: Fortran

      .. code-block:: fortran

         program odc_test
             use odc
             implicit none

             integer :: rc

             rc = odc_initialise_api()
         end program


      .. note::

         Make sure to reference the linked libraries when compiling:

         .. code-block:: shell

            gfortran -lodccore -lfodc odc_test.f90


.. index:: Integer Handling

Integer Handling
----------------

In **odc**, integers can be treated in two ways.

By default, an integer is represented as ``double``. Advantage of storing integers as floating-point numbers is that a wider range of integer types can be supported.

Alternatively, an integer can be represented as ``long``. This can be specified by calling a special function, immediately after the initialisation.

.. tabs::

   .. group-tab:: C

      .. code-block:: c

         odc_integer_behaviour(ODC_INTEGERS_AS_LONGS);


   .. group-tab:: C++

      .. code-block:: cpp

         Settings::treatIntegersAsDoubles(false);


   .. group-tab:: Fortran

      .. code-block:: fortran

         rc = odc_integer_behaviour(ODC_INTEGERS_AS_LONGS)


.. note::

   The only reason why integers are not being represented as ``long`` by default is to maintain backward compatibility with existing tools.


.. index:: Compatible Data, Incompatible Data

Compatible vs Incompatible Data
-------------------------------

Underlying data has patterns and is stored in chunks. This is suitable for archiving by grouping related data and also keeps its size under control.

For different types of data there is a range of appropriate schemas. However, a valid data stream can contain both related and unrelated data.

The data related by its structure is called *compatible*, and the data that is not *incompatible*.

In compatible data the columnar structure of the frames in the underlying ODB-2 data is the same – it contains the same number of columns, with the same names and types, but not necessarily in the same order. The consequence is that these columns can be treated as part of a larger *aggregated frame*. This is highly desirable as decoding multiple real frames within one virtual frame can be parallelised, leading to much better performance.

Incompatible data requires the analysis to be repeated for each block.


.. _`eckit`: https://github.com/ecmwf/eckit
