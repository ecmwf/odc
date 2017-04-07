#!/usr/bin/env python 

import sys
sys.path.append('/tmp/build/bundle/debug/odb_api/src/python/odb')

import unittest
from odb import *

class TestODB250_A(unittest.TestCase):
    """
    ODB-250 Bitfields were not read correctly
    """
    def setUp(self): 
        self.N = 100000
        self.db = c_voidp()
        self.stmt = c_voidp()
        self.tail = c_char_p()
        rc = odbql_open("", byref(self.db))
        self.assertEqual(rc, ODBQL_OK)
        rc = odbql_prepare_v2(self.db, """ CREATE TYPE bf AS (f bit31); CREATE TABLE foo AS (bf1 bf) ON 'test_python_bitfields.odb'; """ , -1, byref(self.stmt), byref(self.tail))
        self.assertEqual(rc, ODBQL_OK)

    def tearDown(self):
        rc = odbql_close(self.db)
        self.assertEqual(rc, ODBQL_OK)

    def test_insert_bitfields(self):
        rc = odbql_prepare_v2(self.db, "INSERT INTO foo VALUES (?);", -1, byref(self.stmt), byref(self.tail))
        self.assertEqual(rc, ODBQL_OK)

        for i in range(self.N): 
            rc = odbql_bind_int(self.stmt, 0, i)
            self.assertEqual(rc, ODBQL_OK)

            rc = odbql_step(self.stmt)
            self.assertEqual(rc, ODBQL_ROW)

        rc = odbql_finalize(self.stmt)
        self.assertEqual(rc, ODBQL_OK)
       
    def test_select_bitfields(self):
        rc = odbql_prepare_v2(self.db, "SELECT * FROM foo;", -1, byref(self.stmt), byref(self.tail))
        for i in range(self.N): 
            rc = odbql_step(self.stmt)
            self.assertEqual(rc, ODBQL_ROW)

            v = odbql_column_value(self.stmt, 0)
            if v == 0:
                self.assertEqual(i, 0)
            else:
                self.assertEqual(i, odbql_value_int(v))

        rc = odbql_step(self.stmt)
        self.assertEqual(rc, ODBQL_DONE)


TEST_DDL = """
            CREATE TYPE bf AS (f1 bit1, f2 bit2); 
            
            CREATE TABLE foo AS (
                x INTEGER, 
                y DOUBLE, 
                v STRING, 
                status bf
            ) ON 'new_api_example_python.odb';
"""

TEST_INSERT = """INSERT INTO foo (x,y,v,status) VALUES (?,?,?,?);"""
TEST_SELECT = """SELECT * FROM foo;"""

