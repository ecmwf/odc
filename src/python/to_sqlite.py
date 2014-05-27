import sys, os
import sqlite3

sys.path.insert(0, '/usr/local/apps/odb_api/0.9.31/lib/python2.7/site-packages/')
import odb

def typeName(typ):
    return {
        0 : 'IGNORE',
        1 : 'INTEGER',
        2 : 'REAL',
        3 : 'STRING',
        4 : 'BITFIELD',
        5 : 'DOUBLE',
        }[typ]

def stripTableName(columnName):
    return columnName.split('@')[0]

def metadata(odbFileName):
    f = odb.open(odbFileName)
    for row in f:
        columns = [(stripTableName(c.name()), typeName(c.type())) for c in row.columns()]
        return columns


def createTable(tableName, columns):
    colDefs = [' '.join(c) for c in columns]
    return 'CREATE TABLE ' + tableName + ' (' + ', '.join(colDefs) + ')'

def convert(inputFileName, outputFileName):
    #os.remove(outputFileName)
    conn = sqlite3.connect(outputFileName)
    c = conn.cursor()
    tableName = inputFileName.split('.')[0]
    columns = metadata(inputFileName)
    ct = createTable(tableName, columns)
    try:
        c.execute(ct)
        print ct
    except:
        dt = 'DELETE FROM ' + tableName
        c.execute(dt)
        print dt

    def rows():
        for row in odb.open(inputFileName):
            yield row[:]

    print 'Copying data from', inputFileName, 'into', outputFileName

    c.executemany('INSERT INTO ' + tableName + ' VALUES (' + ','.join('?' for c in columns) + ')', rows())
    conn.commit()
    conn.close()

if __name__ == '__main__':
    inputFileName = sys.argv[1]
    outputFileName = sys.argv[2]
    convert(inputFileName, outputFileName)

