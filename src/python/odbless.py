import sys
import odb

def quote(s): return '"%s"' % s

statsFunctions = "count,min,max,avg,rms".split(',')

def genStatSQL(columnNames, fs = statsFunctions):
	def stats(column):
		return ",".join([f + '(%(column)s)' for f in fs]) % locals()
	select_list = ",".join([stats(c) for c in columnNames])
	return 'select %s' % select_list

def chunks(l, n):
	for i in xrange(0, len(l), n):
		yield l[i:i+n]

def getColumns(dataFile):
	f = odb.open(dataFile)
	for r in f:
		return [(c.name(), c.type()) for c in r.columns()]

def printStats(dataFile, fs = statsFunctions):
	columns = getColumns(dataFile)
	maxColumn = max([len(c[0]) for c in columns])
	columnFormat = '%-' + str(maxColumn) + 's'
	valueFormat = '%20s'
	sql = genStatSQL([c[0] for c in columns], fs) + ' from ' + quote(dataFile)
	for r in odb.sql(sql):
		print columnFormat % 'column', "".join([valueFormat % v for v in fs])
		values = [str(v) for v in r[:]]
		for (c, vs) in zip(columns, chunks(values, len(fs))):
			#if c[1] in [3,4]: vs = ['NA' for v in vs]
			print columnFormat % c[0], "".join([valueFormat % v for v in vs])

if __name__ == '__main__':
	if len(sys.argv) == 1:
		print 'Usage:\n\t', sys.argv[0], ' <fileName>+'
	else:
		for fn in sys.argv[1:]:
			printStats(fn)
