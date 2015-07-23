/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// File ResultSetStore.h
// Piotr Kuchta - ECMWF June 2015

#ifndef odb_sql_ResultSetStore_H
#define odb_sql_ResultSetStore_H

#include <map>
#include <ostream>

#include "odb_api/ecml_data/ResultSet.h"

namespace odb {
namespace sql {

//TODO: ExecutionContext will have an instance of this class, its static functions will become member ones

class ResultSetStore {
public:
    ResultSetStore();
    ~ResultSetStore(); 

    static void put(const std::string&, ResultSet&);
    static ResultSet& get(const std::string&);
    static void remove(const std::string&);

private:
// No copy allowed
    ResultSetStore(const ResultSetStore&);
    ResultSetStore& operator=(const ResultSetStore&);

    //void print(std::ostream&) const;

    static std::map<std::string, ResultSet*> resultSets_;

// -- Friends
    //friend std::ostream& operator<<(std::ostream& s, const ResultSetStore& p) { p.print(s); return s; }
};

} // namespace sql
} // namespace odb

#endif
