# (C) Copyright 1996-2012 ECMWF.
# 
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
# In applying this licence, ECMWF does not waive the privileges and immunities 
# granted to it by virtue of its status as an intergovernmental organisation nor
# does it submit to any jurisdiction.

""" 
Python interface for ODB API
Piotr Kuchta - ECMWF Nov 2011

The new, PEP-0249 conformant API is in module odbql, see its source for 
documentation and examples. Starting point is function connect.

Functions sql and open are a legacy, unsupported API, with limited functionality (read only). 

"""

from odbql import *

try:
    import pyodbapi

    def addSemicolon(s):
        """ Adds semicolon to the end of SQL statement if it's missing.
        """
        if not s.strip().endswith(';'):
            s += ';'
        return s;

    class Select:
        def __init__(self, sql):
            if not isinstance(sql, str):
                raise TypeError('sql must be a string object')
            self.sql = sql
            
    def sql(statement):
        """
        Executes an SQL statement. Returns an iterator over rows of the statement's result set.
        This is a legacy API. It is recommended to use the new, PEP-0249 conformant API, see
        function connect (module odbql.py)
        """
        if not isinstance(statement, str):
            raise TypeError('statement must be a string object')
        statement = addSemicolon(statement)
        return pyodbapi.Select(statement)

    def open(fileName):
        """
        Opens an ODB API file. Returns an iterator over rows in the file.
        This is a legacy API. It is recommended to use the new, PEP-0249 conformant API, see
        function connect (module odbql.py)
        """
        if not isinstance(fileName, str):
            raise TypeError('fileName must be a string object')
        return pyodbapi.Reader(fileName)

except ImportError:
    # In case we cannot import SWIG generated wrappers, use new ctypes based implementation.
    # Note, the functions sql and open are the old, legacy API. It is recommended to
    # use the new PEP-0249 conformant API, see function connect, imported here from module odbql. 
    # See source of odbql.py for dccumentation and examples.
    sql = new_sql
    open = new_open
