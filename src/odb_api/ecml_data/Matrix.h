/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// File Matrix.h
// Piotr Kuchta - ECMWF June 2015

#ifndef odb_sql_Matrix_H
#define odb_sql_Matrix_H

#include <vector>
#include <ostream>

namespace odb {
namespace sql {

class Matrix {
public:
    Matrix();
    ~Matrix(); 

    void append(std::vector<double>&, std::vector<bool>&);

    std::vector<std::vector<double> >& rows() { return rows_; }

    size_t numberOfRows() const;
    size_t numberOfColumns() const;

private:
// No copy allowed
    Matrix(const Matrix&);
    Matrix& operator=(const Matrix&);

    void print(std::ostream&) const;

// -- Friends
    friend std::ostream& operator<<(std::ostream& s, const Matrix& p)
        { p.print(s); return s; }

    std::vector<std::vector<double> > rows_;
    std::vector<std::vector<bool> > missing_;
};

} // namespace sql
} // namespace odb

#endif
