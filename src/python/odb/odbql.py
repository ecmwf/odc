"""
Python Database API (PEP 249) implementation for ODB API

@author Piotr Kuchta, ECMWF, August 2016

This module is a Python wrapper for ODB API, ECMWF library for encoding,
decoding and processing of observational data.

ODB API includes a streaming SQL engine and a MARS language syntax
based embedded scripting language, ECML. ECML verbs can be called using
this module as stored procedures via Cursor.callproc method.


Examples:

    # Select data from a file

    >>> import odbql
    >>> con = odbql.connect('')
    >>> c = con.cursor()
    >>> c.execute('''CREATE TABLE conv on 'conv.odb';''')
    >>> c.execute('select * from conv;')
    >>> c.fetchone()
        [262, '    0001', 1, 1025, 20160902, 120000, 16001, 3, 13, 11, 4287629, 0, 3, 17, 1, 14, None, '   89324', 20160902, 140000, 1, 0, 0, -80.0, -119.4000015258789, 1.0, 1534.8011474609375, 1530.0, None, 0, 1, 78120.0, 110, 2, 15004.1748046875, None, 1, 4096, 1, 3145728, 0, -200.0, -200.0, 106.85060119628906, 40.43746566772461, 0.44642725586891174, 0.0, 0.0, 53.77234649658203, 53.77234649658203, 19.578968048095703, 28.869945526123047, 80.12548828125]
    >>> print [d[0] for d in c.description]
        ['type', 'expver', 'class', 'stream', 'andate', 'antime', 'reportype', 'mxup_traj@desc', 'numtsl@desc', 'timeslot@timeslot_index', 'seqno@hdr', 'bufrtype@hdr', 'subtype@hdr', 'groupid@hdr', 'obstype@hdr', 'codetype@hdr', 'sensor@hdr', 'statid@hdr', 'date@hdr', 'time@hdr', 'report_status@hdr', 'report_event1@hdr', 'report_rdbflag@hdr', 'lat@hdr', 'lon@hdr', 'lsm@modsurf', 'orography@modsurf', 'stalt@hdr', 'sonde_type@conv', 'station_type@conv', 'entryno@body', 'obsvalue@body', 'varno@body', 'vertco_type@body', 'vertco_reference_1@body', 'vertco_reference_2@body', 'ppcode@conv_body', 'datum_anflag@body', 'datum_status@body', 'datum_event1@body', 'datum_rdbflag@body', 'biascorr@body', 'biascorr_fg@body', 'an_depar@body', 'fg_depar@body', 'qc_pge@body', 'fc_sens_obs@body', 'an_sens_obs@body', 'obs_error@errstat', 'final_obs_error@errstat', 'fg_error@errstat', 'eda_spread@errstat', 'hires@update_2']




"""

import os
from ctypes import *
import types
from sys import platform

apilevel = '2.0'
threadsafety = 1 # https://www.python.org/dev/peps/pep-0249/#threadsafety
paramstyle = 'qmark' # https://www.python.org/dev/peps/pep-0249/#paramstyle

def connect(file_name):
    """
    Returns a Connection object.

    Parameter: file_name  file to be open or empty string

    Examples:

        >>> conn1 = connect("conv.odb")

        >>> conn2 = connect("mars://RETRIEVE,DATABASE=marsod,CLASS=OD,TYPE=MFB,STREAM=OPER,EXPVER=0001,DATE=20160830,TIME=1200,REPORTYPE=16001")

    See also:

        https://www.python.org/dev/peps/pep-0249/#connect

    """
    return Connection(file_name)

def lib_extension():
    if 'linux' in platform: return '.so'
    if 'darwin' in platform: return '.dylib'
    if 'win' in platform: return '.DLL'
    raise Exception("Don't know lib extension for platform " + platform)

def __find_lib(paths, lib='libOdb', extension = lib_extension()):
    file_name = lib + extension
    for p in paths:
        path = p.split(os.sep)[:-1]
        for i in range(len(path)):
            pth = os.sep.join(path + ['..'] * i + ['lib', file_name])
            try:
                r = CDLL(pth)
                #print '__find_libOdb: FOUND', pth
                return r
            except OSError:
                #print '__find_libOdb: not found: ', pth
                pass
    raise Exception("Can't find " + file_name)

libodb = __find_lib([__file__, '/tmp/build/bundle/debug/bin'])

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
odbql_errmsg = libodb.odbql_errmsg
odbql_errmsg.restype = c_char_p

