/*
 * (C) Copyright 1996-2012 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// \file CAPIExamples.cc
///
/// This file contains examples of usage of public APIs.
///
/// @author Piotr Kuchta, ECMWF, September 2015

#include <string>
#include <iostream>
#include <vector>
#include <math.h>

#include "odc/odc.h"

#include "odc/odccapi.h"
#include "odc/tools/odc_tools_c.h"

#include "TestCase.h"

namespace {

TEST(c_api_example_select_data_read_results)
{
    const int maxCols = 4;
    int err;
    double buffer[maxCols];
    double* data = buffer;
    int newDataset = 0;
    int nRows = 0;
    oda_ptr oh;
    oda_select_iterator* it;
    size_t i;

    // Prepare input data
    import_text("x:INTEGER,y:INTEGER,v:DOUBLE,f:BITFIELD[a:1;b:2]]\n" 
                "1,1,0.3,0\n"
                "1,1,0.2,1\n" 
                "2,2,0.4,2\n" 
                "2,2,0.1,3\n", 
                "c_example_select_data_read_results.odb");

    oh = odb_select_create("", &err);
    ASSERT(0 == err);
    it = odb_create_select_iterator(oh, "select x, min(v), max(v)"
                                        " from \"c_example_select_data_read_results.odb\"" 
                                        " order by x;", &err);
    ASSERT(0 == err);
    for (i = 0; 0 == odb_select_iterator_get_next_row(it, 3, data, &newDataset); ++i)
    {
        switch (i) {
            case 0: ASSERT(data[0] == 1 && fabs(data[1] - 0.2) < 0.0000001); break;
            case 1: ASSERT(data[0] == 2 && fabs(data[1] - 0.1) < 0.0000001); break;
        }
    }
    ASSERT(i == 2);
    ASSERT(0 == odb_select_iterator_destroy(it));

    // Select some bitfields
    it = odb_create_select_iterator(oh, "select f, f.a, f.b from \"c_example_select_data_read_results.odb\";", &err);
    ASSERT(0 == err);

    for (i = 0; 0 == odb_select_iterator_get_next_row(it, 3, data, &newDataset); ++i)
    {
        double f = data[0], a = data[1], b = data[2];
        switch (i) {
            case 0: ASSERT(f == 0 && a == 0 && b == 0); break;
            case 1: ASSERT(f == 1 && a == 1 && b == 0); break;
            case 2: ASSERT(f == 2 && a == 0 && b == 1); break;
            case 3: ASSERT(f == 3 && a == 1 && b == 1); break;
        }
    }
    ASSERT(i == 4);

    ASSERT(0 == odb_select_iterator_destroy(it));
    ASSERT(0 == odb_select_destroy(oh));
}


TEST(c_api_example_read_data)
{
    const int numberOfColumns = 4;
    int err, nCols, type, nameLength;
    char *name;
    oda_ptr oh;
    oda_read_iterator* it;
    double buffer[numberOfColumns];
    double* data;
    int newDataset;
    int nRows;

    // Prepare input data
    import_text("x:INTEGER,y:REAL,v:DOUBLE,f:BITFIELD[a:1;b:2]\n" 
                "1,1,0.3,0\n" 
                "2,1,0.2,1\n" 
                "3,2,0.4,2\n" 
                "4,2,0.1,3\n",
                "c_api_example_read_data.odb");

    oh = odb_read_create("", &err);
    it = odb_create_read_iterator(oh, "c_api_example_read_data.odb", &err);
    ASSERT(0 == err);
    ASSERT(0 != it);

    odb_read_iterator_get_no_of_columns(it, &nCols);
    ASSERT(nCols == numberOfColumns);

    ASSERT(0 == odb_read_iterator_get_column_type(it, 0, &type));
    ASSERT(type == 1 /*INTEGER*/);

    ASSERT(0 == odb_read_iterator_get_column_type(it, 1, &type));
    ASSERT(type == 2 /*REAL*/);

    ASSERT(0 == odb_read_iterator_get_column_type(it, 2, &type));
    ASSERT(type == 5 /*DOUBLE*/);

    ASSERT(0 == odb_read_iterator_get_column_type(it, 3, &type));
    ASSERT(type == 4 /*BITFIELD*/);

    ASSERT(0 == odb_read_iterator_get_column_name(it, 0, &name, &nameLength));
    ASSERT(0 == strncmp("x", name, strlen("x")));
    ASSERT(0 == odb_read_iterator_get_column_name(it, 1, &name, &nameLength));
    ASSERT(0 == strncmp("y", name, strlen("y")));
    ASSERT(0 == odb_read_iterator_get_column_name(it, 2, &name, &nameLength));
    ASSERT(0 == strncmp("v", name, strlen("v")));
    ASSERT(0 == odb_read_iterator_get_column_name(it, 3, &name, &nameLength));
    ASSERT(0 == strncmp("f", name, strlen("f")));

    data = buffer;
    newDataset = 0;
    for (nRows = 0; 0 == odb_read_iterator_get_next_row(it, numberOfColumns, data, &newDataset); ++nRows)
    {
        int x = int(data[0]);
        int f = int(data[3]);

        std::cout << "Read row " << nRows << ", x=" << x << std::endl;

        ASSERT(x == nRows + 1 && f == nRows);
    }

    ASSERT(nRows == 4);

    ASSERT(0 == odb_read_iterator_destroy(it));
    ASSERT(0 == odb_read_destroy(oh));
}

TEST(c_api_example_write_data)
{
    int err;
    oda_writer* writer = odb_writer_create("", &err);
    oda_write_iterator* wi = odb_create_write_iterator(writer, "c_api_example_write_data.odb", &err);
    ASSERT(0 == odb_write_iterator_set_no_of_columns(wi, 4));
    ASSERT(0 == odb_write_iterator_set_column(wi, 0, odc::INTEGER, "x"));
    ASSERT(0 == odb_write_iterator_set_column(wi, 1, odc::REAL, "y"));
    ASSERT(0 == odb_write_iterator_set_column(wi, 2, odc::DOUBLE, "v"));
    // Define three fields: a (1 bit only), b (2 bits), c (1 bit)
    ASSERT(0 == odb_write_iterator_set_bitfield(wi, 3, odc::BITFIELD, "bf", "a:b:c", "1:2:1"));
    ASSERT(0 == odb_write_iterator_write_header(wi));

    double data[4];
    for (int i = 1; i <= 10; ++i)
    {
        data[0] = i;
        data[1] = i * 10;
        data[2] = i * 100;
        data[3] = i;

        ASSERT(0 == odb_write_iterator_set_next_row(wi, data, 4));
    }

    ASSERT(0 == odb_write_iterator_destroy(wi));
    ASSERT(0 == odb_writer_destroy(writer));
}

} // namespace

