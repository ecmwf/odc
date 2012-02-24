CC=$(CFLAGS)
DEPLIB= ../lib/libEc.a 
INCLUDE=-I../eclib
BINLIB=-L../lib -lEc 
LIBLIB=-L../lib -lEc 

# ------------------- Python
#---------------------------------------------------------------------------------------------------------------------
PYVER					=	2.7
PYVER_MINOR				=	1
PYTHON					=	/usr/local/apps/python/$(PYVER)
#---------------------------------------------------------------------------------------------------------------------

SHLIB.TARGET=../lib/libOdb.a 

SRC.cc= \
ODBAPIVersion.cc \
StringTool.cc \
md5.cc \
MD5.cc \
InMemoryDataHandle.cc \
TReadOnlyMemoryDataHandle.cc \
Reader.cc \
MetaDataReader.cc \
MetaDataReaderIterator.cc \
TextReader.cc \
ODBSelect.cc \
Writer.cc \
DispatchingWriter.cc \
WriterBufferingIterator.cc \
TemplateParameters.cc \
FixedSizeWriterIterator.cc \
ReaderIterator.cc \
TextReaderIterator.cc \
SelectIterator.cc \
MetaData.cc \
HashTable.cc \
Codec.cc \
Column.cc \
Comparator.cc \
ODADatabase.cc \
ODAColumn.cc \
ParameterExpression.cc \
Decoder.cc \
SQLServerSession.cc \
SQLServerOutput.cc \
SQLService.cc \
SQLType.cc \
SQLUser.cc \
SQLIndex.cc \
SQLBit.cc \
SQLReal.cc \
SQLDouble.cc \
SQLInt.cc \
SQLString.cc \
SQLColumn.cc \
SQLBitColumn.cc \
SQLBitfield.cc \
FunctionEQ.cc \
FunctionJOIN.cc \
SQLServer.cc \
SQLSession.cc \
SQLInteractiveSession.cc \
SQLDatabase.cc \
SchemaAnalyzer.cc \
SQLTable.cc \
SQLOutput.cc \
SQLOutputConfig.cc \
SQLODAOutput.cc \
SQLDistinctOutput.cc \
SQLSimpleOutput.cc \
SQLOrderOutput.cc \
FunctionAND.cc \
FunctionRMS.cc \
FunctionOR.cc \
FunctionIN.cc \
FunctionNOT_IN.cc \
FunctionNOT_NULL.cc \
FunctionNULL.cc \
FunctionNVL.cc \
FunctionSTDEV.cc \
FunctionVAR.cc \
SQLCountStar.cc \
ColumnExpression.cc \
FunctionExpression.cc \
FunctionIntegerExpression.cc \
FunctionCOUNT.cc \
FunctionROWNUMBER.cc \
FunctionMIN.cc \
FunctionMAX.cc \
FunctionAVG.cc \
FunctionSUM.cc \
FunctionTHIN.cc \
RegionCache.cc \
EqRegionCache.cc \
RggRegionCache.cc \
FunctionEQ_BOXLAT.cc \
FunctionEQ_BOXLON.cc \
FunctionRGG_BOXLAT.cc \
FunctionRGG_BOXLON.cc \
FunctionTIMESTAMP.cc \
Duration.cc \
DateTime.cc \
FunctionTDIFF.cc \
FunctionJULIAN.cc \
FunctionDOTP.cc \
FunctionNORM.cc \
BitColumnExpression.cc \
VariableExpression.cc \
NumberExpression.cc \
StringExpression.cc \
SQLExpression.cc \
Expressions.cc \
Dictionary.cc \
OrderByExpressions.cc \
SQLSelect.cc \
SelectOneTable.cc \
Environment.cc \
Stack.cc \
SQLSelectFactory.cc \
SQLCreateTable.cc \
SQLStatement.cc \
SQLParser.cc \
GribCodes.cc \
Tracer.cc \
MemoryBlock.cc \
ODBAPISettings.cc \
ODAHandle.cc \
ConstantExpression.cc \
SQLExpressionEvaluated.cc \
odbcapi.cc


SRC.c=
PERSIST=
LINK=


all :: sqll.c sqly.c
all :: sharedlib 


include ../make.mars

clean::
	rm -f sqll.c sqly.c

#INSTALLLIBDIR=../lib
#INSTALLINCDIR=../include

CURRENT=0.9.11

INSTALLLIBDIR=/usr/local/lib/metaps/lib/odalib/$(CURRENT)/lib
INSTALLINCDIR=/usr/local/lib/metaps/lib/odalib/$(CURRENT)/include


install:
	if [ -e $(INSTALLLIBDIR) ]; then echo Directory $(INSTALLLIBDIR) already exists. You have probably forgot to change version number. && exit 1; fi
	mkdir -p $(INSTALLLIBDIR) 
	mkdir -p $(INSTALLINCDIR) || true
	mkdir -p $(INSTALLINCDIR)/../etc/
	cp -f $(SHLIB.TARGET) $(INSTALLLIBDIR)
	cp -f ../lib/libEc.a $(INSTALLLIBDIR)
	cp -f ../lib/libOdb_fortran.a $(INSTALLLIBDIR)
	cp -f ../f2003/odb_c_binding.mod $(INSTALLINCDIR)
	cp -f *.h $(INSTALLINCDIR)
	cp -f *.cc $(INSTALLINCDIR)
	cp -f ../eclib/*.h $(INSTALLINCDIR)
	cp -f ../eclib/*.cc $(INSTALLINCDIR)
	cp -f ../eclib/*.b $(INSTALLINCDIR)
	cp -f ../etc/*.cfg $(INSTALLINCDIR)/../etc/
	chmod -R a+rx $(INSTALLLIBDIR)/../
	chmod -R a+rx $(INSTALLLIBDIR) 
	chmod -R a+rx  $(INSTALLINCDIR)/../
	chmod -R a+rx  $(INSTALLINCDIR)
	chmod -R a+rx  $(INSTALLINCDIR)/../etc/
	find $(INSTALLLIBDIR) $(INSTALLINCDIR) $(INSTALLINCDIR)/../etc/ -type d -exec chmod a+rx {} \;
	find $(INSTALLLIBDIR) $(INSTALLINCDIR) $(INSTALLINCDIR)/../etc/ -type f -exec chmod a+r {} \;

pyodbapi: pyodbapi.swig 
	swig -python -c++ pyodbapi.swig
	$(CCC) -g -fpic -c pyodbapi_wrap.cxx $(INCLUDE) -I. -I$(INSTALLINCDIR) -I$(PYTHON)/include/python$(PYVER)  -I$(PYTHON)/lib/python$(PYVER)/site-packages/
	#$(SHLIB.CMD) pyodbapi_wrap.o -o _pyodbapi.so -L../lib -lOdb -lEc -L$(PYTHON)/lib/python$(PYVER)/config -lpython$(PYVER) 
	$(CCC) -g -shared pyodbapi_wrap.o -o _pyodbapi.so -L$(INSTALLLIBDIR) -lOdb -lEc -L$(PYTHON)/lib/python$(PYVER)/config -lpython$(PYVER) 
