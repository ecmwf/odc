#!/usr/bin/env python 

import odbql

conn = odbql.connect('''
    CREATE TYPE bf AS (f1 bit1, f2 bit2); 

    CREATE TABLE foo AS 
    ( x INTEGER, y DOUBLE, v STRING, status bf) 
    ON 'new_api_example.odb';
''')

c = conn.cursor()
c.executemany('INSERT INTO foo (x,y,v,status) VALUES (?,?,?,?);', 
                [[1,0.1, '  one   ', 1], 
                 [2,0.2, '  two   ', 2], 
                 [3,0.3, '  three ', 3], 
                 [4,0.4, '  four  ', 4]])
conn.commit()
                 
c = conn.cursor()
c.execute('select * from foo;')

for row in c.fetchall():
    print ",".join(str(v) for v in row)

import pandas as pd
c.execute('select * from foo;')
d = pd.DataFrame.from_records(c.fetchall(), 
                              columns = [d[0] for d in c.description],
                              exclude = ['v@foo'])

print 'Pandas DataFrame:\n', d

import numpy as np

c.execute('select x, y, status.f1, status.f2 from foo;')
a = np.array(c.fetchall())
print 'numpy array:\n', a

"""Load some data from Mars or ODB Server into Pandas DataFrame
"""

conn = odbql.connect(ddl = '''
    CREATE TABLE rt16001
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
c.execute('select * from rt16001;')
d = pd.DataFrame.from_records(c.fetchall(), 
                              columns = [d[0] for d in c.description],
                              exclude = [])
print d

print "That's all, folks!"