# odbql constants
ODBQL_OK               = 0
ODBQL_ROW              = 100
ODBQL_DONE             = 101
ODBQL_METADATA_CHANGED = 102

ODBQL_STATIC           = c_voidp(0)

ODBQL_INTEGER          = 1
ODBQL_FLOAT            = 2
ODBQL_TEXT             = 3
ODBQL_BLOB             = 4
ODBQL_NULL             = 5
ODBQL_BITFIELD         = 6

def type_name(i):
    return [None, 'INTEGER', 'REAL', 'TEXT', 'BLOB', 'NULL', 'BITFIELD'][i]

class Connection:
    def __init__(self, file_name):
        self.file_name = file_name
        self.db = c_voidp()
        rc = odbql_open(self.file_name, byref(self.db))
        # TODO
        #self.assertEqual(rc, ODBQL_OK)

    def close(self): pass
    def commit(self): pass
    # Not supported:
    #def rollback(self): pass

    def cursor(self):
        return Cursor(self.file_name, self)


class fetchall_generator(object):
    def __init__(self, cursor):
        self.cursor = cursor
    def __iter__(self): return self
    def __next__(self): return self.next()
    def next(self):
        v = self.cursor.fetchone()
        if not v:
            raise StopIteration()
        return v

class Cursor:

    def __init__(self, file_name, connection):
        self.file_name = file_name
        self.stmt = c_voidp(0)
        self.number_of_columns = None
        ## https://www.python.org/dev/peps/pep-0249/#id28
        self.connection = connection

        ## https://www.python.org/dev/peps/pep-0249/#description
        #
        #  This read-only attribute is a sequence of 7-item sequences.
        #
        # Each of these sequences contains information describing one result column:
        #   name, type_code, display_size, internal_size, precision, scale, null_ok
        self.description = []

    def __column_info(self, name, typ):
        return (name,
                typ,   # type_code ## TODO
                None,  # display_size
                None,  # internal_size
                None,  # precision
                None,  # scale
                True   # null_ok
                )

    def close(self):
        if self.stmt is not None:
            rc = odbql_finalize(self.stmt)
            assert rc == ODBQL_OK
            self.stmt = None

    def execute(self, operation, parameters = None):
        self.close()
        self.stmt, tail, db = c_voidp(0), c_char_p(0), self.connection.db

        operation = self.__add_semicolon_if_needed(operation)
        rc = odbql_prepare_v2(db, operation, -1, byref(self.stmt), byref(tail))
        if rc <> ODBQL_OK:
            err_msg = odbql_errmsg(db).strip()
            #print 'execute: odbql_prepare_v2 failed with error message: "%s"' % err_msg
            if err_msg == "syntax error":
                raise SyntaxError()

            if err_msg.startswith('Cannot open ') and err_msg.endswith('(No such file or directory)'):
                #'Cannot open non_existing.odb  (No such file or directory)'
                raise IOError("No such file or directory: '" + err_msg.split()[2] + "'")

            raise Exception('execute: prepare failed')

        self.__populate_meta_data()

    def __populate_meta_data(self):
        self.number_of_columns = odbql_column_count(self.stmt)
        self.names = [odbql_column_name(self.stmt, i) for i in range(self.number_of_columns)]
        self.types = [odbql_column_type(self.stmt, i) for i in range(self.number_of_columns)]
        self.description = map (self.__column_info, self.names, self.types)

    def fetchall(self):
        return fetchall_generator(self)

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

        if rc == ODBQL_METADATA_CHANGED or not self.description:
            self.__populate_meta_data()

        if not rc in (ODBQL_ROW, ODBQL_METADATA_CHANGED):
            return None

        r = [self.value(column) for column in range(self.number_of_columns)]
        return r

    def value(self, column):
        v = odbql_column_value(self.stmt, column)
        if not v: return None
        else:
            t = self.types[column]
            if t == ODBQL_FLOAT: return odbql_value_double(v)
            if t == ODBQL_INTEGER: return odbql_value_int(v)
            if t == ODBQL_TEXT: return odbql_column_text(self.stmt, column)
            if t == ODBQL_NULL: return None
            if t == ODBQL_BITFIELD: return odbql_value_int(v)
            #ODBQL_BLOB     = 4
            return odbql_column_text(self.stmt, column)

    def executemany(self, operation, parameters):
        """
        """

        self.close()
        self.stmt, tail, db = c_voidp(0), c_char_p(0), self.connection.db

        operation = self.__add_semicolon_if_needed(operation)
        rc = odbql_prepare_v2(db, operation, -1, byref(self.stmt), byref(tail))
        #self.assertEqual(rc, ODBQL_OK)

        for ps in parameters:
            self.__bind(ps)
            rc = odbql_step(self.stmt)
            #self.assertEqual(rc, ODBQL_ROW)

        #rc = odbql_finalize(self.stmt)
        #self.assertEqual(rc, ODBQL_OK)

    def callproc(self, procname, *parameters, **keyword_parameters):
        """
        Execute ECML verb
        """
        db, self.stmt, tail = c_voidp(), c_voidp(), c_char_p()
        rc = odbql_open(self.file_name, byref(db))

        operation = '{ ' + self.__marsify(procname, keyword_parameters) + ' }; '

        rc = odbql_prepare_v2(db, operation, -1, byref(self.stmt), byref(tail))
        rc = odbql_step(self.stmt)
        #rc = odbql_finalize(self.stmt)
        return ODBQL_OK


    def __marsify(self, procname, keyword_parameters):

        def marslist(l):
            if type(l) in (types.GeneratorType, types.ListType, types.TupleType):
                return '/'.join([str(x) for x in l])
            else:
                return str(l)

        r = procname + "".join ( [ ',' + k + '=' + marslist(v) for k,v in keyword_parameters.iteritems()] )
        return r


    def __bind(self, parameters):
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

    def __add_semicolon_if_needed(self, s):
        if s.strip().endswith(';'):
            return s
        else:
            return s + ';'


