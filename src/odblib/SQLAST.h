/*
 * © Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @file SQLAST.h
/// @author Piotr Kuchta, ECMWF April 2009

#ifndef SQLAST_H
#define SQLAST_H



namespace odb {
namespace sql {

typedef pair<long,long> Range;

class ColumnDef
{
public:
	ColumnDef() {}

	ColumnDef(const string& name, const string& type, const Range& range)
	: name_(name), type_(type), range_(range)
	{}

	const string& name() const { return name_; }
	const string& type() const { return type_; }
	const Range& range() const { return range_; }
	
private:
	string name_;
	string type_;
	Range range_;
};

typedef vector<ColumnDef> ColumnDefs; 

typedef pair<string, ColumnDefs> TableDef;

typedef vector<TableDef> TableDefs;

} // namespace sql
} // namespace odb

#endif

