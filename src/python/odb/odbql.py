# Python Database API (PEP 249) implementation for ODB API
#
# @author Piotr Kuchta, ECMWF, August 2016


import os
os.sys.path.insert(0, '/tmp/build/bundle/debug/odb_api/src/python/odb') #/pyodbapi.py

import unittest, os
from ctypes import *

apilevel = '2.0' 
threadsafety = 1 # https://www.python.org/dev/peps/pep-0249/#threadsafety
paramstyle = 'qmark' # https://www.python.org/dev/peps/pep-0249/#paramstyle

def connect(ddl=''):
    """
    Returns a Connection object. 
    
    Keyword arguments:

        ddl  -- (Optional) string with DDL (Data Definition Language)

    Examples:

        >>> conn1 = connect(ddl='''CREATE TABLE bar on "conv.odb";'''
        >>> conn2 = connect(ddl='''
            CREATE TABLE foo ON "mars://RETRIEVE,DATABASE=marsod,CLASS=OD,TYPE=MFB,STREAM=OPER,EXPVER=0001,DATE=20160830,TIME=1200,REPORTYPE=16001";''')

    See also:
    
        https://www.python.org/dev/peps/pep-0249/#connect 
    
    """
    return Connection(ddl)


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
odbql_column_value = libodb.odbql_column_value
odbql_column_text = libodb.odbql_column_text
odbql_column_text.restype = c_char_p  # TODO: should be unsigned: const unsigned char *odbql_column_text(odbql_stmt* stmt, int iCol)
odbql_column_type = libodb.odbql_column_type
odbql_value_double = libodb.odbql_value_double
odbql_value_double.restype = c_double
odbql_value_int = libodb.odbql_value_int

# odbql constants
ODBQL_OK = 0
ODBQL_ROW = 100
ODBQL_STATIC = c_voidp(0)

ODBQL_INTEGER  = 1
ODBQL_FLOAT    = 2
ODBQL_BLOB     = 4
ODBQL_NULL     = 5
ODBQL_TEXT     = 3

class Connection:
    def __init__(self, ddl):
        self.ddl = ddl

    def close(self): pass
    def commit(self): pass
    # Not supported
    #def rollback(self): pass

    def cursor(self):
        return Cursor(self.ddl, self)

class Cursor:

    def __init__(self, ddl, connection):
        self.ddl = ddl
        self.stmt = None
        self.number_of_columns = None
        #https://www.python.org/dev/peps/pep-0249/#id28
        self.connection = connection

        # https://www.python.org/dev/peps/pep-0249/#description
        #
        #  This read-only attribute is a sequence of 7-item sequences.
        #
        # Each of these sequences contains information describing one result column:     
        #   name, type_code, display_size, internal_size, precision, scale, null_ok
        self.description = []

    def __column_info(self, name, typ):
        return (name, 
                typ,   # type_code
                None,  # display_size
                None,  # internal_size
                None,  # precision
                None,  # scale
                True   # null_ok
                )
        
    def close(self):
        rc = odbql_finalize(self.stmt)
        self.stmt = None

    def execute(self, operation, parameters = None):
        print 'execute:', operation, ', parameters: ', parameters
        db, self.stmt, tail = c_voidp(0), c_voidp(), c_char_p()
        rc = odbql_open(self.ddl, byref(db))
        rc = odbql_prepare_v2(db, operation, -1, byref(self.stmt), byref(tail))
        if rc <> ODBQL_OK:
            raise Exception('execute: prepare failed')

        self.number_of_columns = odbql_column_count(self.stmt)

        self.names = [odbql_column_name(self.stmt, i) for i in range(self.number_of_columns)]
        self.types = [odbql_column_type(self.stmt, i) for i in range(self.number_of_columns)]

        self.description = map (self.__column_info, self.names, self.types)

        print '.description: ', self.description
        
    def fetchall(self):
        r = []
        while True:
            v = self.fetchone()
            if not v: 
                break
            r.append(v)
        return r

    def __iter__(self): return self

    def next(self):
        r = self.fetchone()
        if r:
            return r
        raise StopIteration()

    def fetchone(self):
        if not self.stmt:
            raise Exception('fetchone: you must call execute first')

        rc = odbql_step(self.stmt)
        # TODO: handle ODBQL_CHANGED_METADATA
        if rc <> ODBQL_ROW:
            return None

        def value(column):
            v = odbql_column_value(self.stmt, column)
            if not v: return None
            else: 
                t = self.types[column]
                if t == ODBQL_FLOAT: return odbql_value_double(v)
                if t == ODBQL_INTEGER: return odbql_value_int(v)
                if t == ODBQL_NULL: return None
                #ODBQL_BLOB     = 4
                #ODBQL_NULL     = 5
                #ODBQL_TEXT     = 3
                return odbql_column_text(self.stmt, column)

        r = [value(column) for column in range(self.number_of_columns)]
        return r


    def executemany(self, operation, parameters):
        db, self.stmt, tail = c_voidp(), c_voidp(), c_char_p()

        rc = odbql_open(self.ddl, byref(db))
        #self.assertEqual(rc, ODBQL_OK)

        rc = odbql_prepare_v2(db, operation, -1, byref(self.stmt), byref(tail))
        #self.assertEqual(rc, ODBQL_OK)

        for ps in parameters:
            self._bind(ps)
            rc = odbql_step(self.stmt)
            #self.assertEqual(rc, ODBQL_ROW)

        rc = odbql_finalize(self.stmt)
        #self.assertEqual(rc, ODBQL_OK)
        rc = odbql_close(db)
        #self.assertEqual(rc, ODBQL_OK)
            

    def _bind(self, parameters):
        for i in range(len(parameters)):

            p = parameters[i]

            if p is None:
                rc = odbql_bind_null(self.stmt, i)
                #self.assertEqual(rc, ODBQL_OK)

            if type(p) == str:
                rc = odbql_bind_text(self.stmt, i, p, len(p), ODBQL_STATIC)
                #self.assertEqual(rc, ODBQL_OK)
            elif type(p) == int: 
                rc = odbql_bind_int(self.stmt, i, p)
                #self.assertEqual(rc, ODBQL_OK)
            elif type(p) == float: 
                rc = odbql_bind_double(self.stmt, i, c_double(p))
                #self.assertEqual(rc, ODBQL_OK)
            else:
                raise "Don't know how to bind parameter " + str(p) + ' of type ' + str(type(p))

