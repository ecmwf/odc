/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "odb_api/ecml_data/Matrix.h"

namespace odb {
namespace sql {

Matrix::Matrix()
: rows_(),
  missing_()
{}

void Matrix::append(std::vector<double>& values, std::vector<bool>& missing)
{
    rows_.push_back(values);
    missing_.push_back(missing);
}

Matrix::~Matrix() {}

size_t Matrix::numberOfRows() const
{
    return rows_.size();
}

size_t Matrix::numberOfColumns() const
{
    return rows_[0].size();
}


} // namespace sql
} // namespace odb
