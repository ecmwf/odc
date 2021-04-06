# odc

A package to read and write ODB-2 data.

## Dependencies

### Required

* C/C++ compiler
* [CMake](https://cmake.org/)
* [ecbuild](https://github.com/ecmwf/ecbuild)
* [eckit](https://github.com/ecmwf/eckit)

### Optional

* Fortran compiler

## Installation

```sh
git clone https://github.com/ecmwf/odc
cd odc

# Setup environment variables (edit as needed)
SRC_DIR=$(pwd)
BUILD_DIR=build
INSTALL_DIR=$HOME/local

# Create the the build directory
mkdir $BUILD_DIR
cd $BUILD_DIR

# Run ecbuild (CMake)
ecbuild --prefix=$INSTALL_DIR -- $SRC_DIR

# Compile and install
make -j10
make test # optional
make install

# Check installation
odc --version
```

## Usage

```c
// odc_test.c
#include "odc/api/odc.h"

int main() {
   odc_initialise_api();
   return 0;
}
```

```sh
gcc -lodccore -o odc_test odc_test.c
./odc_test
```

## License

`odc` is distributed under the Apache license 2.0 - see the accompanying [LICENSE](./LICENSE) file for more details.
