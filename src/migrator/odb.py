#!/usr/local/bin/python
# © Copyright 1996-2012 ECMWF.
# 
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
# In applying this licence, ECMWF does not waive the privileges and immunities 
# granted to it by virtue of its status as an intergovernmental organisation nor
# does it submit to any jurisdiction.

import re
import os
import subprocess

def execute(cmd):
	p = subprocess.Popen(cmd, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
	errcode = p.wait()
	out = p.stdout.read()
	errmess = p.stderr.read()
	if errcode:
		print 'cmd "', cmd, '" failed <%s>: %s' % (errcode, errmess)
	return out
	

def codes(datapath='/hugetmp/data/2009072800',
          dbs='conv airs amsre amsua amsub geos gpsro hirs iasi meris mhs reo3 satob scatt ssmi tmi'.split(),
          #dbs='conv'.split()
          SQL = "select distinct sensor, satob[1:4],bufrtype,subtype,obstype,codetype from hdr,sat"):
	records = []

	for db in dbs:
		cmd = 'odbsql -f dump -q "' + SQL + '" -i %s/ECMA.%s' % (datapath, db)
		print 
		print db

		lines = execute(cmd).splitlines()
		md = [w.split(':')[1].split('@')[0] for w in lines[0].split(',')]
		for line in lines[1:]:
			record = {}
			row = [w.replace("'","") for w in line.split(',')]
			assert len(md) == len(row)
			#print row
			for i in range(0, len(row)):
				record[md[i]] = row[i]

			records.append(record)
	return records

tableMD  = "reportype reportype_name group group_name sensor sensor_name platform platform_name method method_name"
tableMD += " bufrtype bufrtype_name subtype subtype_name obstype obstype_name codetype codetype_name"
tableMD = tableMD.split()

cfgMD = "sensor,satob_1,satob_2,satob_3,satob_4,bufrtype,subtype,obstype,codetype".split(',')

def f(s):
	if not s: return 'NULL'
	if s == 'N/A': return 'NULL'
	try: return str(int(s))
	except:	return "'" + s + "'"
	raise 'Unrecognized: ' + s

def table(fn = "../obsdb/all_table_AG.tex"):
	f = open(fn)
	s = f.read()
	f.close()

	lines = s.splitlines()
	records = []
	for s in lines:
		if len(s) and not s[0] in "0123456789":
			continue
		s, n = re.subn("\\\\textbf[{]\\\\color[{][a-z]+[}][{]", "", s)
		s, n = re.subn("[}][}]", "", s)
		s, n = re.subn("\\\\", "", s)
		record = s.split("&")
		record = map(lambda x: x.strip(), record)

		r = {}
		for i in range(0, len(record)):
			r[tableMD[i]] = record[i]
		records.append(r)
	return records

#cs = codes()
tab = table()


def parse_satob(s):
	r = {}
	ws = s.split()
	for w in ws:
		ass = w.split('=')
		if len(ass) == 2:
			r[ass[0]] = ass[1]
	return r

for row in codes(dbs='satob'.split(), SQL = "select distinct satob[1:4] from hdr"):
	s = row['satob_1'] + row['satob_2'] + row['satob_3'] + row['satob_4']
	print s, parse_satob(s)

