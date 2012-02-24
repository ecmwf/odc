// File SQLIteratorSession.h
// Piotr Kuchta - ECMWF Feb 09

#ifndef SQLIteratorSession_H
#define SQLIteratorSession_H

#include "SQLSession.h"

namespace odb {

class ReaderIterator;
class SelectIterator;
class SQLOutput;
class SQLStatement;

namespace sql {

template <typename T = SelectIterator>
class SQLIteratorSession : public SQLSession {
public:
	SQLIteratorSession(T &);
	~SQLIteratorSession(); // Change to virtual if base class

	SQLStatement* statement();

private:
// No copy allowed
	SQLIteratorSession(const SQLIteratorSession&);
	SQLIteratorSession& operator=(const SQLIteratorSession&);

	SQLStatement* statement_;
	T& iterator_;

// -- Overridden methods

	void       statement(SQLStatement*);
	SQLOutput* defaultOutput();

// -- Friends

	//friend ostream& operator<<(ostream& s,const SQLIteratorSession& p)
	//	{ p.print(s); return s; }

};

} // namespace sql
} // namespace odb

#include "SQLIteratorSession.cc"

#endif
