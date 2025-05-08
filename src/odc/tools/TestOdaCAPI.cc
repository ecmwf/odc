/*
 * (C) Copyright 1996-2012 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "TestCase.h"
#include "eckit/eckit.h"
#include "eckit/exception/Exceptions.h"
#include "eckit/log/Log.h"
#include "eckit/log/Timer.h"
#include "odc/Writer.h"

#include "odc/odccapi.h"

#include "TestOdaCAPI.h"

using namespace std;
using namespace eckit;
using namespace odc;

namespace odc {
namespace tool {
namespace test {

int test_odacapi_setup_in_C(int argc, char* argv[]) {
    const char* filename = "test.odb";
    int err              = 0;

    oda_writer* writer = odb_writer_create("", &err);
    ASSERT(writer);

    oda_write_iterator* wi = odb_create_write_iterator(writer, filename, &err);
    ASSERT(wi);

    ASSERT(0 == odb_write_iterator_set_no_of_columns(wi, 2));
    ASSERT(0 == odb_write_iterator_set_column(wi, 0, odc::api::INTEGER, "ifoo"));
    ASSERT(0 == odb_write_iterator_set_column(wi, 1, odc::api::REAL, "nbar"));

    ASSERT(0 == odb_write_iterator_write_header(wi));

    double data[2];
    for (int i = 1; i <= 10; i++) {
        data[0] = i;
        data[1] = i;

        ASSERT(0 == odb_write_iterator_set_next_row(wi, data, 2));
    }

    ASSERT(0 == odb_write_iterator_destroy(wi));
    ASSERT(0 == odb_writer_destroy(writer));
    return 0;
}

int test_odacapi_setup(int argc, char* argv[]) {
    Timer t("Writing test.odb");
    odc::Writer<> oda("test.odb");

    odc::Writer<>::iterator writer = oda.begin();
    writer->setNumberOfColumns(2);

    writer->setColumn(0, "ifoo", odc::api::INTEGER);
    writer->setColumn(1, "nbar", odc::api::REAL);

    writer->writeHeader();

    for (int i = 1; i <= 10; i++) {
        writer->data()[0] = i;  // col 0
        writer->data()[1] = i;  // col 1
        ++writer;
    }
    // writer->close();
    return 0;
}

int test_odacapi1(int argc, char* argv[]) {
    std::cout << "UnitTest odacapi..." << std::endl;

    int err;

    std::cout << "Calling oda_create..." << std::endl;

    oda_ptr oh = odb_read_create("", &err);

    oda_read_iterator* it = odb_create_read_iterator(oh, "test.odb", &err);
    ASSERT(0 == err);
    ASSERT(0 != it);

    int nCols;
    ASSERT(0 == odb_read_iterator_get_no_of_columns(it, &nCols));
    ASSERT(nCols == 2);

    int type0;
    ASSERT(0 == odb_read_iterator_get_column_type(it, 0, &type0));
    ASSERT(type0 == 1 /*INTEGER*/);

    int type1;
    ASSERT(0 == odb_read_iterator_get_column_type(it, 1, &type1));
    ASSERT(type1 == 2 /*REAL*/);

    char* name0;
    int name0size;
    ASSERT(0 == odb_read_iterator_get_column_name(it, 0, &name0, &name0size));

    char* name1;
    int name1size;
    ASSERT(0 == odb_read_iterator_get_column_name(it, 1, &name1, &name1size));

    double buffer[2];
    double* data   = buffer;
    int newDataset = 0;
    int nRows      = 0;
    while (0 == odb_read_iterator_get_next_row(it, 2, data, &newDataset)) {
        ++nRows;
        int v0 = int(data[0]);

        std::cout << "Read row " << nRows << std::endl;

        ASSERT(v0 == nRows);
    }

    ASSERT(0 == odb_read_iterator_destroy(it));
    ASSERT(0 == odb_read_destroy(oh));
    std::cout << "OK" << std::endl;
    return 0;
}