class TestODBQL(unittest.TestCase):

    def setUp(self): 
        pass

    def tearDown(self):
        pass

    def test_insert_data(self):
        db, stmt, tail = c_voidp(), c_voidp(), c_char_p()

        rc = odbql_open("", byref(db))
        self.assertEqual(rc, ODBQL_OK)

        rc = odbql_prepare_v2(db, TEST_DDL, -1, byref(stmt), byref(tail))
        self.assertEqual(rc, ODBQL_OK)
        rc = odbql_prepare_v2(db, TEST_INSERT, -1, byref(stmt), byref(tail))
        self.assertEqual(rc, ODBQL_OK)
        
        for i in range(4): 
            rc = odbql_bind_null(stmt, i)
            self.assertEqual(rc, ODBQL_OK)

        rc = odbql_step(stmt)
        self.assertEqual(rc, ODBQL_ROW)

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

        rc = odbql_open("new_api_example_python.odb", byref(db))
        self.assertEqual(rc, ODBQL_OK)

        rc = odbql_prepare_v2(db, "CREATE TABLE foo on 'new_api_example_python.odb';", -1, byref(stmt), byref(tail))
        self.assertEqual(rc, ODBQL_OK)

        rc = odbql_prepare_v2(db, "SELECT ALL * FROM foo;", -1, byref(stmt), byref(tail))
        self.assertEqual(rc, ODBQL_OK)

        number_of_rows = 0 
        rc = None
        while rc <> ODBQL_DONE:

            if number_of_rows == 0 or rc == ODBQL_METADATA_CHANGED:
                number_of_columns = odbql_column_count(stmt)
                print 'number_of_columns=', number_of_columns
                for i in range(0,number_of_columns):
                    print i, odbql_column_name(stmt, i) + ':' + type_name(odbql_column_type(stmt, i))
            else:
                self.assertEqual(rc, ODBQL_ROW)

                def value(column):
                    v = odbql_column_value(stmt, column)
                    if not v: return None
                    else: 
                        t = odbql_column_type(stmt, column)
                        if t == ODBQL_FLOAT: return odbql_value_double(v)
                        if t == ODBQL_INTEGER: return odbql_value_int(v)
                        if t == ODBQL_NULL: return None
                        #ODBQL_BLOB     = 4
                        #ODBQL_NULL     = 5
                        #ODBQL_TEXT     = 3
                        return odbql_column_text(stmt, column)

                print ','.join([str(value(column)) for column in range(number_of_columns)])
            number_of_rows += 1
            rc = odbql_step(stmt)


        rc = odbql_finalize(stmt)
        self.assertEqual(rc, ODBQL_OK)
        rc = odbql_close(db)
        self.assertEqual(rc, ODBQL_OK)


    def test_bitfields(self): # ODB-97
        print "TestODBQL.test_bitfields"
        db, stmt, tail = c_voidp(), c_voidp(), c_char_p()
        rc = odbql_open( "", byref(db))
        self.assertEqual(rc, ODBQL_OK)
        rc = odbql_prepare_v2(db, '''create table atovs on "ATOVS.trimmed.odb";''', -1, byref(stmt), byref(tail))
        self.assertEqual(rc, ODBQL_OK)
        rc = odbql_prepare_v2(db, '''select distinct qcinfo_1dvar from atovs order by 1 asc;''', -1, byref(stmt), byref(tail))
        self.assertEqual(rc, ODBQL_OK)
        
        rc = None
        values = []
        while True:
            rc = odbql_step(stmt)
            if rc == ODBQL_DONE:
                break
            v = odbql_column_value(stmt, 0)
            values.append(odbql_value_int(v))

        rc = odbql_finalize(stmt)
        self.assertEqual(rc, ODBQL_OK)
        rc = odbql_close(db)
        self.assertEqual(rc, ODBQL_OK)

        expected = [4198786,4198806,4202498,4202518,4202626,4202646,4210690,4210710,4210818,4210838,4210946,4211074,4211094,4243458,4243478,4243586,4243606,4243714,4243842,4243862,4264086,4264322,4264342,4268034,4268054,4268162,4268182,4276226,4276246,4276354,4276374,4276482,4276610,4308994,4309014,4309122,4309142,4309250,4309270,4309378,4309398,8404994,8405014,8405122,8405142,8405378,8405398,8458626,8458646,8462338,8462358,8470530,8470550,8470658,8470678]
        print 'values =', values
        self.assertEqual(values, expected)

    def test_stored_procedure(self):
        db, stmt, tail = c_voidp(), c_voidp(), c_char_p()

        rc = odbql_open("new_api_example_python.odb", byref(db))
        self.assertEqual(rc, ODBQL_OK)

        e = """ { compare, left = new_api_example_python.odb, right = new_api_example_python.odb } ; """
        rc = odbql_prepare_v2(db, e, -1, byref(stmt), byref(tail))
        self.assertEqual(rc, ODBQL_OK)

        rc = odbql_step(stmt)
        self.assertEqual(rc, ODBQL_DONE)

        print 'test_stored_procedure: OK!'


