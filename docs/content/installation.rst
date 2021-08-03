Installation
============

.. index:: Dependencies

Dependencies
------------

.. index::
   single: Dependencies; Required

Required
~~~~~~~~

* C or C++ compiler
* `CMake`_
* `ecbuild`_
* `eckit`_


.. index:: Dependencies; Optional

Optional
~~~~~~~~

* Fortran compiler
* `Doxygen`_


.. index:: Build, Install
   :name: build-install

Build & Install
---------------

.. code-block:: shell

   git clone https://github.com/ecmwf/odc
   cd odc

   # Setup environment variables (edit as needed)
   SRC_DIR=$(pwd)
   BUILD_DIR=build
   INSTALL_DIR=$HOME/local
   export eckit_DIR=$INSTALL_DIR # set to eckit install prefix

   # Create the the build directory
   mkdir $BUILD_DIR
   cd $BUILD_DIR

   # Run ecbuild (CMake)
   ecbuild --prefix=$INSTALL_DIR -- $SRC_DIR

   # Build and install
   make -j10
   make test # optional
   make install

   # Check installation
   $INSTALL_DIR/bin/odc --version


.. index:: Build; Optional Features, Install; Optional Features

Optional Features
~~~~~~~~~~~~~~~~~

Fortran interface is an optional feature, and requires an activation flag to be set during build configuration:

.. code-block:: shell

   ENABLE_FORTRAN=ON
   ecbuild --prefix=$INSTALL_DIR -- -DENABLE_FORTRAN=$ENABLE_FORTRAN $SRC_DIR


.. _`CMake`: https://cmake.org
.. _`ecbuild`: https://github.com/ecmwf/ecbuild
.. _`eckit`: https://github.com/ecmwf/eckit
.. _`Doxygen`: https://www.doxygen.nl
