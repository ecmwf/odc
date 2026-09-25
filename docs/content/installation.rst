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
* `Rust`_ toolchain (>= 1.90)


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


.. index::
   single: Install; Conda Forge

Conda Forge
-----------

Alternatively, you can install **odc** package in `Conda`_ environment via `conda-forge`_ channel:

.. code-block:: shell

   conda install -c conda-forge odc


.. note::

   Installing **odc** via **conda-forge** channel will provide Fortran interface out of the box.


Check if the package was installed correctly:

.. code-block:: shell

   odc --version


.. index::
   single: Install; Rust

Rust
----

The Rust crate lives in the ``rust/`` directory of the repository and requires a `Rust`_ toolchain (>= 1.90). By default it builds **odc** and its dependencies from source (the ``vendored`` feature), so no separate installation is needed — add the dependency to ``Cargo.toml``:

.. code-block:: toml

   [dependencies]
   odc = { git = "https://github.com/ecmwf/odc" }


To link against a system-installed **odc** instead, disable the default features and enable ``system``:

.. code-block:: toml

   [dependencies]
   odc = { git = "https://github.com/ecmwf/odc", default-features = false, features = ["system"] }


.. _`CMake`: https://cmake.org
.. _`ecbuild`: https://github.com/ecmwf/ecbuild
.. _`eckit`: https://github.com/ecmwf/eckit
.. _`Doxygen`: https://www.doxygen.nl
.. _`Conda`: https://docs.conda.io
.. _`Rust`: https://www.rust-lang.org
.. _`conda-forge`: https://conda-forge.org
