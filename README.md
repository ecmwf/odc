odc
===

A package to read/write ODB data.


Requirements
------------

* [CMake](https://cmake.org/)
* [ecbuild](https://github.com/ecmwf/ecbuild)
* [eckit](https://github.com/ecmwf/eckit)


Installation
------------

```sh
git clone https://github.com/ecmwf/odc
cd odc

# Setup environment variables (edit as needed)
SRC_DIR=$(pwd)
BUILD_DIR=build

# Create the the build directory
mkdir $BUILD_DIR
cd $BUILD_DIR

# Run ecbuild (CMake)
ecbuild $SRC_DIR

# Compile and install
make -j10
make test # optional
make install

# Check installation
odc --version
```


License
-------

It is distributed under the Apache license 2.0 - see the accompanying [LICENSE](./LICENSE) file for more details.
