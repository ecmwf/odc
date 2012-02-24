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

