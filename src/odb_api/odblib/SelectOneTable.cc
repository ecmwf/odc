/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// \file SelectOneTable.cc
/// Piotr Kuchta - ECMWF Jul 11

#include "odb_api/odblib/SelectOneTable.h"

namespace odb {
namespace sql {

SelectOneTable::SelectOneTable(const SQLTable* table)
: table_(table),
  offset_(0),
  length_(0),
  column_(0),
  table1_(0),
  table2_(0),
  order_(0)
{}

SelectOneTable::~SelectOneTable() {}

} // namespace sql
} // namespace odb