class __new_sql_generator:
    def __init__(self, cursor): self.cursor = cursor
    def __iter__(self): return self
    def __next__(self): return self.next()
    def next(self):
        v = self.cursor.fetchone()
        if not v:
            raise StopIteration()
        return v

## Support for legacy functions open and sql

class new_sql_row(object):

    def __init__(self, cursor):
        self.cursor = cursor
        self.name_to_index = None
        self.names = None

    def __getitem__(self, *indices):
        if len(indices) == 1:
            if type(indices[0]) == tuple:
                return tuple(self.__getitem__(i) for i in indices[0])
            else:
                return self.__get_one_item__(indices[0])

        return [self.__get_one_item__(i) for i in indices]

    def columns(self):
        class column_info:
            def __init__(self, name, typ):
                self.__name = name
                self.__typ = typ

            def name(self): return self.__name
            def type(self): return self.__typ

        return [column_info(c[0],c[1]) for c in self.cursor.description]


    def __get_one_item__(self, index):
        if type(index) == int: return self.cursor.value(index)
        if type(index) == str: return self.__value_by_name(index)
        if type(index) == tuple: return tuple(self.__get_one_item__(i) for i in index)
        if index == slice(None,None,None):
            return [self.cursor.value(i) for i in range(len(self.cursor.description))]
        if type(index) == slice:
            return [self.__get_one_item__(i) for i in [t[0] for t in enumerate(self.cursor.description)][index.start : index.stop : index.step]]

        raise TypeError('__get_one_item__: index == ' + str(index))

    def __value_by_name(self, index):
        if self.name_to_index is None:
            self.name_to_index = dict([(self.cursor.description[i][0], i) for i in range(len(self.cursor.description))])
            self.names = [self.cursor.description[i][0] for i in range(len(self.cursor.description))]
        try:
            return self.cursor.value(self.name_to_index[index])
        except KeyError:
            simillar_columns = [n for n in self.names if n.startswith(index)]

            if len(simillar_columns) == 1 and simillar_columns[0].startswith(index + '@'):
                self.name_to_index[index] = self.name_to_index[simillar_columns[0]]
                return self.cursor.value(self.name_to_index[index])

            msg = simillar_columns and \
                  'could be: ' + ','.join(simillar_columns) \
                  or 'should be one of: ' + ','.join(self.names)

            raise KeyError(str(index) + ', ' + msg)


class new_sql_generator(object):
    def __init__(self, cursor):
        self.cursor = cursor
    def __iter__(self): return self
    def __next__(self): return self.next()
    def next(self):
        if not self.cursor.stmt:
            raise Exception('execute must be called first')

        rc = odbql_step(self.cursor.stmt)
        if not rc in (ODBQL_ROW, ODBQL_METADATA_CHANGED):
            raise StopIteration()
        return new_sql_row(self.cursor)

def new_sql(s):
    conn = connect("")
    c = conn.cursor()
    c.execute(s)
    return new_sql_generator(c)

def new_open(fn):
    s = '''select all * from "{}";'''.format(fn)
    conn = connect("")
    c = conn.cursor()
    c.execute(s)
    return new_sql_generator(c)
