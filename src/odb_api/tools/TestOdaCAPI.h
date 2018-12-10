/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#ifndef TEST_ODA_C_API_H
#define TEST_ODA_C_API_H

namespace odc {
namespace tool {
namespace test {

int test_odacapi(int argc, char *argv[]);

int test_odacapi_setup_in_C(int argc, char *argv[]);
int test_odacapi_setup(int argc, char *argv[]);
int test_odacapi1(int argc, char *argv[]);
int test_odacapi2(int argc, char *argv[]);
int test_odacapi3(int argc, char *argv[]);

} // namespace test 
} // namespace tool 
} // namespace odc 

#endif
