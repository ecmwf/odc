/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

///
/// \file odbcapi.cc
///
/// @author Piotr Kuchta, March 2009

#include "odb_api/odb_api.h"

#include "eckit/runtime/Main.h"

#include "odb_api/FastODA2Request.h"
#include "odb_api/MetaData.h"
#include "odb_api/MetaDataReader.h"
#include "odb_api/MetaDataReaderIterator.h"
#include "odb_api/ODAHandle.h"
#include "odb_api/ODBAPISettings.h"
#include "odb_api/ODBAPIVersion.h"
#include "odb_api/odbcapi.h"
#include "odb_api/Select.h"
#include "odb_api/Reader.h"
#include "odb_api/Writer.h"

using namespace eckit;
using namespace odb;

char *dummyCommandLineArgs[] = { const_cast<char*>("odbcapi"), 0 };

//#include "odbcapi.h"

template <typename T, typename I> 
int get_bitfield(T it,
	int index,
	char** bitfield_names,
	char** bitfield_sizes,
	int* nSize,
	int* sSize)
{
	I* iter = reinterpret_cast<I*>(it);
	const BitfieldDef& bitfieldDef(iter->columns()[index]->bitfieldDef());
	FieldNames fieldNames(bitfieldDef.first);
	Sizes sizes(bitfieldDef.second);

    std::stringstream ns, ss;
	for (size_t i = 0; i < fieldNames.size(); ++i)
	{
		ns << fieldNames[i] << ":";
		ss << sizes[i] << ":";
	}

	std::string names(ns.str());
	std::string ssizes(ss.str());

	//FIXME: the memory allocated by strdup should be freed on Fortran side
	*bitfield_names = strdup(names.c_str());
	*bitfield_sizes = strdup(ssizes.c_str());

	*nSize = names.size();
	*sSize = ssizes.size();
	return 0;
}

