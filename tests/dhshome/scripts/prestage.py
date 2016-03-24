#!/usr/bin/env python

import sys, os, re
import urllib2

if all(os.path.exists(fn) for fn in sys.argv[1:]):
    print sys.argv[0] +': all files', sys.argv[1:], 'exist' 
    os._exit(0)

"""
 els /emos_backup/an/0001/2012061900/* 
 ECMA.airs.tar
 ECMA.amsua.tar
 ECMA.amsub.tar
 ECMA.audit
 ECMA.conv.tar
 ECMA.geos.tar
 ECMA.gpsro.tar
 ECMA.hirs.tar
 ECMA.iasi.tar
 ECMA.mhs.tar
 ECMA.mwhs.tar
 ECMA.mwts.tar
 ECMA.nexrad.tar
 ECMA.resat.tar
 ECMA.satob.tar
 ECMA.scatt.tar
 ECMA.ssmi.tar
 ECMA.ssmis.tar
 ECMA.surf_conv.tar
 ECMA.tmi.tar
 ECMA.windsat.tar
"""
emosBackup = "/emos_backup/an/{expver}/{date}{time}/ECMA.{marsname}.tar" # marsname is group's short name

#print 'Hello from ', sys.argv[0], ". I'm not going to do anything with file(s) ", sys.argv[1:]

def dhshome():
    return os.getenv('DHSHOME') or os.getenv('TEST_DHSHOME') 

def localConfig():
    "$DHSHOME/etc/config/local"
    home = dhshome()
    return home and os.sep.join( (home, 'etc', 'config', 'local') ) or None

def odbPathNameSchema():
    # default from odbsvr/ODBRetriever.cc
    schema = ":{class}:{stream}:{expver}:{date}/:{time}:{type}:{groupid}/{reportype}.odb"
    cfg = localConfig()
    if cfg:
        lines = [l for l in open(cfg).readlines() if l.find('odbPathNameSchema') <> -1]
        schema = lines[0].split(':')[1].strip().strip('"')
    return schema

def decodeFileName(fileName, pathNameSchema = odbPathNameSchema()):
    r = pathNameSchema
    r = re.sub('[{]', '(?P<', r)
    r = re.sub('[}]', '>.*)', r)
    r = '.*/' + r
    print 'fileName:', fileName
    print 'RE:', r
    d = {'class': 'od', 'type' : 'ofb', 'stream' : 'oper', 'expver' : '0001'}
    d.update(re.search(r, fileName).groupdict())
    return d

def encodeFileName (d, template):
    s = template
    for k,v in d.iteritems():
        s = re.sub('{%s}' % k, str(v), s)
    return s

# http://data-portal.ecmwf.int/odbgov/csv/ReportType/
""" code&uarr;    ; group    ; description    ;    
     1  ; HIRS  ; TIROS-N HIRS Radiances  ;    
     2  ; HIRS  ; NOAA 6 HIRS Radiances  ;    
"""
def reportType2group(rt, fn = '/usr/local/apps/odb_api/codes/report_type.txt'):
    for line in open(fn).readlines():
        r = [v.strip() for v in line.split(';')]
        if r[0] == str(rt):
            print 'reportType2group("'+ rt + '") => ', r[1]
            return r[1] # group
    
# http://data-portal.ecmwf.int/odbgov/csv/Group/
""" id&uarr;    ; name    ; kind_id    ; marsname    ; description    ;    
     1  ; HIRS  ; 2  ; HIRS  ;    ;    
     2  ; AMSUA  ; 2  ; AMSUA  ;    ;   
"""
def group2marsname(group, fn = '/usr/local/apps/odb_api/codes/group.txt'):
    for line in open(fn).readlines():
        r = [v.strip() for v in line.split(';')]
        if r[1] == str(group):
            print 'group2marsname("'+ group + '") => ', r[0]
            return r[3] # marsname

for fn in sys.argv[1:]:
    d = decodeFileName(fn)
    d['marsname'] = group2marsname(reportType2group(d['reportype'])).lower()
    ecfs = encodeFileName(d, emosBackup)
    print fn, '=> (', d, ')', ecfs
