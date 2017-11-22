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
/// \file IteratorProxy.h
///
/// @author Piotr Kuchta, Feb 2009

#ifndef IteratorProxy_H
#define IteratorProxy_H

#include "odb_api/ColumnType.h"
#include "odb_api/SelectIterator.h"
#include "odb_api/Types.h"

namespace eckit { class PathName; }
namespace eckit { class DataHandle; }

#ifdef SWIGPYTHON

#include <Python.h>

#include "odb_api/Column.h"
#include "odb_api/odbcapi.h"

struct ODBStopIteration : public std::exception {
	const char* what() const throw() { return "end of data"; }
};

struct ODBIndexError : public std::exception {
	const char* what() const throw() { return "index out of range"; }
};

extern "C" void python_api_start()
{
    odb_start();
}

#endif

namespace odb {

class Reader;
class ReaderIterator;
class MetaData;
class MetaDataReaderIterator;
class SelectIterator;


template <typename ITERATOR, typename O, typename DATA>
class IteratorProxy;

template <typename ITERATOR, typename O, typename DATA, typename ITERATOR_PROXY>
class Row_
{
public:
#ifdef SWIGPYTHON
	Row_() : it_() {}
#endif
	Row_(ITERATOR_PROXY& it) : it_(&it) {}

	DATA& operator[](size_t i) { return (*it_)->data()[i]; }

    DATA* data() { return ((*it_).iter_)->data(); }
	DATA& data(size_t i) { return ((*it_).iter_)->data(i); }

    const DATA* data() const { return ((*it_).iter_)->data(); }
    const DATA& data(size_t i) const { return ((*it_).iter_)->data(i); }

    int integer(size_t i) { return int((*it_)->data()[i]); }
	std::string string(int i)
	{
		const char *s = reinterpret_cast<const char *>(&data()[i]);
		size_t j = 0;
		for (; j < sizeof(double) && s[j]; ++j)
			; 
		return std::string(s, j);
	}

    const MetaData& columns() const { return ((*it_).iter_)->columns(); }
    void setNumberOfColumns(size_t n) { ((*it_).iter_)->setNumberOfColumns(n); }
	const MetaData& columns(const MetaData& md) { return ((*it_).iter_)->columns(md); }
	bool isNewDataset() { return ((*it_).iter_)->isNewDataset(); }
	bool isMissing(size_t i) { return ((*it_).iter_)->columns()[i]->missingValue() == (*it_)->data()[i]; }
	double missingValue(size_t i) { return ((*it_).iter_)->columns()[i]->missingValue(); }

	int setColumn(size_t index, const std::string& name, ColumnType type)
	{ return (*((*it_).iter_)).setColumn(index, name, type); }

	int setBitfieldColumn(size_t index, const std::string& name, ColumnType type, BitfieldDef b)
	{ return ((*it_).iter_)->setBitfieldColumn(index, name, type, b); }

	void missingValue(size_t index, double v) { ((*it_).iter_)->missingValue(index, v); }

	void writeHeader() { (*((*it_).iter_)).writeHeader(); }
	void close() { ((*it_).iter_)->close(); }

    const std::map<std::string, std::string>& properties() const { return it_->iter_->properties(); }

	template <typename T> unsigned long pass1(T b, const T e) { return ((*it_).iter_)->pass1(b, e); }

	ITERATOR& operator*() { return *((*it_).iter_); }
private:
	ITERATOR_PROXY* it_;
};

template <typename ITERATOR, typename O = Reader, typename DATA = double>
class IteratorProxy
{
public:
	typedef Row_<ITERATOR,O,DATA,IteratorProxy> Row;

#ifdef SWIGPYTHON
	IteratorProxy() : iter_(), row_(*this) {}
#endif

	IteratorProxy(ITERATOR* iter) : iter_(iter), row_(*this)
	{ if(iter_) ++iter_->refCount_; }

    IteratorProxy(const IteratorProxy& other) : iter_(other.iter_), row_(*this)
	{ if (iter_) ++iter_->refCount_; }

	~IteratorProxy()
	{
		if(iter_ && (--iter_->refCount_ == 0))
			delete iter_;
	}

    IteratorProxy& operator=(const IteratorProxy& other)
	{
		if (iter_ == other.iter_)
			return *this;

		if (iter_ && (--iter_->refCount_ == 0))
			delete iter_;

		iter_ = other.iter_;
		++iter_->refCount_;
		return *this;
	}

	Row* operator->() { return &row_; }
    const Row* operator->() const { return &row_; }

    Row& operator*() { return row_; }
    const Row& operator*() const { return row_; }

    bool operator!=(const IteratorProxy&) 
    { 
        return iter_ != 0 && !iter_->noMore_; 
    }

