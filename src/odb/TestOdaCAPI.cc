/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eclib/PathName.h"
#include "eclib/FileHandle.h"
#include "eclib/Timer.h"

#include "odblib/oda.h"

extern "C" {
#include "odblib/odbcapi.h"
}

#include "odb/TestOdaCAPI.h"

using namespace std;

namespace odb {
namespace tool {
namespace test {

int test_odacapi(int argc, char *argv[])
{
	cout << "Calling odb_init..." << endl;
	odb_start();
	Log::info() << "Log::info initialised properly." << endl;

	//return test_odacapi_setup(argc, argv)
	return test_odacapi_setup_in_C(argc, argv)
		|| test_odacapi1(argc, argv)
		|| test_odacapi2(argc, argv)
		|| test_odacapi3(argc, argv);
}

int test_odacapi_setup_in_C(int argc, char *argv[])
{
	const char *filename = "test.odb";
	int err = 0;

	oda_writer* writer = odb_writer_create("", &err);
	ASSERT(writer);

	oda_write_iterator* wi = odb_create_write_iterator(writer, filename, &err);
	ASSERT(wi);

	ASSERT(0 == odb_write_iterator_set_no_of_columns(wi, 2));
	ASSERT(0 == odb_write_iterator_set_column(wi, 0, odb::INTEGER, "ifoo"));
	ASSERT(0 == odb_write_iterator_set_column(wi, 1, odb::REAL, "nbar"));
	
	ASSERT(0 == odb_write_iterator_write_header(wi));

	double data[2];
	for (int i = 1; i <= 10; i++)
	{
		data[0] = i;
		data[1] = i;

		ASSERT(0 == odb_write_iterator_set_next_row(wi, data, 2));
	}

	ASSERT(0 == odb_write_iterator_destroy(wi));
	ASSERT(0 == odb_writer_destroy(writer));
	return 0;
}

int test_odacapi_setup(int argc, char *argv[])
{
	Timer t("Writing test.odb");
	odb::Writer<> oda("test.odb");

	odb::Writer<>::iterator writer = oda.begin();
	writer->columns().setSize(2);

	writer->setColumn(0, "ifoo", odb::INTEGER);
	writer->setColumn(1, "nbar", odb::REAL);
	
	writer->writeHeader();

	for (int i = 1; i <= 10; i++)
	{
		writer->data()[0] = i; // col 0
		writer->data()[1] = i; // col 1
		++writer;
	}
	//writer->close();
	return 0;
}

int test_odacapi1(int argc, char *argv[])
{
	cout << "Testing odacapi..." << endl;

	int err;

	cout << "Calling oda_create..." << endl;

	oda* oh = odb_create("", &err);
	
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

	char *name0;
	int name0size;
	ASSERT(0 == odb_read_iterator_get_column_name(it, 0, &name0, &name0size));

	char *name1;
	int name1size;
	ASSERT(0 == odb_read_iterator_get_column_name(it, 1, &name1, &name1size));

	double buffer[2];
	double* data = buffer;
	int newDataset = 0;
	int nRows = 0;
	while (0 == odb_read_iterator_get_next_row(it, 2, data, &newDataset))
	{
		++nRows;
		int v0 = int(data[0]);

		cout << "Read row " << nRows << endl;

		ASSERT(v0 == nRows);
	}

	ASSERT(0 == odb_read_iterator_destroy(it));
	ASSERT(0 == odb_destroy(oh));
	cout << "OK" << endl;
	return 0;
}

int test_odacapi2(int argc, char *argv[])
{
	cout << "Testing odacapi 2..." << endl;

	int err;

	cout << "Calling odb_start..." << endl;
	odb_start();

	cout << "Calling odb_create..." << endl;

	oda* oh = odb_create("", &err);
	
	Log::info() << "Log::info initialised properly." << endl;
	
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

	char *name0;
	int name0size;
	ASSERT(0 == odb_select_iterator_get_column_name(it, 0, &name0, &name0size));

	char *name1;
	int name1size;
	ASSERT(0 == odb_select_iterator_get_column_name(it, 1, &name1, &name1size));

	double buffer[2];
	double* data = buffer;
	int newDataset = 0;
	int nRows = 0;
	while (0 == odb_select_iterator_get_next_row(it, 2, data, &newDataset))
	{
		++nRows;
		int v0 = int(data[0]);

		cout << "Read row " << nRows << endl;

		ASSERT(v0 == nRows);
	}

	ASSERT(0 == odb_select_iterator_destroy(it));
	ASSERT(0 == odb_destroy(oh));
	cout << "OK" << endl;
	return 0;
}

int test_odacapi3(int argc, char *argv[])
{
	cout << "Testing ODB C API append to file functionality..." << endl;

	const char *filename = "test.odb";
	int err = 0;

	double n = odb_count(filename);
	cout << "test_odacapi3: number of rows = " << n << endl;
	ASSERT(n == 10);

	oda_writer* writer = odb_writer_create("", &err);
	ASSERT(writer);

	oda_write_iterator* wi = odb_create_append_iterator(writer, filename, &err);
	ASSERT(wi);
	ASSERT(0 == err);
	ASSERT(0 != wi);

	ASSERT(0 == odb_write_iterator_set_no_of_columns(wi, 2));
	ASSERT(0 == odb_write_iterator_set_column(wi, 0, odb::INTEGER, "ifoo"));
	ASSERT(0 == odb_write_iterator_set_column(wi, 1, odb::REAL, "nbar"));
	
	ASSERT(0 == odb_write_iterator_write_header(wi));

	double data[2];
	for (int i = 1; i <= 10; i++)
	{
		data[0] = i;
		data[1] = i;

		ASSERT(0 == odb_write_iterator_set_next_row(wi, data, 2));
	}

	ASSERT(0 == odb_write_iterator_destroy(wi));
	ASSERT(0 == odb_writer_destroy(writer));


	n = odb_count(filename);
	cout << "test_odacapi3: number of rows = " << n << endl;
	ASSERT(n == 20);

	return 0;
}

} // namespace test 
} // namespace tool 
} // namespace odb 

