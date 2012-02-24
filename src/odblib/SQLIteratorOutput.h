/// \file SQLIteratorOutput.h
/// Piotr Kuchta - ECMWF Feb 09

#ifndef SQLIteratorOutput_H
#define SQLIteratorOutput_H

#include "SQLOutput.h"

namespace odb {

class SelectIterator;

namespace sql {

class ReaderIterator;

template <typename T = odb::SelectIterator>
class SQLIteratorOutput : public SQLOutput {
public:
	SQLIteratorOutput(T &);
	virtual ~SQLIteratorOutput(); // Change to virtual if base class

protected:

	virtual void print(ostream&) const; // Change to virtual if base class	

private:
// No copy allowed
	SQLIteratorOutput(const SQLIteratorOutput&);
	SQLIteratorOutput& operator=(const SQLIteratorOutput&);

// -- Members
	T& iterator_;
	//bool headerSaved = true;
	unsigned long long count_;

// -- Methods
	// None

// -- Overridden methods

	virtual void size(int);
	virtual void reset();
	virtual void flush();
	virtual bool output(const expression::Expressions&);
	virtual void prepare(SQLSelect&);
	virtual void cleanup(SQLSelect&);
	virtual unsigned long long count();

	virtual void outputReal(double, bool) const { NOTIMP; };
	virtual void outputDouble(double, bool) const { NOTIMP; };
	virtual void outputInt(double, bool) const { NOTIMP; };
	virtual void outputUnsignedInt(double, bool) const { NOTIMP; };
	virtual void outputString(double, bool) const { NOTIMP; };
	virtual void outputBitfield(double, bool) const { NOTIMP; };
};

} // namespace sql
} // namespace odb

#include "SQLIteratorOutput.cc"

#endif
