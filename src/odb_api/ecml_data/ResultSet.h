/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// File ResultSet.h
// Piotr Kuchta - ECMWF June 2015

#ifndef ResultSet_H
#define ResultSet_H

#include <vector>
#include <ostream>

#include "odb_api/ecml_data/Matrix.h"

namespace odb {
namespace sql {

class ResultSet {
public:
    ResultSet();
    ~ResultSet(); 

    void append(std::vector<double>&, std::vector<bool>&);

    std::vector<std::vector<double> >& rows() { return matrix_.rows(); }

private:
// No copy allowed
    ResultSet(const ResultSet&);
    ResultSet& operator=(const ResultSet&);

    void print(std::ostream&) const;

    Matrix matrix_;

// -- Friends
    friend std::ostream& operator<<(std::ostream& s, const ResultSet& p)
        { p.print(s); return s; }
};

} // namespace sql
} // namespace odb

#endif
