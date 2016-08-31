import odb

# See also https://software.ecmwf.int/wiki/display/ODB/Python+interface

test_file = '2000010106.odb'

def read_file(file_name = test_file):
    """ Read contents of a file without using SQL."""

    # Open a file and read rows in a loop.
    for row in odb.open(file_name):
        # Row object can be indexed with column names (strings).
        assert row['andate'] == 20000101
        # OK, one row is enough for the testing purposes.
        break

def sql_select(file_name = test_file):
    """ SQL filtering / calculations. """

    s = '''select varno,count(*),min(obsvalue),max(obsvalue) 
           from "%s"
           order by 2 desc''' % file_name
    for row in odb.sql(s):
        print ', '.join(map(str, row[:]))

def indexing_result_set_row(file_name = test_file):
    """ Various ways of indexing row object."""

    for row in odb.sql('select * from "%s"' % file_name):
        # Row can be indexed with a tuple containing column names
        expver, analysis_date, analysis_time = row['expver', 'andate', 'antime']
        print 'expver: "%s",  analysis date: %d analysis time: %d' % (expver, analysis_date, analysis_time)
        break

    for row in odb.sql('select lat,lon,varno,obsvalue from "%s"' % file_name):
        # Row can be indexed with integers representing position of the column in the result set,
        # starting from 0
        print row[0], row[1], row[2], row[3]
        # We can also index with a tuple containing several integers;
        # the result will be a tuple of values.
        print row[0,1,2,3]
        break

def reading_metadata_of_result_set(file_name = test_file):
    for row in odb.sql('select * from "%s"' % file_name):
        columns = [c.name() for c in row.columns()]
        break
    assert columns[:5] == ['expver@desc', 'andate@desc', 'antime@desc', 'seqno@hdr', 'obstype@hdr']
    

def main():
    read_file()
    sql_select()
    indexing_result_set_row()
    reading_metadata_of_result_set()

if __name__ == '__main__': main()