class TestPEP249(unittest.TestCase):

    def setUp(self):
        self.data = [[1,0.1, '  one   ', 1],
                     [2,0.2, '  two   ', 2],
                     [3,0.3, '  three ', 3],
                     [4,0.4, '  four  ', 4],
                    ]
        self.conn = connect("")


    def test_insert_data(self):
        c = self.conn.cursor()
        print self.data
        #c.execute(TEST_DDL)
        c.executemany(TEST_DDL + TEST_INSERT, self.data)


    def read_with_legacy_api(self, file_name = 'new_api_example_python.odb'):
        return  new_open(file_name)

    def test_select_data_fetchone(self):

        legacy = self.read_with_legacy_api()
        print 'legacy:', legacy

        c = self.conn.cursor()
        c.execute(TEST_DDL)
        c.execute(TEST_SELECT)
        number_of_rows = 0
        while True:
            row = c.fetchone()
            if row is None:
                break

            original = self.data [number_of_rows]
            # Legacy API does not handle bitfields correctly
            #self.assertEqual (original, legacy [number_of_rows])
            self.assertEqual (original, row)

            print row
            number_of_rows += 1

        self.assertEqual ( number_of_rows, 4 )


    def test_select_data_fetchall(self):
        """"""
        c = self.conn.cursor()
        c.execute(TEST_DDL)
        c.execute(TEST_SELECT)
        rows = [r for r in c.fetchall()]
        self.assertEqual ( len(rows), 4 )
        self.assertEqual ( rows, self.data )

    def test_select_data_iterate(self):
        """https://www.python.org/dev/peps/pep-0249/#iter"""
        c = self.conn.cursor()
        c.execute(TEST_DDL)
        c.execute(TEST_SELECT)
        self.assertEqual ( self.data,  [r for r in c] )

    def test_callproc(self):
        """https://www.python.org/dev/peps/pep-0249/#callproc"""
        c = self.conn.cursor()
        rc = c.callproc('compare', left = 'new_api_example_python.odb', right = 'new_api_example_python.odb')
        # TODO: check rc

    def test_bitfields(self): 
        """
        ODB-97 (old, Swig based API), ODB-250 (new API): Bitfield values read incorrectly.
        """
        print "TestPEP249.test_bitfields"
        # !odb sql select qcflags_info_1dvar    -i ATOVS.trimmed.odb | sort | uniq
        expected = [0,1,4,5,8,9,12,13,130,131,134,135,138,139,142,143,642]

        conn = odb.connect("ATOVS.trimmed.odb")
        c = conn.cursor()
        #c.execute('''create table atovs on "ATOVS.trimmed.odb";''')
        c.execute('''select distinct qcinfo_1dvar from "ATOVS.trimmed.odb" order by 1''')
        actual = [r[0] for r in c.fetchall()]
        # TODO: ODB-250
        #self.assertEqual(actual, expected) 

    def test_sorting_string_columns(self): # ODB-94 
        conn = odb.connect("") 
        c = conn.cursor()
        c.execute("""CREATE TABLE foo AS (statid string) ON 'test_sorting_string_columns.odb';""")
        data = [[w] for w in """12345678 abcdefgh dfgsdfgs DFADSFAD sdffffff aaaaaaaa""".split()]
        print data
        c.executemany('INSERT INTO foo (statid,x) VALUES (?,?);', data)
        conn.commit()

        ## TODO: the next three lines should not be needed, I think 
        #conn = odb.connect("") 
        #c = conn.cursor()
        #c.execute("""CREATE TABLE foo ON 'test_sorting_string_columns.odb';""")

        c.execute('''select statid from foo order by 1 asc;''')
        sql_sorted = [r[0] for r in c.fetchall()]
        python_sorted = sorted(sql_sorted)
        self.assertEqual ( sql_sorted, python_sorted )

    def test_sql_variables(self): # ODB-127
        conn = odb.connect("") 
        c = conn.cursor()
        c.execute("""CREATE TABLE test_sql_variables AS (varno integer, obsvalue real) ON 'test_sql_variables.odb';""")
        def cast(s):
            vals = s.split(',')
            return int(vals[0]), float(vals[1])
        data = [cast(w) for w in """1,0.1 2,0.2 3,0.3 4,0.4 1,0.11""".split()]
        print data
        c.executemany('INSERT INTO test_sql_variables (varno,obsvalue) VALUES (?,?);', data)
        conn.commit()

        c.execute('''select varno,obsvalue from test_sql_variables where varno = $z;''')
        result = [r[0] for r in c.fetchall()]
        self.assertEqual ( result, [1,1] )

"""
    def test_select_data_from_mars(self):
        conn = connect(ddl = '''
            CREATE TABLE foo 
            ON "mars://RETRIEVE,
                        DATABASE  = marsod,
                        CLASS     = OD,
                        TYPE      = MFB,
                        STREAM    = OPER,
                        EXPVER    = 0001,
                        DATE      = 20160830,
                        TIME      = 1200,
                        REPORTYPE = 16001 "''')
        c = conn.cursor()
        c.execute('SELECT * from foo;')
        data = c.fetchall()
        self.assertEqual(len(data), 4438) 

        c.execute('SELECT * from foo')
        data = c.fetchall()
        self.assertEqual(len(data), 4438) 
"""

if __name__ == '__main__':
    unittest.main()



