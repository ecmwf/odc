/*
 * © Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// \file SQLOutputConfig.h
/// Piotr Kuchta - ECMWF Jul 2010

#ifndef SQLOutputConfig_H
#define SQLOutputConfig_H

#include <string>
using namespace std;

namespace odb {
namespace sql {

class SQLOutputConfig {
public:
	SQLOutputConfig(const SQLOutputConfig& that)
	: doNotWriteColumnNames(that.doNotWriteColumnNames),
	  doNotWriteNULL(that.doNotWriteNULL),
	  fieldDelimiter(that.fieldDelimiter),
	  outputFile(that.outputFile),
	  outputFormat(that.outputFormat)
	{}

	SQLOutputConfig& operator=(const SQLOutputConfig& that)
	{
		doNotWriteColumnNames = that.doNotWriteColumnNames;
		doNotWriteNULL = that.doNotWriteNULL;
		fieldDelimiter = that.fieldDelimiter;
		outputFile = that.outputFile;
		outputFormat = that.outputFormat;
		return *this;
	}


	SQLOutputConfig(bool cn = false, bool n = false, const string& d = defaultDelimiter_, 
                    const string& output = defaultOutputFile_, const string& format = defaultFormat_)
	: doNotWriteColumnNames(cn),
	  doNotWriteNULL(n),
	  fieldDelimiter(d),
      outputFile(output),
      outputFormat(format)
	{}

	static const SQLOutputConfig& defaultConfig() { return defaultConfig_; }

//private:
	bool doNotWriteColumnNames;
	bool doNotWriteNULL;
	string fieldDelimiter;
	string outputFile;          // -o
	string outputFormat;        // -f

	static const SQLOutputConfig defaultConfig_;
	static const char* defaultDelimiter_;
	static const char* defaultOutputFile_;
	static const char* defaultFormat_;
};

} // namespace sql
} // namespace odb

#endif