    IteratorProxy& operator++()
	{
        iter_->next();
		return *this;
	}

#ifdef SWIGPYTHON
	size_t __len__() { return iter_->columns().size(); } 

	PyObject* getitem(const char* s)
	{
        std::string name(s);
		if (iter_->columns().hasColumn(name))
			return getitem(iter_->columns().columnIndex(name));
		else
			throw ODBIndexError();
	}

	PyObject* getitem(int i)
	{
		Column& column = *iter_->columns()[i];
		double d = iter_->data()[i];
		if (d == column.missingValue())
			Py_RETURN_NONE;

		switch (column.type()) {
		case STRING:
		{
			const char *s = reinterpret_cast<const char *>(&d);
			size_t j = 0;
			for (; j < sizeof(double) && s[j]; ++j)
				; 
			return PyString_FromStringAndSize(s, j);
		}
		case INTEGER: return PyLong_FromDouble(d);
		case BITFIELD:
		{
			//cerr << "BITFIELD" << std::endl;
			typedef unsigned long B;
			char buf[sizeof(B) + 1];
			char *s = buf;
		
			B n = d;
			B mask = 1 << (sizeof(B) - 1);
			for(size_t j = 0; j < sizeof(B); ++j)
			{
				*s++ = (n & mask) ? '1' : '0';
				mask >>= 1;
			}
			buf[sizeof(B)] = 0;
			return PyString_FromStringAndSize(buf, sizeof(B) + 1);
		}
		default: return PyFloat_FromDouble(d);
		}
	}

	PyObject* __getitem__(PyObject* i)
	{
		//cerr << "__getitem__: start: " << PyString_AsString(PyObject_Repr(i)) << std::endl;
		if (PyTuple_Check(i))
		{
			Py_ssize_t n = PyTuple_Size(i);
			PyObject* l = PyTuple_New(n);
			for(int j (0); j < n; ++j)
			{
				PyObject* o = PyTuple_GetItem(i, j);
				PyTuple_SetItem(l, j, __getitem__(o));
			}
			return l;
		}
		if (PyList_Check(i))
		{
			Py_ssize_t n (PyList_Size(i));
			PyObject* l (PyTuple_New(n));
			for(ssize_t j (0); j < n; ++j)
			{
				PyObject* o (PyList_GetItem(i, j));
				PyTuple_SetItem(l, j, __getitem__(o));
			}
			return l;
		}
		if (PyString_Check(i))
		{
			//cerr << "__getitem__: start: PyString " << PyString_AsString(PyObject_Repr(i)) << std::endl;
			return getitem(PyString_AsString(i));
		}
		if (PySlice_Check(i))
		{
			//cerr << "__getitem__: we've got a PySliceObject here: ";
			return getslice((PySliceObject*) i);
		}

		long li = PyLong_AsLong(i);
		return getitem(li);
	}

	PyObject* getslice(PySliceObject* slice)
	{
		//cerr << "__getslice__(PySliceObject*):" << std::endl;
		Py_ssize_t start = 0, stop = 0, step = 0, slicelength = 0;
		PySlice_GetIndicesEx(slice, __len__(), &start, &stop, &step, &slicelength);
		
		return getslice(start, stop, step, slicelength);
	}

	PyObject* getslice(Py_ssize_t start, Py_ssize_t stop, Py_ssize_t step, Py_ssize_t slicelength)
	{
		ssize_t maxIndex = __len__();
		if (start < 0 || start > maxIndex) throw ODBIndexError();
		if (stop < 0 || stop > maxIndex) throw ODBIndexError();

		//cerr << "__getslice__: start=" << start << ", stop=" << stop << ", step=" << step << ", slicelength=" << slicelength << std::endl; 

		size_t outputSize = 0;
		for (int index = start; (step > 0) ? (index < stop) : (index > stop); index += step)
			++outputSize;
		PyObject* l = PyList_New(outputSize);

		size_t outIndex = 0;
		for (int index = start; (step > 0) ? (index < stop) : (index > stop); index += step)
		{
			ASSERT(outIndex < outputSize);
			PyList_SetItem(l, outIndex++, getitem(index));
		}
		return l;
	}

	IteratorProxy  __iter__() { return *this; }
	IteratorProxy  __next__() { return next(); }
	IteratorProxy next()
	{
        if (! iter_->next())
			throw ODBStopIteration();
		return *this;
	}
#endif

//private:
	ITERATOR *iter_;
	Row row_;

	friend class ReaderIterator;
	friend class MetaDataReaderIterator;
    friend std::ostream& operator<<(std::ostream &o, const IteratorProxy& it) {
		for (size_t i = 0; i < it.iter_->columns().size(); ++i)
			o << it.iter_->data()[i] << "\t";
		return o;
	}
};

} // namespace odb 

#endif
