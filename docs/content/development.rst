Development
===========

Contributions
-------------

.. index:: Github

The code repository is hosted on `Github`_. Feel free to fork it and submit your PRs against the **develop** branch. The first time you do this, you will be asked to sign our Contributor License Agreement via an automatic comment in the pull request.

To clone the repository locally, you can use the following command:

.. code-block:: shell

   git clone --branch develop git@github.com:ecmwf/odc.git


.. index:: Dependencies; Development

Development Dependencies
------------------------

Required
~~~~~~~~

* C or C++ compiler
* `CMake`_ (>= 3.20.1)
* `ecbuild`_ (>= 3.6.1)
* `eckit`_ (>= 1.16.1)


Optional
~~~~~~~~

* Fortran compiler
* `Doxygen`_ (>= 1.9.1)


.. index:: Unit Tests

Run Unit Tests
--------------

To run the test suite, you can use the following command:

.. code-block:: shell

   # Setup environment variables (edit as needed)
   SRC_DIR=$(pwd)
   BUILD_DIR=build
   export eckit_DIR=$HOME/local # set to eckit prefix

   # Create the the build directory
   mkdir $BUILD_DIR
   cd $BUILD_DIR

   # Run ecbuild (CMake)
   ecbuild -- $SRC_DIR

   # Build and test
   make -j10
   make test


Optional Features
~~~~~~~~~~~~~~~~~

Fortran interface and its tests are an optional feature, and require an activation flag to be set during build configuration:

.. code-block:: shell

   ENABLE_FORTRAN=ON
   ecbuild -- -DENABLE_FORTRAN=$ENABLE_FORTRAN $SRC_DIR


.. index:: Dependencies; Documentation

Build Documentation
-------------------

The documentation is generated using Sphinx.

First, make sure that ``Doxygen`` module is available, and then install Python dependencies in your environment:

.. code-block:: shell

   pip install -r docs/requirements.txt


You can then build the documentation by using **make**:

.. code-block:: shell

   cd docs
   make html


The built HTML documentation will be available under the ``docs/_build/html/index.html`` path.


.. _`Github`: https://github.com/ecmwf/odc
.. _`cmake`: https://cmake.org
.. _`ecbuild`: https://github.com/ecmwf/ecbuild
.. _`eckit`: https://github.com/ecmwf/eckit
.. _`Doxygen`: https://www.doxygen.nl
