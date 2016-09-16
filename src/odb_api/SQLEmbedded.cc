/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "ecml/core/ExecutionContext.h"
#include "eckit/types/Types.h"

#include "odb_api/SQLEmbedded.h"

namespace odb {
namespace sql {

SQLEmbedded::SQLEmbedded(const EmbeddedAST& a) 
: code_(a.code_)
{}

SQLEmbedded::~SQLEmbedded() {}

void SQLEmbedded::print(std::ostream& s) const {}

unsigned long long SQLEmbedded::execute(ecml::ExecutionContext* context)
{
    NOTIMP;
    return 0; // TODO:
}

expression::Expressions SQLEmbedded::output() const
{
    NOTIMP;
}

} // namespace sql 
} // namespace odb 


