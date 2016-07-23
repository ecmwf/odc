/*
 * (C) Copyright 1996-2012 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// \file examples.cc
///
/// This file contains examples of usage of public APIs.
///
/// @author Piotr Kuchta, ECMWF, July 2016

#include "odbql.h"

// We need to include stdio.h for the fprintf declaration
#include "stdio.h"

#define checkRC(return_code, message, db) { \
    if (return_code != ODBQL_OK) { \
        fprintf(stderr, "%s: %s\n", message, odbql_errmsg(db)); \
        odbql_close(db); \
        return 1; \
    } \
}

int odbql_example_insert_data()
{
    odbql *db;
    odbql_stmt *stmt;

    int rc = odbql_open("CREATE TABLE foo AS (x INTEGER, y INTEGER, v REAL)"
                          " ON 'example_select_data_read_results.odb';", &db);
    checkRC(rc, "Cannot open database", db);
    
    rc = odbql_prepare_v2(db, "INSERT INTO foo (x,y,v) VALUES (?,?,?);", -1, &stmt, 0);    
    checkRC(rc, "Failed to prepare INSERT statement", db);

    for (int i = 1; i <= 3; ++i)
    {
        rc = odbql_bind_int(stmt, 0, 1 * i);
        checkRC(rc, "Failed to bind int value", db);

        rc = odbql_bind_int(stmt, 1, 10 * i);
        checkRC(rc, "Failed to bind int value", db);

        rc = odbql_bind_double(stmt, 2, 0.1 * i);
        checkRC(rc, "Failed to bind double value", db);

        rc = odbql_step(stmt);
        //checkRC(rc, "Failed to step and write row", db);
    }
    rc = odbql_finalize(stmt);
    checkRC(rc, "odbql_finalize failed", db);

    rc = odbql_close(db);
    checkRC(rc, "odbql_close failed", db);

    return 0;
}

int odbql_example_select_data_read_results()
{
    odbql *db;
    odbql_stmt *res;
    
    int rc = odbql_open("CREATE TABLE foo ON 'example_select_data_read_results.odb';", &db);
                           //" ON 'mars://RETRIEVE,CLASS=OD,TYPE=MFB,STREAM=OPER,EXPVER=0001,DATE=20160720,TIME=1200,DATABASE=marsod';", &db);
    checkRC(rc, "Cannot open database", db);
    
    rc = odbql_prepare_v2(db, "SELECT * FROM foo;", -1, &res, 0);
    checkRC(rc, "Failed to prepare statement", db);

    int number_of_columns = odbql_column_count(res);

    // Print CSV header. Each field is a colon separated pair of column name and type.
    for (int i = 0; i < number_of_columns; ++i)
        printf("%s:%d%s", odbql_column_name(res, i), 
                          odbql_column_type(res, i), 
                          ((i < number_of_columns - 1) ? "," : ""));
    printf("\n");
   
    // Print rows of data. 
    while((rc = odbql_step(res)) != ODBQL_DONE)
    {
        if (rc == ODBQL_ROW)
        {
            for (int column = 0; column < number_of_columns; ++column)
                printf("%s%s", odbql_column_text(res, column),
                               ((column < number_of_columns - 1) ? "," : ""));
            printf("\n");
        }
    }
    
    rc = odbql_finalize(res);
    checkRC(rc, "odbql_finalize failed", db);
    rc = odbql_close(db);
    checkRC(rc, "odbql_close failed", db);
    return 0;
}
