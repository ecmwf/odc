# © Copyright 1996-2012 ECMWF.
# 
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
# In applying this licence, ECMWF does not waive the privileges and immunities 
# granted to it by virtue of its status as an intergovernmental organisation nor
# does it submit to any jurisdiction.

# Python ODB API
# Piotr Kuchta - ECMWF Nov 2011

import pyodbapi

class Select:
	def __init__(self, sql):
		if not isinstance(sql, str):
			raise TypeError('sql must be a string object')
		self.sql = sql
		

def sql(statement):
	if not isinstance(statement, str):
		raise TypeError('statement must be a string object')
	print "odb.sql: '" + statement + "'"
	return pyodbapi.Select(statement)

def open(fileName):
	if not isinstance(fileName, str):
		raise TypeError('fileName must be a string object')
	print "odb.open: '" + fileName + "'"
	return pyodbapi.Reader(fileName)

# ex file
fn = '../oda/some_conv_2011-01-08.odb'

