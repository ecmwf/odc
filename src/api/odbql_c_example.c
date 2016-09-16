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
                        " ON 'new_api_c_example.odb';", &db);
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
    int i, rc, column, number_of_columns;
    long long number_of_rows = 0, number_of_rows_in_current_dataset = 0;
    
    rc = odbql_open("CREATE TABLE foo ON 'new_api_c_example.odb';", &db);
                           //" ON 'mars://RETRIEVE,CLASS=OD,TYPE=MFB,STREAM=OPER,EXPVER=0001,DATE=20160720,TIME=1200,DATABASE=marsod';", &db);
    checkRC(rc, "Cannot open database", db);
    
    //rc = odbql_prepare_v2(db, "SELECT x,y,v,status,status.* FROM foo;", -1, &res, 0);
    rc = odbql_prepare_v2(db, "SELECT ALL * FROM foo;", -1, &res, 0);
    checkRC(rc, "Failed to prepare statement", db);
   
    // Print rows of data. 
    while((rc = odbql_step(res)) != ODBQL_DONE) 
    {
        if (number_of_rows == 0 || rc == ODBQL_METADATA_CHANGED)
        {
            number_of_columns = odbql_column_count(res);

            if (number_of_rows_in_current_dataset)
                printf("Number of rows: %d\n", number_of_rows_in_current_dataset);
/*
            // Print CSV header. Each field is a colon separated pair of column name and type.
            printf("\nNew dataset. Number of columns: %d\n", number_of_columns);
            for (i = 0; i < number_of_columns; ++i)
                printf("%s:%d%s", odbql_column_name(res, i), 
                                  odbql_column_type(res, i), 
                                  ((i < number_of_columns - 1) ? "," : ""));
            printf("\n");
*/

            number_of_rows_in_current_dataset = 0;
        }

        if (rc == ODBQL_ROW) 
        {
            column = 0;

/*
            for (; column < number_of_columns; ++column)
            {
                printf("%s%s", odbql_column_value(res, column) 
                                ? odbql_column_text(res, column) 
                                : (unsigned char *) "NULL",
                               ((column < number_of_columns - 1) ? "," : ""));

            }
*/
            if (number_of_rows == 0)
            {
                for (i = 0; i < number_of_columns; ++i)
                    if (odbql_column_value(res, i)) return 100; // first row should be all NULLL
            }
            else
            {
                // Check first column
                odbql_value* pv = odbql_column_value(res, 0);
                if ( pv == 0 )
                {
                    fprintf (stderr, "\nUnexpected NULL in column %ld of row %ld\n", 0L, number_of_rows);
                    return 101;
                }

                int iv = odbql_value_int(pv);
                if ( iv != number_of_rows - 1)
                {
                    fprintf (stderr, "\nUnexpected value in column %ld of row %ld\n", 0L, number_of_rows);
                    return 102;
                }

                // Check last column (bitfield)
                // We have a NULL on the first column (set with odbql_bind_null)
                // but also on the second line, because 0 in a bitfield column means NULL
                // (ECMWF convention since ODB-1)
                if (number_of_rows > 1)
                {
                    pv = odbql_column_value(res, 3);
                    if ( pv == 0 )
                    {
                        fprintf (stderr, "\nUnexpected NULL in column %ld of row %ld\n", 3L, number_of_rows);
                        return 103;
                    }

                    iv = odbql_value_int(pv);
                    if ( iv != 3 * (number_of_rows - 1))
                    {
                        fprintf (stderr, "\nUnexpected value in column %ld of row %ld\n", 3L, number_of_rows);
                        return 104;
                    }
                }
            }

            printf("\n");
        }
        ++ number_of_rows;
        ++ number_of_rows_in_current_dataset;
    }
    if (number_of_rows_in_current_dataset)
        printf("Number of rows: %d\n", number_of_rows_in_current_dataset);

    printf("\nProcessed %ld rows.\n", number_of_rows);
    return 0;
}

int odbql_example_execute_embedded_ecml()
{
    odbql *db;
    odbql_stmt *res;
    int rc;

    rc = odbql_open("CREATE TABLE foo ON 'new_api_c_example.odb';", &db);
    checkRC(rc, "Cannot open database", db);

    rc = odbql_prepare_v2(db, " { compare, left = new_api_c_example.odb, right = new_api_c_example.odb }; ", -1, &res, 0);
    checkRC(rc, "Failed to prepare embedded statement", db);

    // Print rows of data. 
    while((rc = odbql_step(res)) != ODBQL_DONE) 
    {
            printf("\n+++\n");
    }
    
    rc = odbql_finalize(res);
    checkRC(rc, "odbql_finalize failed", db);
    rc = odbql_close(db);
    checkRC(rc, "odbql_close failed", db);

    return 0;
}