extern "C" {

void odb_start()
{
    static const char *argv[2] = {"odb_start", 0};

    odb_start_with_args(1, const_cast<char **>(argv));
}

void odb_start_with_args(int argc, char* argv[])
{
    eckit::Main::initialise(argc, argv);
}

double odb_count(const char * filename)
{
	double n = 0;

	PathName path = filename;
	typedef MetaDataReader<MetaDataReaderIterator> MDR;
	MDR mdReader(path);
	for (MDR::iterator it(mdReader.begin()), end(mdReader.end()); it != end; ++it)
		n += it->columns().rowsNumber();
	return n;
}

int get_blocks_offsets(const char* fileName, size_t* numberOfBlocks,  off_t** offsets, size_t** sizes)
{
	FastODA2Request<ODA2RequestClientTraits> o;
	o.mergeSimilarBlocks(false);

	OffsetList offs;
	LengthList lengths;
	std::vector<ODAHandle*> handles;

	o.scanFile(fileName, offs, lengths, handles);

	ASSERT(offs.size() == lengths.size());
	ASSERT(offs.size() == handles.size());

	size_t n = offs.size();

	*numberOfBlocks = n;
	*offsets = new off_t[n];
	*sizes = new size_t[n];
	
	for (size_t i = 0; i < n; ++i)
	{
		(*offsets)[i] = offs[i];
		(*sizes)[i] = lengths[i];
		delete handles[i];
	}

	return 0;
}

int release_blocks_offsets(off_t** offsets) { delete [] *offsets; *offsets = 0; return 0; }
int release_blocks_sizes(size_t** sizes) { delete [] *sizes; *sizes = 0; return 0; }

unsigned int odb_get_headerBufferSize() { return ODBAPISettings::instance().headerBufferSize(); } 
void odb_set_headerBufferSize(unsigned int n) { ODBAPISettings::instance().headerBufferSize(n); }

unsigned int odb_get_setvbufferSize() { return ODBAPISettings::instance().setvbufferSize(); } 
void odb_set_setvbufferSize(unsigned int n) { ODBAPISettings::instance().setvbufferSize(n); }

const char* odb_api_version() { return odb::ODBAPIVersion::version(); }
const char* odb_api_git_sha1() { return odb::ODBAPIVersion::gitsha1(); }

unsigned int odb_api_format_version_major() { return odb::ODBAPIVersion::formatVersionMajor(); }
unsigned int odb_api_format_version_minor() { return odb::ODBAPIVersion::formatVersionMinor(); }

/// @param config  ignored for now.
oda_ptr odb_read_create(const char *config, int *err)
{
	Reader* o = new Reader;
	*err = !o;
	return oda_ptr(o);
}

oda_ptr odb_create(const char *config, int *err)
{
    return odb_read_create(config, err);
}

/// @param config  ignored for now.
oda_ptr odb_select_create(const char *config, int *err)
{
	Select* o = new Select;
	*err = !o;
	return oda_ptr(o);
}

/// @param config  ignored for now.
oda_writer_ptr odb_writer_create(const char *config, int *err)
{
	//PathName path = filename;
	Writer<>* o = new Writer<>; //(path);
	*err = !o;
	return oda_writer_ptr(o);
}

int odb_read_destroy(oda_ptr o)
{
	delete reinterpret_cast<Reader *>(o);
	return 0;
}

int odb_destroy(oda_ptr o)
{
    return odb_read_destroy(o);
}

int odb_select_destroy(oda_ptr o)
{
	delete reinterpret_cast<Select *>(o);
	return 0;
}

int odb_writer_destroy(oda_writer_ptr o)
{
	delete reinterpret_cast<Writer<> *>(o);
	return 0;
}

oda_read_iterator_ptr odb_create_read_iterator(oda_ptr co, const char *filename, int *err)
{
	Reader *o (reinterpret_cast<Reader*>(co));
	std::string fileName (filename);
	PathName fn (fileName);
	if (! fn.exists())
	{
		*err = 2; //TODO: define error codes
		return 0;
	}
	
	ReaderIterator* iter (o->createReadIterator(fn));
	*err = !iter;
	return oda_read_iterator_ptr(iter);
	
}

oda_select_iterator_ptr odb_create_select_iterator(oda_ptr co, const char *sql, int *err)
{
    Select *o (reinterpret_cast<Select*>(co));
    try { 
        SelectIterator* iter (o->createSelectIterator(sql));
        *err = !iter;
        return oda_select_iterator_ptr(iter);
    }
    catch (eckit::CantOpenFile e)
    {
		*err = 1;
		return 0;
    }
    catch (eckit::ReadError e)
    {
		*err = 2; 
		return 0;
    }
}

oda_select_iterator_ptr odb_create_select_iterator_from_file(oda_ptr co, const char *sql, const char *filename, int *err)
{
    Select *o (reinterpret_cast<Select*>(co));

    std::string full_sql (std::string(sql) + " from \"" + std::string(filename) + "\"");

    SelectIterator* iter (o->createSelectIterator(full_sql));
    *err = !iter;
    return oda_select_iterator_ptr(iter);
}


int odb_read_iterator_destroy(oda_read_iterator_ptr it)
{
	delete reinterpret_cast<ReaderIterator*>(it);
	return 0;
}

int odb_select_iterator_destroy(oda_select_iterator_ptr it)
{
	delete reinterpret_cast<SelectIterator*>(it);
	return 0;
}

int odb_read_iterator_get_no_of_columns(oda_read_iterator_ptr it, int *numberOfColumns)
{
	ReaderIterator* iter (reinterpret_cast<ReaderIterator*>(it));
	*numberOfColumns = iter->columns().size();
	return 0;
}

int odb_read_iterator_get_column_size(oda_read_iterator_ptr it, int n, int* sizes)
{
    ReaderIterator* iter (reinterpret_cast<ReaderIterator*>(it));
    NOTIMP;
//	*numberOfColumns = iter->columns().size();
    return 0;
}

int odb_read_iterator_get_column_offset(oda_read_iterator_ptr it, int n, int* offsets)
{
    ReaderIterator* iter (reinterpret_cast<ReaderIterator*>(it));
    NOTIMP;
//	*numberOfColumns = iter->columns().size();
    return 0;
}

int odb_select_iterator_get_no_of_columns(oda_select_iterator_ptr it, int *numberOfColumns)
{
	SelectIterator* iter (reinterpret_cast<SelectIterator*>(it));
	*numberOfColumns = iter->columns().size();
	return 0;
}

int odb_read_iterator_get_column_type(oda_read_iterator_ptr it, int n, int *type)
{
	ReaderIterator* iter (reinterpret_cast<ReaderIterator*>(it));
	*type = iter->columns()[n]->type();
	return 0;
}

int odb_select_iterator_get_column_type(oda_select_iterator_ptr it, int n, int *type)
{
	SelectIterator* iter (reinterpret_cast<SelectIterator*>(it));
	*type = iter->columns()[n]->type();
	return 0;
}

int odb_read_iterator_get_column_name(oda_read_iterator_ptr it, int n, char **name, int *size_name)
{
	ReaderIterator* iter (reinterpret_cast<ReaderIterator*>(it));
	*name = const_cast<char*>(iter->columns()[n]->name().c_str());
	*size_name = iter->columns()[n]->name().length();
    return 0;
}

int odb_select_iterator_get_column_name(oda_select_iterator_ptr it, int n, char **name, int *size_name)
{
	SelectIterator* iter (reinterpret_cast<SelectIterator*>(it));
	*name = const_cast<char*>(iter->columns()[n]->name().c_str());
	*size_name = iter->columns()[n]->name().length();
     return 0;
}

int odb_read_iterator_get_next_row(oda_read_iterator_ptr it, int count, double* data, int *new_dataset)
{
	ReaderIterator* iter (reinterpret_cast<ReaderIterator*>(it));
    if (! iter->next())
		return 1;

	if (iter->isNewDataset())
		*new_dataset = 1;
	else
		*new_dataset = 0;

    ASSERT(count >= 0);

	if (count != static_cast<int>(iter->columns().size()))
		return 2; // TDOO: define error codes

    ::memcpy(data, iter->data(), count * sizeof(data[0]));

	return 0;
}

int odb_select_iterator_get_next_row(oda_select_iterator_ptr it, int count, double* data, int *new_dataset)
{
	SelectIterator* iter (reinterpret_cast<SelectIterator*>(it));
    if (! iter->next())
		return 1;

	if (iter->isNewDataset())
		*new_dataset = 1;
	else
		*new_dataset = 0;

	if (count != static_cast<int>(iter->columns().size()))
		return 2; // TDOO: define error codes

	for (int i (0); i < count; ++i)
		data[i] = iter->data()[i];

	return 0;
}

oda_write_iterator_ptr odb_create_append_iterator(oda_ptr co, const char *filename, int *err)
{
	Writer<> *o (reinterpret_cast<Writer<> *>(co));
    eckit::Length estimatedLength(0);
	DataHandle *fh = ODBAPISettings::instance().appendToFile(PathName(std::string(filename)), estimatedLength, true);

	// TODO: make sure there's no leaks (FileHandle)
	Writer<>::iterator_class* w (new Writer<>::iterator_class(*o, fh, false));
	*err = !w;
	return oda_write_iterator_ptr(w);
}

oda_write_iterator_ptr odb_create_write_iterator(oda_ptr co, const char *filename, int *err)
{
	Writer<> *o (reinterpret_cast<Writer<> *>(co));
    eckit::Length estimatedLength(0);
	DataHandle *fh = ODBAPISettings::instance().writeToFile(PathName(std::string(filename)), estimatedLength, true);

	// TODO: make sure there's no leaks (FileHandle)
	Writer<>::iterator_class* w (new Writer<>::iterator_class(*o, fh, true));
	*err = !w;
	return oda_write_iterator_ptr(w);
}

int odb_write_iterator_destroy(oda_write_iterator_ptr wi)
{
	delete reinterpret_cast<Writer<>::iterator_class *>(wi);
	return 0;
}

int odb_write_iterator_set_no_of_columns(oda_write_iterator_ptr wi, int n)
{
	Writer<>::iterator_class *w (reinterpret_cast<Writer<>::iterator_class *>(wi));
	w->setNumberOfColumns(n);
	return 0;
}

int odb_write_iterator_set_column(oda_write_iterator_ptr wi, int index, int type, const char *name)
{
	Writer<>::iterator_class * w (reinterpret_cast<Writer<>::iterator_class *>(wi));
	return w->setColumn(index, std::string(name), ColumnType(type));
}

int odb_write_iterator_set_bitfield(oda_write_iterator_ptr wi, int index, int type, const char *name, const char* bitfieldNames, const char *bitfieldSizes)
{
	std::string bnames (bitfieldNames);
    std::string bsizes (bitfieldSizes);
    odb::FieldNames    (bitfield_names);
    odb::Sizes         (bitfield_sizes);
 
//	std::cout << " columnName = " << name << " " << bnames << " " << bsizes << std::endl;
	size_t iprev (0);
	for (size_t i (0); i < bnames.size(); i++)
    {
		if (bnames[i] == ':')
        {
			std::string name (bnames.substr(iprev,i-iprev));
			iprev = i+1;    
			bitfield_names.push_back(name);
		}
	}

    iprev = 0;
	for (size_t i (0); i < bsizes.size(); i++)
    {
		if (bsizes[i] == ':')
        {
			std::string name (bsizes.substr(iprev, i-iprev));
			size_t size (atof(name.c_str())); // bit[0-9]+
			iprev = i+1;    
			bitfield_sizes.push_back(size);
		}
	}

    odb::BitfieldDef bitfieldType(make_pair(bitfield_names, bitfield_sizes));

	Writer<>::iterator_class * w (reinterpret_cast<Writer<>::iterator_class *>(wi));
	std::string columnName(name);
	
	int rc (w->setBitfieldColumn(index, columnName, ColumnType(type), bitfieldType));
	return rc;
}

int odb_read_iterator_get_missing_value(oda_read_iterator_ptr ri, int index, double* value)
{
    ReaderIterator* r (reinterpret_cast<ReaderIterator*>(ri));
    if (index < 0 && r->columns().size() < index) 
    {
        stringstream ss;
        ss << "odb_read_iterator_get_missing_value: index " << index 
            << " out of range, should be between 0 and " << r->columns().size();
        throw UserError(ss.str());
    }
    *value = r->columns()[index]->missingValue();
    return 0;
}

int odb_write_iterator_set_missing_value(oda_write_iterator_ptr wi, int index, double value)
{
	Writer<>::iterator_class * w (reinterpret_cast<Writer<>::iterator_class *>(wi));
	w->missingValue(index, value);
	return 0;
}

int odb_write_iterator_write_header(oda_write_iterator_ptr wi)
{
	Writer<>::iterator_class * w (reinterpret_cast<Writer<>::iterator_class *>(wi));
	w->writeHeader();
	return 0;
}

int odb_write_iterator_set_next_row(oda_write_iterator_ptr wi, double *data, int count)
{ 
	Writer<>::iterator_class * w (reinterpret_cast<Writer<>::iterator_class *>(wi));
    return w->writeRow(data, count);
}

int odb_read_iterator_get_bitfield(oda_read_iterator_ptr it,
	int index,
	char** bitfield_names,
	char** bitfield_sizes,
	int* nSize,
	int* sSize)
{ return get_bitfield<oda_read_iterator_ptr,ReaderIterator>(it, index, bitfield_names, bitfield_sizes, nSize, sSize); }

int odb_select_iterator_get_bitfield(oda_select_iterator_ptr it,
	int index,
	char** bitfield_names,
	char** bitfield_sizes,
	int* nSize,
	int* sSize)
{ return get_bitfield<oda_select_iterator_ptr,SelectIterator>(it, index, bitfield_names, bitfield_sizes, nSize, sSize); }

} // extern "C" 

