/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <climits>
#include <cfloat>

#include "odblib/oda.h"

#include "odblib/SQLIteratorSession.h"
#include "odblib/SQLSelect.h"
#include "odblib/SQLType.h"
#include "odblib/SQLIteratorOutput.h"

namespace odb {
namespace sql {

template <typename T>
SQLIteratorOutput<T>::SQLIteratorOutput(T& it):
        iterator_(it),
        count_(0)
{}

template <typename T>
SQLIteratorOutput<T>::~SQLIteratorOutput() {}

template <typename T>
void SQLIteratorOutput<T>::print(ostream& s) const
{
        s << "SQLIteratorOutput";
}

template <typename T>
void SQLIteratorOutput<T>::size(int) {}

template <typename T>
void SQLIteratorOutput<T>::reset() { count_ = 0; }

template <typename T>
void SQLIteratorOutput<T>::flush() {}

template <typename T>
bool SQLIteratorOutput<T>::output(const expression::Expressions& results)
{
        size_t nCols = results.size();
        ///ASSERT(nCols == iterator_.columns().size());
        bool missing = false;

        if (iterator_.isCachingRows())
                iterator_.cacheRow(results);
        else
                for(size_t i = 0; i < nCols; i++)
                        iterator_.data_[i] = results[i]->eval(missing /*=false*/);
        count_++;
        return true;
}

template <typename T>
void SQLIteratorOutput<T>::prepare(SQLSelect& sql) {}

template <typename T>
void SQLIteratorOutput<T>::cleanup(SQLSelect& sql) {}

template <typename T>
unsigned long long SQLIteratorOutput<T>::count() { return count_; }

} // namespace sql
} // namespace odb
