
#include "SQLIteratorOutput.h"

namespace odb {
namespace sql {

template <typename T>
SQLIteratorSession<T>::SQLIteratorSession(T &it)
: statement_(0),
  iterator_(it)
{}

template <typename T>
SQLIteratorSession<T>::~SQLIteratorSession() {}

template <typename T>
SQLOutput* SQLIteratorSession<T>::defaultOutput()
{
	return new SQLIteratorOutput<T>(iterator_);
}

template <typename T>
void SQLIteratorSession<T>::statement(odb::sql::SQLStatement *sql)
{
	ASSERT(sql);	
	statement_ = sql;
	//execute(*sql);
	//delete sql;
}

template <typename T>
SQLStatement* SQLIteratorSession<T>::statement() { return statement_; }

} // namespace sql 
} // namespace odb 
