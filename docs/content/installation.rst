Installation
============

.. index:: Dependencies

Dependencies
------------

.. index::
   single: Dependencies; Required

Required
^^^^^^^^

* C or C++ compiler
* `CMake`_
* `ecbuild`_
* `eckit`_


.. index:: Dependencies; Optional

Optional
^^^^^^^^

* Fortran compiler


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
   export eckit_DIR=$HOME/local # set to eckit prefix
   ENABLE_FORTRAN=OFF # set to ON for Fortran support

   # Create the the build directory
   mkdir $BUILD_DIR
   cd $BUILD_DIR

   # Run ecbuild (CMake)
   ecbuild --prefix=$INSTALL_DIR -- -DENABLE_FORTRAN=$ENABLE_FORTRAN $SRC_DIR

   # Build and install
   make -j10
   make test # optional
   make install

   # Check installation
   $INSTALL_DIR/bin/odc --version


.. _`CMake`: https://cmake.org
.. _`ecbuild`: https://github.com/ecmwf/ecbuild
.. _`eckit`: https://github.com/ecmwf/eckit
