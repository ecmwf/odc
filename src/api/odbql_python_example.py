#!/usr/bin/env python 

"""
Examples of usage of ODB API Python interface.

@author Piotr Kuchta, ECMWF, August 2016 
"""

import odb

""" Example 1. Create a new ODB file.
"""

""" 1.1. Define metadata (column names and their types) 
    of a new table associated with a physical file 
    by passing some DDL (Data Definition Language):
    CREATE TYPE (to define bitfields) and CREATE TABLE
    statements to function connect.

    Function connect returns a Connection object.
"""
conn = odb.connect('''
    CREATE TYPE bf AS (f1 bit1, f2 bit2); 

    CREATE TABLE foo AS 
    ( x INTEGER, y DOUBLE, v STRING, status bf) 
    ON 'new_api_example.odb';
''')

""" 1.2 Create a Cursor object. """
c = conn.cursor()

""" 1.3 Populate table with data using SQL INSERT statement 
    and method Cursor.executemany. 
"""
c.executemany('INSERT INTO foo (x,y,v,status) VALUES (?,?,?,?);', 
                [[1,0.1, '  one   ', 1], 
                 [2,0.2, '  two   ', 2], 
                 [3,0.3, '  three ', 3], 
                 [4,0.4, '  four  ', 4]])
""" 1.4 Flush buffers and write to file associated with the table. """
conn.commit()


"""Example 2. Read contents of a file.

   Note: table foo is associated with a file using CREATE TABLE
   DDL statement, see previous example.
"""

""" 2.1 Create Cursor object and execute
"""
                 
c = conn.cursor()
c.execute('select * from foo;')

""" 2.2 Use Cursor.fetchall to retrieve whole result set of SELECT
    and print its rows using simple print 
"""
for row in c.fetchall():
    print ",".join(str(v) for v in row)


""" Example 3. Read a file into Pandas DataFrame object.
"""

import pandas as pd
c.execute('select * from foo;')
d = pd.DataFrame.from_records(c.fetchall(), 
                              columns = [d[0] for d in c.description],
                              exclude = ['v@foo'])

print 'Pandas DataFrame:\n', d

""" Example 4. Create numpy array.
"""
import numpy as np

c.execute('select x, y, status.f1, status.f2 from foo;')
a = np.array(c.fetchall())
print 'numpy array:\n', a


""" Example 5. Load some data from MARS or ODB Server into Pandas DataFrame
"""
conn = odb.connect(ddl = '''
    CREATE TABLE rt16001
    ON "mars://RETRIEVE,
                DATABASE  = marsod,
                CLASS     = OD,
                TYPE      = MFB,
                STREAM    = OPER,
                EXPVER    = 0001,
                DATE      = 20160830,
                TIME      = 1200,
                REPORTYPE = 16001";
''')
c = conn.cursor()
c.execute('select * from rt16001;')
d = pd.DataFrame.from_records(c.fetchall(), 
                              columns = [d[0] for d in c.description],
                              exclude = ['expver','class','stream'])
print d

print "That's all, folks!"
