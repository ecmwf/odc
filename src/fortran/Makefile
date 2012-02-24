DEPLIB= ../lib/libOdb.a 
INCLUDE=-I../liboda
BINLIB=-L../lib -lOdb -lEc
LIBLIB=-L../lib -lOdb -lEc

# ------------------- Python
#---------------------------------------------------------------------------------------------------------------------
PYVER					=	2.5
PYVER_MINOR				=	2
PYTHON					=	/usr/local/apps/python/$(PYVER)
#---------------------------------------------------------------------------------------------------------------------

SHLIB.TARGET=../lib/libOdb_fortran.a 

SRC.f90=odb_c_binding.f90


SRC.c=
PERSIST=


all :: sharedlib 


include ../make.mars
clean::
	rm -f odb_c_binding.o odb_c_binding.mod

#INSTALLLIBDIR=/usr/local/lib/metaps/lib/odalib/current/lib
#INSTALLINCDIR=/usr/local/lib/metaps/lib/odalib/current/include

INSTALLLIBDIR=../lib
INSTALLINCDIR=../include

install:
	if [ -e $(INSTALLLIBDIR) ]; then echo Directory $(INSTALLLIBDIR) already exists. You have probably forgot to change version number. && exit 1; fi
	mkdir -p $(INSTALLLIBDIR) 
	cp -f $(SHLIB.TARGET) $(INSTALLLIBDIR)
	cp -f ../lib/libEc.a $(INSTALLLIBDIR)
	cp -f ../lib/libOdb.a $(INSTALLLIBDIR)
	mkdir -p $(INSTALLINCDIR) || true
	cp -f *.h $(INSTALLINCDIR)
	cp -f *.f90 $(INSTALLINCDIR)
	chmod -R a+rx $(INSTALLLIBDIR)/../
	chmod -R a+rx $(INSTALLLIBDIR) 
	chmod -R a+rx  $(INSTALLINCDIR)/../
	chmod -R a+rx  $(INSTALLINCDIR)
	find $(INSTALLLIBDIR) $(INSTALLINCDIR) -type d -exec chmod a+rx {} \;
	find $(INSTALLLIBDIR) $(INSTALLINCDIR) -type f -exec chmod a+r {} \;

# LIBRARIES=$(SHLIB.TARGET)
test: test_fortran_api

test_fortran_api: test_fortran_api.o  $(DEPLIB) $(SHLIB.TARGET)
	rm -f $@
	$(FORTRAN_LINKER) $(FFLAGS)  $(DEBUG) $(OPTIMIZE) $(INCLUDE) -o $@ $@.o  $(APPLIB) $(BINLIB) $(TAPELIB) $(APPLIB) $(SHLIB.TARGET) $(FORTRAN)
	-$(DEPEND.CMD)
	ls -l $@
	
	
