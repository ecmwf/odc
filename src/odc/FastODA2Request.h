/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#ifndef FastODA2Request_H
#define FastODA2Request_H

#include <set>

#include "eckit/filesystem/PathName.h"
#include "eckit/io/Length.h"
#include "eckit/io/Offset.h"

#include "odc/core/MetaData.h"

namespace odc {

class ODAHandle;

template <typename T>
class FastODA2Request {

public:
	FastODA2Request();

	void parseConfig(const std::string& s);
	void addColumn(const std::string& keyword, const std::string& columnName);

	bool scanFile(const eckit::PathName&);
	bool scanFile(const eckit::PathName&, eckit::OffsetList&, eckit::LengthList&, std::vector<ODAHandle*>&);

	std::string genRequest() const;

    const std::set<std::string>& getValues(const std::string& keyword);
	std::map<std::string, double> getUniqueValues();
	std::map<std::string, std::vector<double> > getValues();

	void mergeSimilarBlocks(bool m) { mergeSimilarBlocks_ = m; }
	
	unsigned long long rowsNumber() { return rowsNumber_; }

protected:
    bool collectValues(const core::MetaData&, ODAHandle&);

    std::string columnIsNotConstant(const core::Column& column) { return T::columnIsNotConstant(column); }
	std::string columnNotFound(const std::string& columnName) { return T::columnNotFound(columnName); }
	bool duplicateCombination(const std::string& errorMessage) { return T::duplicateCombination(errorMessage); }

	std::string patchValue(const std::string& k, const std::string& value) const;

private:
    core::MetaData firstMD_;
	eckit::PathName inputFile_;
	std::vector<std::string> keywords_;
	std::vector<std::string> columnNames_;
    std::vector<std::set<std::string> > values_;
	std::map<std::string, std::set<double> > doubleValues_;

    std::map<std::vector<std::string>, std::pair<eckit::Offset, eckit::Offset> > valuesSeen_;
	unsigned long long rowsNumber_;
	bool mergeSimilarBlocks_;
};


struct ODA2RequestServerTraits {
    static std::string columnIsNotConstant(const core::Column& column)
	{
        std::stringstream ss;
		ss << "Column '" << column.name() << "' is not constant"
			<< " (min=" << column.min() << ", max=" << column.max() << ")";
		throw eckit::UserError(ss.str());
	}

	static std::string columnNotFound(const std::string& columnName)
	{ throw eckit::UserError(std::string("Column '") + columnName + "' not found."); }

	static bool duplicateCombination(const std::string& errorMessage)
	{ throw eckit::UserError(errorMessage); return false; }
};

struct ODA2RequestClientTraits {
    static std::string columnIsNotConstant(const core::Column& column) { return std::string("MULTIPLE"); }
	static std::string columnNotFound(const std::string& columnName) { return std::string("MISSING"); }
	static bool duplicateCombination(const std::string& errorMessage)
	{ eckit::Log::error() << errorMessage << std::endl; return false; }
};

} // namespace odc

#include "odc/FastODA2Request.cc"

#endif

