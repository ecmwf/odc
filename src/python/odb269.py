import odb

conn = odb.connect('')
c = conn.cursor()
c.execute('''
    CREATE TABLE foo AS
    ( x INTEGER, y DOUBLE, v STRING)
    ON 'test_odb269.odb';
    ''')
c.executemany('INSERT INTO foo (x,y,v,status) VALUES (?,?,?);',
               [[1,0.1, '  one   '],
                [2,0.2, '  two   '],
                [3,0.3, '  three '],
                [4,0.4, '  four  ']])

conn.commit()

for i in range(100000):
    print str(i) + '.'
    c.execute('SELECT count(*) from foo')
    for row in c.fetchall():
        print row[0]
    c.close()
