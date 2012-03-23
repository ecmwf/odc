/*
 * © Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#include "odblib/SQLIteratorOutput.h"

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
