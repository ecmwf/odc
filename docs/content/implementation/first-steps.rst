First Steps
===========

.. index:: Initialisation

Initialisation
--------------

The **odc** library makes heavy internal use of the provisions of the eckit library. As such, eckit must be properly initialised.

If **odc** is being used in a context where `eckit`_ is not already being used, it is necessary to call an initialisation function before **odc** is used.

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
         #include "odb/api/Odb.h"

         // We use the namespace to simplify the examples later in this documentation
         using namespace odc::api;

         int main(int argc, char* argv[]) {
             eckit::Main::initialise(argc, argv);
             return 0;
         }

      .. note::

         Make sure to reference the linked libraries when compiling:

         .. code-block:: shell

            g++ -std=c++11 -lodccore odc_test.cc


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

            gfortran -lfodc odc_test.f90


.. index:: Integer Handling

Integer Handling
----------------

In the **odc** API interface integers can be treated in two different ways.

 * By default, an integer is represented as a 64bit floating point number (a ``double``).

 * Alternatively, an integer can be represented as a 64-bit signed integer (a ``long``).

The integer-handling behaviour can be specified by calling a special function immediately after initialisation.

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

   The only reason why integers are not being represented as 64-bit integers by default is to maintain backward compatibility with existing tools.

.. note::

   The default integer-handling behaviour will change in a future release. It is highly recommended to be explicit about the integer-handling behaviour desired in any application.


.. index:: Compatible Data, Incompatible Data

Compatible and Incompatible Data
-------------------------------

A stream of ODB-2 data comprises a sequence of Frames. These frames may be related to each other, or not, and they may or may not have the same columnar structure.

When two frames have the same columnar structure, we call them *compatible*. Otherwise the frames are *incompatible*.

In compatible data the columnar structure of the frames in the underlying ODB-2 data is logically the same – it contains the same number of columns, with the same names and types, albeit not necessarily in the same order. Within the **odc** library we can treat a contiguous group of these compatible frames as a larger *aggregated frame*. This can have performance benefits during decoding, as the real frames can be decoded in parallel within one logical frame.

Incompatible data requires the calling code to treat each frame separately.


.. _`eckit`: https://github.com/ecmwf/eckit
