# (C) Copyright 1996-2012 ECMWF.
# 
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
# In applying this licence, ECMWF does not waive the privileges and immunities 
# granted to it by virtue of its status as an intergovernmental organisation nor
# does it submit to any jurisdiction.

# Python ODB API
# Piotr Kuchta - ECMWF Nov 2011

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
    if not isinstance(statement, str):
        raise TypeError('statement must be a string object')
    statement = addSemicolon(statement)
    return pyodbapi.Select(statement)

def open(fileName):
    if not isinstance(fileName, str):
        raise TypeError('fileName must be a string object')
    return pyodbapi.Reader(fileName)

