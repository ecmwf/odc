/*
 * © Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#ifndef FastODA2Request_H
#define FastODA2Request_H

#include "MetaDataReader.h"
#include "ODAHandle.h"

namespace odb {

template <typename T>
class FastODA2Request {

public:
	FastODA2Request();

	void parseConfig(const string& s);
	void addColumn(const string& keyword, const string& columnName);

	bool scanFile(const PathName&);
	bool scanFile(const PathName&, OffsetList&, LengthList&, vector<ODAHandle*>&);

	string genRequest() const;

	const set<string>& getValues(const string& keyword);
	map<string, double> getUniqueValues();
	map<string, vector<double> > getValues();

	void mergeSimilarBlocks(bool m) { mergeSimilarBlocks_ = m; }
	
	unsigned long long rowsNumber() { return rowsNumber_; }

protected:
	bool collectValues(const MetaData&, ODAHandle&);

	string columnIsNotConstant(const Column& column) { return T::columnIsNotConstant(column); }
	string columnNotFound(const string& columnName) { return T::columnNotFound(columnName); }
	bool duplicateCombination(const string& errorMessage) { return T::duplicateCombination(errorMessage); }

	string patchValue(const string& k, const string& value) const;

private:
	MetaData firstMD_;
	PathName inputFile_;
	vector<string> keywords_;
	vector<string> columnNames_;
	vector<set<string> > values_;
	map<string, set<double> > doubleValues_;

	map<vector<string>, pair<Offset, Offset> > valuesSeen_;
	unsigned long long rowsNumber_;
	bool mergeSimilarBlocks_;
};


struct ODA2RequestServerTraits {
	static string columnIsNotConstant(const Column& column)
	{
		stringstream ss;
		ss << "Column '" << column.name() << "' is not constant"
			<< " (min=" << column.min() << ", max=" << column.max() << ")";
		throw UserError(ss.str());
	}

	static string columnNotFound(const string& columnName)
	{ throw UserError(string("Column '") + columnName + "' not found."); }

	static bool duplicateCombination(const string& errorMessage)
	{ throw UserError(errorMessage); return false; }
};

struct ODA2RequestClientTraits {
	static string columnIsNotConstant(const Column& column) { return string("MULTIPLE"); }
	static string columnNotFound(const string& columnName) { return string("MISSING"); }
	static bool duplicateCombination(const string& errorMessage)
	{ Log::error() << errorMessage << endl; return false; }
};

} // namespace odb

#include "FastODA2Request.cc"

#endif

