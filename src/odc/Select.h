/*
 * (C) Copyright 1996-2012 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @author Piotr Kuchta
/// @author
/// @date April 2010

#ifndef odc_Select_H
#define odc_Select_H

#ifdef SWIGPYTHON
#include <Python.h>
#endif

#include "eckit/sql/SQLSession.h"
#include "odc/IteratorProxy.h"
#include "odc/SelectIterator.h"

namespace eckit {
class PathName;
}
namespace eckit {
class DataHandle;
}

namespace odc {

//----------------------------------------------------------------------------------------------------------------------

class Select {
public:

    typedef IteratorProxy<SelectIterator, Select, const double> iterator;
    typedef iterator::Row row;

    Select(const std::string& selectStatement = "", bool manageOwnBuffer = true);
    Select(const std::string& selectStatement, eckit::DataHandle& dh, bool manageOwnBuffer = true);
    Select(const std::string& selectStatement, const eckit::PathName& path, bool manageOwnBuffer = true);

    // This only exists to disambiguate const char* --> std::string rather than to bool.
    Select(const std::string& selectStatement, const char* path, bool manageOwnBuffer = true);

    ~Select() noexcept(false);

#ifdef SWIGPYTHON
    iterator __iter__() { return iterator(createSelectIterator(selectStatement_)); }
#endif

    eckit::sql::SQLDatabase& database();

    iterator begin();
    const iterator end();

    SelectIterator* createSelectIterator(const std::string&);

private:


    friend class odc::IteratorProxy<odc::SelectIterator, odc::Select, const double>;

    std::unique_ptr<eckit::DataHandle> ownDH_;

    //	std::istream* istream_;

    std::string selectStatement_;
    std::string delimiter_;

    eckit::sql::SQLSession session_;

    // This is horrible, but the TextReader, and any stream based iteraton, can only
    // iterate once, so we MUST NOT create two iterators if begin() is called twice.
    bool initted_;
    iterator it_;
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace odc

#endif