int test_odacapi2(int argc, char* argv[]) {
    std::cout << "UnitTest odacapi 2..." << std::endl;

    int err;

    std::cout << "Calling odb_start_with_args..." << std::endl;
    odb_start_with_args(argc, argv);

    std::cout << "Calling odb_create..." << std::endl;

    oda_ptr oh = odb_select_create("", &err);

    Log::info() << "Log::info initialised properly." << std::endl;

    oda_select_iterator* it = odb_create_select_iterator(oh, "select * from \"test.odb\";", &err);
    ASSERT(0 == err);
    ASSERT(0 != it);

    int nCols;
    ASSERT(0 == odb_select_iterator_get_no_of_columns(it, &nCols));
    ASSERT(nCols == 2);

    int type0;
    ASSERT(0 == odb_select_iterator_get_column_type(it, 0, &type0));
    ASSERT(type0 == 1 /*INTEGER*/);

    int type1;
    ASSERT(0 == odb_select_iterator_get_column_type(it, 1, &type1));
    ASSERT(type1 == 2 /*REAL*/);

    char* name0;
    int name0size;
    ASSERT(0 == odb_select_iterator_get_column_name(it, 0, &name0, &name0size));

    char* name1;
    int name1size;
    ASSERT(0 == odb_select_iterator_get_column_name(it, 1, &name1, &name1size));

    double buffer[2];
    double* data   = buffer;
    int newDataset = 0;
    int nRows      = 0;
    while (0 == odb_select_iterator_get_next_row(it, 2, data, &newDataset)) {
        ++nRows;
        int v0 = int(data[0]);

        std::cout << "Read row " << nRows << std::endl;

        ASSERT(v0 == nRows);
    }

    ASSERT(0 == odb_select_iterator_destroy(it));
    ASSERT(0 == odb_read_destroy(oh));
    std::cout << "OK" << std::endl;
    return 0;
}

int test_odacapi3(int argc, char* argv[]) {
    std::cout << "UnitTest ODB C API append to file functionality..." << std::endl;

    const char* filename = "test.odb";
    int err              = 0;

    double n = odb_count(filename);
    std::cout << "test_odacapi3: number of rows = " << n << std::endl;
    ASSERT(n == 10);

    oda_writer* writer = odb_writer_create("", &err);
    ASSERT(writer);

    oda_write_iterator* wi = odb_create_append_iterator(writer, filename, &err);
    ASSERT(wi);
    ASSERT(0 == err);
    ASSERT(0 != wi);

    ASSERT(0 == odb_write_iterator_set_no_of_columns(wi, 2));
    ASSERT(0 == odb_write_iterator_set_column(wi, 0, odc::api::INTEGER, "ifoo"));
    ASSERT(0 == odb_write_iterator_set_column(wi, 1, odc::api::REAL, "nbar"));

    ASSERT(0 == odb_write_iterator_write_header(wi));

    double data[2];
    for (int i = 1; i <= 10; i++) {
        data[0] = i;
        data[1] = i;

        ASSERT(0 == odb_write_iterator_set_next_row(wi, data, 2));
    }

    ASSERT(0 == odb_write_iterator_destroy(wi));
    ASSERT(0 == odb_writer_destroy(writer));


    n = odb_count(filename);
    std::cout << "test_odacapi3: number of rows = " << n << std::endl;
    ASSERT(n == 20);

    return 0;
}

int test_odacapi(int argc, char* argv[]) {
    std::cout << "Calling odb_init..." << std::endl;
    odb_start_with_args(argc, argv);
    Log::info() << "Log::info initialised properly." << std::endl;

    // return test_odacapi_setup()
    return test_odacapi_setup_in_C(argc, argv) || test_odacapi1(argc, argv) || test_odacapi2(argc, argv) ||
           test_odacapi3(argc, argv);
}

}  // namespace test
}  // namespace tool
}  // namespace odc
