/// \file FakeODBIterator.h
/// @author Piotr Kuchta, ECMWF, March 2009

#ifndef FakeODBIterator_H
#define FakeODBIterator_H
class PathName;

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


	FakeODBIterator(const PathName& db, const std::string& sql); 
	~FakeODBIterator ();

	const FakeODBIterator& end() { return *reinterpret_cast<FakeODBIterator*>(0); };
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
