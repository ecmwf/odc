/*
 * © Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <iostream>
#include <sstream>
#include <omp.h>

#include "odblib/odb_api.h"

#include "odbcapi.h"

#define RMDI   -2147483647

int main(int argc, char *argv[])
{
	odb_start();

# pragma omp parallel 
	{
		stringstream ss;
		ss << "omp" << omp_get_thread_num() << ".odb";
		odb::Writer<> odb_writer(ss.str());

# pragma omp for schedule(static,1)
		for (int i = 0; i < omp_get_num_threads(); ++i)
		{
			odb::Writer<>::iterator row = odb_writer.begin();
			row->columns().setSize(2);

			row->setColumn(0, "date", odb::INTEGER, false, RMDI);
			row->setColumn(1, "time", odb::INTEGER, false, RMDI);

			row->writeHeader();

			for (size_t n = 0; n < 1000; ++n)
			{
				(*row)[0] = 20090706.0;
				(*row)[1] = 210109.0;
				++row;

				unsigned short state[3] = {0,1,2};
				::usleep(nrand48(state) % 10);
			}
		}
	}

	return 0;
}
