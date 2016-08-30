# Python Database API (PEP 249) implementation for ODB API

# @author Piotr Kuchta, ECMWF, August 2016

import unittest, os
from ctypes import *

# TODO: get the path to libOdb.so
libodb = CDLL('/tmp/build/bundle/debug/lib/libOdb.so')

# odbql prototypes 
odbql_open = libodb.odbql_open
odbql_prepare_v2 = libodb.odbql_prepare_v2
odbql_step = libodb.odbql_step
odbql_column_name = libodb.odbql_column_name
odbql_column_name.restype = c_char_p
odbql_column_count = libodb.odbql_column_count
odbql_finalize = libodb.odbql_finalize
odbql_close = libodb.odbql_close
odbql_bind_null = libodb.odbql_bind_null
odbql_bind_int = libodb.odbql_bind_int
odbql_bind_double = libodb.odbql_bind_double
odbql_bind_text = libodb.odbql_bind_text
odbql_bind_int = libodb.odbql_bind_int

# constants
ODBQL_OK = 0
ODBQL_ROW = 100
ODBQL_STATIC = c_voidp(0)

class TestODBQL(unittest.TestCase):

    def setUp(self): 
        pass

    def tearDown(self):
        pass

    def test_insert_data(self):
        db, stmt, tail = c_voidp(), c_voidp(), c_char_p()

        rc = odbql_open("""
            CREATE TYPE bf AS (f1 bit1, f2 bit2); 
            
            CREATE TABLE foo AS (
                x INTEGER, 
                y REAL, 
                v STRING, 
                status bf
            ) ON 'new_api_example_python.odb';
            """, byref(db))
        self.assertEqual(rc, ODBQL_OK)

        rc = odbql_prepare_v2(db, 
            """INSERT INTO foo (x,y,v,status) VALUES (?,?,?,?);""",
             -1,
             byref(stmt),
             byref(tail))
        self.assertEqual(rc, ODBQL_OK)
        
        for i in range(4): 
            odbql_bind_null(stmt, i)
        rc = odbql_step(stmt)
        for i in range(4): 
            rc = odbql_bind_int(stmt, 0, 1 * i)
            self.assertEqual(rc, ODBQL_OK)
            rc = odbql_bind_double(stmt, 1, c_double(0.1 * i))
            self.assertEqual(rc, ODBQL_OK)
            rc = odbql_bind_text(stmt, 2, "hello", len("hello"), ODBQL_STATIC)
            self.assertEqual(rc, ODBQL_OK)
            rc = odbql_bind_int(stmt, 3, 3 * i)
            self.assertEqual(rc, ODBQL_OK)
            rc = odbql_step(stmt)
            self.assertEqual(rc, ODBQL_ROW)
        
        rc = odbql_finalize(stmt)
        self.assertEqual(rc, ODBQL_OK)
        rc = odbql_close(db)
        self.assertEqual(rc, ODBQL_OK)


    def test_select_data(self):
        db, stmt, tail = c_voidp(), c_voidp(), c_char_p()

        rc = odbql_open("CREATE TABLE foo ON 'new_api_example_python.odb';", byref(db))
        self.assertEqual(rc, ODBQL_OK)

        rc = odbql_prepare_v2(db, "SELECT ALL * FROM foo;", -1, byref(stmt), byref(tail))
        self.assertEqual(rc, ODBQL_OK)

        rc = odbql_step(stmt)
        self.assertEqual(rc, ODBQL_ROW)
        number_of_columns = odbql_column_count(stmt)
        print 'number_of_columns=', number_of_columns

        for i in range(0,number_of_columns):
            column_name = odbql_column_name(stmt,i)
            print i, column_name

        rc = odbql_finalize(stmt)
        self.assertEqual(rc, ODBQL_OK)
        rc = odbql_close(db)
        self.assertEqual(rc, ODBQL_OK)

        os.remove('new_api_example_python.odb')

if __name__ == '__main__':
    unittest.main()
