import os, subprocess, multiprocessing

exe = "/tmp/p4/source/main/build/Debug/bin/odb"
exe = "/marsdev/data/p4/odb_api_dev/development/build/Production/bin/odb"

def mergeBlocks(blocks, maxBlockSize):
	currentOffset, currentLength, currentNumberOfRows  = blocks[0]
	for offset, length, nrows in blocks[1:]:
		if (currentLength + length) > maxBlockSize:
			yield currentOffset, currentLength, currentNumberOfRows
			currentOffset = offset
			currentLength = length
			currentNumberOfRows = nrows
		else:
			currentLength += length
			currentNumberOfRows += nrows
	yield currentOffset, currentLength, currentNumberOfRows

def getBlocks(fileName):
	l = [exe, "header", "-offsets", fileName]
	blocks = subprocess.check_output(l)
	blocks = [map(int, l.split()) for l in blocks.split('\n') if l]
	return blocks

def divideFile(fileName, maxBlockSize = None, numberOfProcessors = None):
	assert maxBlockSize or numberOfProcessors
	blocks = getBlocks(fileName)
	if maxBlockSize:
		blocks = [b for b in mergeBlocks(blocks, maxBlockSize)]
	return blocks
	
def outputFileName(offset, length, nrows):
	return 'out_' + offset + '_' + length + '.odb'

def filterPartOfFile(inputFile, fileChunk, select = '*', where = ''):
	offset, length, nrows = map(str, fileChunk)
	outputFile = outputFileName(offset, length, nrows)
	sql = "select " + select + " into " + '"' + outputFile  + '"'
	if where: sql += ' where ' + where
	l = [exe, "sql", "-i", inputFile, "-offset", offset, "-length", length, sql]
	print 'filterBlock: running ', l
	subprocess.call(l) 
	return outputFile

def filterBlock(p): return filterPartOfFile(*p)

def sql(processes = 2, inputFile = None, outputFile = None, select='*', where = ''):
	assert inputFile and outputFile
	pool = multiprocessing.Pool(processes = processes)
	blocks = [(inputFile, t, select, where) for t in divideFile(inputFile, maxBlockSize = 1024*1024*200)]
	outputFiles = pool.map(filterBlock, blocks)
	os.system("cat " + " ".join(outputFiles) + " >" + outputFile)

if __name__ == '__main__':
	sql(processes = 5,
		inputFile = "ofb_1656_20040602to20040603.odb", #"ofb_1657_20040602to20040603.odb",
		select = "source,count(*) as counts",
		outputFile = 'out.odb')
	subprocess.call([exe, "sql", "-i", "out.odb", "select source,sum(counts)"])
