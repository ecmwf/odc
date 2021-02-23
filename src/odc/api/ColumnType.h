/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#ifndef odc_api_ColumnType_H
#define odc_api_ColumnType_H

#include <cstdint>
#include <cstddef>

namespace odc {
namespace api {

//------------------------------------------------------------------------------------------------------------

enum ColumnType {
	IGNORE   = 0,
	INTEGER  = 1,
	REAL     = 2,
	STRING   = 3,
	BITFIELD = 4,
	DOUBLE   = 5,

	// This value exists only to keep the Clang Undefined Behaviour sanitizer happy in unit tests.
    INVALID_ONLY_FOR_TESTING = 123
};

constexpr int NUM_TYPES = 6;

template <ColumnType ty> struct OdbTypes{};

// Specialisations

template <> struct OdbTypes<IGNORE>   {
    static constexpr const char* name = "ignore";
    // Size undefined
};
template <> struct OdbTypes<INTEGER>  {
    static constexpr const char* name = "integer";
    static constexpr size_t size = sizeof(int64_t);
};
template <> struct OdbTypes<REAL>     {
    static constexpr const char* name = "real";
    static constexpr size_t size = sizeof(float);
};
template <> struct OdbTypes<STRING>   {
    static constexpr const char* name = "string";
    // Size variable
};
template <> struct OdbTypes<BITFIELD> {
    static constexpr const char* name = "bitfield";
    static constexpr size_t size = sizeof(int64_t);
};
template <> struct OdbTypes<DOUBLE> {
    static constexpr const char* name = "double";
    static constexpr size_t size = sizeof(double);
};

//------------------------------------------------------------------------------------------------------------

} // namespace api
} // namespace odc 

#endif // odc_api_ColumnType_H
