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
import re

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


def type_to_name(n):
    if n == 1: return 'INTEGER'
    if n == 2: return 'REAL'
    if n == 3: return 'STRING'
    if n == 4: return 'BITFIELD'
    raise 'Unknown type ' + str(n)

def as_DataFrame(statement, convert_time=True):
    import pandas as pd
    import numpy as np
    for row in sql(statement):
        columns = [c.name() for c in row.columns()]
        types = [c.type() for c in row.columns()]
        print "\n".join([t[0] + ':' + type_to_name(t[1]) for t in zip(columns, types) if type_to_name(t[1]) == 'BITFIELD'])
        break
    rows = [r[:] for r in sql(statement)]
    xx = pd.DataFrame.from_records(rows, columns = columns)
    xx.rename(columns=lambda x: re.sub('@[^ ]*$','',x).strip(), inplace=True)
    if 'statid' in xx.columns:
        xx['statid'] = xx.statid.apply(lambda x: re.sub("'",'',x).strip())
    if convert_time:
        xx['date'] = pd.to_datetime(xx['date'].astype(str),format='%Y%m%d')
        xx['time'] = (xx['time']//10000)*3600.+ (np.mod(xx['time'],10000)//100)*60+(np.mod(xx['time'],100))*60
        # Add datetime
        xx['datetime']=(xx['date'].values + xx['time'].values.astype('timedelta64[s]'))
    return xx

# TODO: a proper test
#df = as_DataFrame('select * from "conv.odb";')
#print df['report_status']


