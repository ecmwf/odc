/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "odb_api/ecml_data/ResultSetStore.h"

using namespace std;

namespace odb {
namespace sql {

std::map<std::string, ResultSet*> ResultSetStore::resultSets_ = std::map<std::string, ResultSet*>();

ResultSetStore::ResultSetStore() {}
ResultSetStore::~ResultSetStore() {}

ResultSet& ResultSetStore::get(const std::string& id)
{
    return *resultSets_[id];
}

void ResultSetStore::put(const std::string& id, ResultSet& rs)
{
    resultSets_[id] = &rs;
}

void ResultSetStore::remove(const std::string& id)
{
    resultSets_.erase(id);
}

//void ResultSetStore::print(std::ostream& s) const { }

} // namespace sql
} // namespace odb
