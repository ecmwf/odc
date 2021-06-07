# odc

A package to read and write ODB\-2 data.

## Dependencies

### Required

* C or C++ compiler
* [CMake]
* [ecbuild]
* [eckit]

### Optional

* Fortran compiler
* [Doxygen]

## Installation

```sh
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
```

## Usage

Include the `odc` headers like so:

```c
// odc_test.c
#include "odc/api/odc.h"

int main() {
   odc_initialise_api();
   return 0;
}
```

Make sure to reference the linked library when compiling:

```sh
gcc -lodccore odc_test.c
```

## Build Documentation

The documentation is generated using Sphinx.

First, make sure that `Doxygen` module is available, and then install Python dependencies in your environment:

```sh
cd docs
pip install -r requirements.txt
```

You can then build the documentation by using **make**:

```sh
cd docs
make html
```

The built HTML documentation will be available under the `docs/_build/html/index.html` path.

## Licence

This software is licensed under the terms of the Apache Licence Version 2.0 which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.

In applying this licence, ECMWF does not waive the privileges and immunities granted to it by virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.

[CMake]: https://cmake.org
[ecbuild]: https://github.com/ecmwf/ecbuild
[eckit]: https://github.com/ecmwf/eckit
[Doxygen]: https://www.doxygen.nl
