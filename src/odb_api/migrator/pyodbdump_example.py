import pyodbdump
db = '/tmp/new_migrator/ECMA.conv'
sql = open('/tmp/new_migrator/ECMA.conv/bigger_query.sql').read()

#rows = [r [:] for r in pyodbdump.ODBReader(db, sql)]

def columns(db, sql):
    for r in pyodbdump.ODBReader(db, sql):
        print dir(r)
        return r

print columns(db, sql)
