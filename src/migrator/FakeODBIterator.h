/*
 * © Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// \file FakeODBIterator.h
/// @author Piotr Kuchta, ECMWF, March 2009

#ifndef FakeODBIterator_H
#define FakeODBIterator_H
namespace eclib { class PathName; }

namespace odb {
namespace tool {

class ODBIterator;

typedef pair<std::string, std::string> Assignment;
typedef vector<Assignment> AssignmentsBase;

struct Assignments : public AssignmentsBase {
	Assignments(const std::string&);
};

struct ConstParameter {
	ConstParameter(std::string name, double value, odb::ColumnType type)
	: name(name), value(value), type(type)
	{}

	std::string name;
	double      value;
	odb::ColumnType  type;
};

class FakeODBIterator : public odb::RowsReaderIterator
{
public:
	struct ConstParameters : public vector<ConstParameter>
	{
		// Not thread safe.
		static ConstParameters& instance() { return instance_; }

		void addInteger(const std::string& name, long);
		void addReal(const std::string& name, double);
		void addString(const std::string& name, std::string value);

		void add(const Assignments&);
	private:
		static ConstParameters instance_;
	};


	FakeODBIterator(const eclib::PathName& db, const std::string& sql); 
	~FakeODBIterator ();

	const FakeODBIterator& end() { return *reinterpret_cast<FakeODBIterator*>(0); }
	//bool operator!=(const FakeODBIterator& o) { ASSERT(&o == 0); return iterator_ != iterator_.end(); }
	//FakeODBIterator& operator++() { next(); return *this; }

	odb::MetaData& columns();

	virtual bool isNewDataset();
	virtual double* data();

//protected:
	virtual bool next();

	int refCount_;
	bool noMore_;

private:
	ODBIterator iterator_;

	odb::MetaData columns_;
	double* data_;

	ConstParameters& constParameters_;
};

} // namespace tool 
} //namespace odb 

#endif
