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

#include "odb_api/odbql.h"

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
    int i;

    int rc = odbql_open("CREATE TYPE bf AS (f1 bit1, f2 bit2);\n"
                        "CREATE TABLE foo AS"
                        "   (x INTEGER, y REAL, v STRING, status bf)"
                        " ON 'new_api_example.odb';", &db);
    checkRC(rc, "Cannot open database", db);
    
    rc = odbql_prepare_v2(db, 
            "INSERT INTO foo (x,y,v,status) VALUES (?,?,?,?);", 
            -1, 
            &stmt, 
            0);
    checkRC(rc, "Failed to prepare INSERT statement", db);

    // Populate first row with NULLs
    for (i = 0; i < 4; ++i) 
        odbql_bind_null(stmt, i);
    rc = odbql_step(stmt);

    // Few more rows with some non-NULL values
    for (i = 0 ; i < 4; ++i)
    {
        rc = odbql_bind_int(stmt, 0, 1 * i);
        checkRC(rc, "Failed to bind int value", db);

        rc = odbql_bind_double(stmt, 1, 0.1 * i);
        checkRC(rc, "Failed to bind double value", db);

        rc = odbql_bind_text(stmt, 2, i%2 ? "hello" : "HELLO", 5 /* strlen("hello") */, ODBQL_STATIC);
        checkRC(rc, "Failed to bind string", db);

        rc = odbql_bind_int(stmt, 3, 3 * i);
        checkRC(rc, "Failed to bind bitfield value", db);

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
    
    int rc = odbql_open("CREATE TABLE foo ON 'new_api_example.odb';", &db);
                           //" ON 'mars://RETRIEVE,CLASS=OD,TYPE=MFB,STREAM=OPER,EXPVER=0001,DATE=20160720,TIME=1200,DATABASE=marsod';", &db);
    checkRC(rc, "Cannot open database", db);
    
    //rc = odbql_prepare_v2(db, "SELECT x,y,v,status,status.* FROM foo;", -1, &res, 0);
    rc = odbql_prepare_v2(db, "SELECT ALL * FROM foo;", -1, &res, 0);
    checkRC(rc, "Failed to prepare statement", db);

    int number_of_columns = odbql_column_count(res);

    // Print CSV header. Each field is a colon separated pair of column name and type.
    int i = 0;
    for ( ; i < number_of_columns; ++i)
        printf("%s:%d%s", odbql_column_name(res, i), 
                          odbql_column_type(res, i), 
                          ((i < number_of_columns - 1) ? "," : ""));
    printf("\n");
   
    // Print rows of data. 
    while((rc = odbql_step(res)) != ODBQL_DONE) {

        if (rc == ODBQL_METADATA_CHANGED)
        {
            // Metadata on file: number, names and types of columns changed.
            number_of_columns = odbql_column_count(res);
        }

        if (rc == ODBQL_ROW) {
            int column = 0;
            for (; column < number_of_columns; ++column)
                printf("%s%s", odbql_column_value(res, column) 
                                ? odbql_column_text(res, column) 
                                : (unsigned char *) "NULL",
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
