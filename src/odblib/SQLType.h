// File SQLType.h
// Baudouin Raoult - ECMWF Dec 03

#ifndef SQLType_H
#define SQLType_H

#include <machine.h>

namespace odb {
namespace sql {

class SQLOutput;

namespace type {

class SQLType {
public:

	enum {
		realType    = 0,
		integerType = 1,
		stringType  = 2,
		bitmapType  = 3,
		blobType    = 4,
		doubleType  = 5
	};

	SQLType(const string&);

	/// Constructor used when defining a bitfield.
	SQLType(const string&, const string&);

	virtual ~SQLType(); // Change to virtual if base class

	const string& name() const { return name_; }

	virtual size_t size() const = 0;
	virtual void output(SQLOutput&, double, bool) const = 0;
	virtual const SQLType* subType(const string&) const;

	virtual int getKind() const = 0;

	static const SQLType& lookup(const string&);

	static void createAlias(const string&, const string&);

protected:
	virtual void print(ostream&) const; // Change to virtual if base class	
	static bool exists(const string&);

private:
// No copy allowed
	SQLType(const SQLType&);
	SQLType& operator=(const SQLType&);

	string name_;

	friend ostream& operator<<(ostream& s,const SQLType& p)
		{ p.print(s); return s; }
};

class DynamicallyCreatedTypesDestroyer {
public:
	static SQLType* registerType(SQLType*);
	~DynamicallyCreatedTypesDestroyer ();
};

} // namespace type 
} // namespace sql
} // namespace odb

#endif
