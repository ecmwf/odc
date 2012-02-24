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

