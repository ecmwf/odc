import odb
import pyodbapi
import unittest

class TestPython(unittest.TestCase):

	def setUp(self):
		self.fn = '../oda/some_conv_2011-01-08.odb'
		self.bitfields=[19, 20, 21, 30, 31, 32, 33,]

	def testSelectFromFile(self):
		sql = 'select lat,lon,obsvalue from "' + self.fn + '"'
		self.assertTrue( 15719 == len([r for r in odb.sql(sql)]) )

	def testOpenFile(self):
		o = odb.open(self.fn)
		allRows = 0

		for r in o:
			stream1, lat1, lon1 = r[0, 'lat','lon']

			stream2, (lat2, lon2, obsvalue2) = r[(0, ('lat','lon', 'obsvalue'))]
			self.assertTrue( (stream1, lat1, lon1) == (stream2, lat2, lon2) )

			allRows += 1
			if allRows > 50: break

		rr = r[:]
		self.assertTrue (rr[1:10:2] == r[1:10:2]) 
		self.assertTrue (rr[2:10:3] == r[2:10:3])
		self.assertTrue (rr[10:1:-1] == r[10:1:-1])
		self.assertTrue (rr[10:1:1] == r[10:1:1])
		self.assertTrue (rr[-1:1:1] == r[-1:1:1])

		md = r.columns()
		self.assertTrue(49, len(md))
		for c in md:
			print c.name(), c.type(), c.missingValue()
			self.assertTrue(c.name() == 'expver')
			self.assertTrue(c.type() == 3)
			self.assertTrue(c.missingValue() == -2147483647.0)
			break

	def testSyntaxError(self):
		def rows(s): return [r[:] for r in odb.sql(s)]
		self.assertRaises(SyntaxError, rows, 'foo bar')

	def testBitfields(self):
		"""
		TODO:
		"""
		for r in odb.open(self.fn):
			print 'elements: ', ",".join([ str(i) + ':' + str(r[i]) for i in self.bitfields])
			print r[0:44]
			break

if __name__ == '__main__':
	unittest.main()
