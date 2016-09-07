#!/usr/bin/env python 

import os
print '*** ', __file__, 'CWD:', os.getcwd()

import odb
import unittest 
import legacy_odb_api_python_examples as examples

def open_non_existing_file(file_name = 'a_non_existin_file.odb', f = lambda fn: odb.open(fn)):
    didThrow = False
    try:
        for row in f(file_name):
            pass
    except (IOError, Exception) as e:
        didThrow = True
        #print "I/O error({0}): {1}, e.args: {2}".format(e.errno, e.strerror, e.args)
    assert didThrow and "Opening of a non existing file was supposed to fail with IOError, see ODB-93" 

def read_non_existing_file(): 
    return open_non_existing_file()

def select_non_existing_file(): 
    return open_non_existing_file(f = lambda fn: odb.sql('select * from "%s"' % fn))

class TestODBAPI(unittest.TestCase):
    def setUp(self):
        pass

    def test_open_non_existing_file(self):
        with self.assertRaises(IOError) as ex:
            for row in odb.sql('select * from "non_existing_file.odb";'):
                pass

    def test_syntax_error_throws_exception(self):
        with self.assertRaises(SyntaxError) as ex:
            for row in odb.sql('select * from '):
                pass

    def test_odb_module_appends_semicolon_if_needed(self):
        """ Check that semicolon at the end of SQL statement is not obligatory (ODB-114)
            Expect IOError, and not SyntaxError.
        """
        with self.assertRaises(IOError) as ex:
            for row in odb.sql('select * from "non_existing_file.odb"'):
                pass

    def test_examples_read_file(self): examples.read_file();
    def test_examples_read_file(self): examples.read_file();
    def test_examples_sql_select(self): examples.sql_select();
    def test_indexing_result_set_row(self): examples.indexing_result_set_row()
    def test_reading_metadata_of_result_set(self): examples.reading_metadata_of_result_set()
    def test_reading_non_existing_file(self): read_non_existing_file()
    def test_selecting_non_existing_file(self): select_non_existing_file()

if __name__ == '__main__':
    unittest.main()
