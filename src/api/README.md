# ODB API examples

This directory contains examples of usage of ODB API.

## C/C++ API

The C API (Application Programming Interface) has been inspired by sqlite3 API. It consists of a set of C functions and constants defined in header file `odbql.h`. This API is intended to be used in C and C++ programs. It is also a basis of Fortran and Python ODB API interfaces' implementations.

Generally, the ODB API functions that are part of public interface start with `odbql_` prefix. All of them have an equivalent in the public interface of sqlite3 that has a `sqlite3_` prefix and a simillar syntax (parameters) and semantics.

Similarly, most status and error codes, as well as numerical codes denoting column types, defined in `odbql.h` with macros prefixed with `ODBQL_`, have an equivalent macro in sqlite3 that starts with `SQLITE_` and has the same meaning and numerical value. 
However, due to differences between ODB API and sqlite3 it was necessary to add some ODB API specific status codes, at this time these are `ODBQL_METADATA_CHANGED` and `ODBQL_BITFIELD` (an ODB specific data type).

See file `odbql_c_example.c` for complete examples of how to read, write and process data with ODB API SQL engine using ODB API C/C++ interface.

## Fortran API

Fortran ODB API is a set of Fortran 95 subroutines and functions wrapping the C API described above. The subroutines and functions are in a module `odbql_wrappers`, and have the same names as their C equivalents.

See file `odbql_fortran_example.f90` for example programs using module `odbql_wrappers`.

## Python API

Python interface of ODB API implements PEP 249 -- Python Database API Specification v2.0.

See `odbql_python_example.py` for example code using the module.

