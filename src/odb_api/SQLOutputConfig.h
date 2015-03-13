/*
 * (C) Copyright 1996-2012 ECMWF.
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

#include "eckit/eckit.h"


namespace odb {
namespace sql {

class SQLOutputConfig {
public:
	SQLOutputConfig(const SQLOutputConfig& that)
	: doNotWriteColumnNames_(that.doNotWriteColumnNames_),
	  doNotWriteNULL_(that.doNotWriteNULL_),
	  fieldDelimiter_(that.fieldDelimiter_),
	  outputFile_(that.outputFile_),
	  outputFormat_(that.outputFormat_),
      displayBitfieldsBinary_(that.displayBitfieldsBinary_),
      disableAlignmentOfColumns_(that.disableAlignmentOfColumns_)
	{}

	SQLOutputConfig& operator=(const SQLOutputConfig& that)
	{
		doNotWriteColumnNames_ = that.doNotWriteColumnNames_;
		doNotWriteNULL_ = that.doNotWriteNULL_;
		fieldDelimiter_ = that.fieldDelimiter_;
		outputFile_ = that.outputFile_;
		outputFormat_ = that.outputFormat_;
        displayBitfieldsBinary_ = that.displayBitfieldsBinary_;
        disableAlignmentOfColumns_ = that.disableAlignmentOfColumns_;
		return *this;
	}


	SQLOutputConfig(bool cn = false,
                    bool n = false,
                    const std::string& d = defaultDelimiter_,
                    const std::string& output = defaultOutputFile_,
                    const std::string& format = defaultFormat_,
                    bool displayBitfieldsBinary = false,
                    bool disableAlignmentOfColumns = false)
	: doNotWriteColumnNames_(cn),
	  doNotWriteNULL_(n),
	  fieldDelimiter_(d),
      outputFile_(output),
      outputFormat_(format),
      displayBitfieldsBinary_(displayBitfieldsBinary),
      disableAlignmentOfColumns_(disableAlignmentOfColumns)
	{}

	bool doNotWriteColumnNames () const { return doNotWriteColumnNames_; }
    void doNotWriteColumnNames(bool b) { doNotWriteColumnNames_ = b; }

	bool doNotWriteNULL () const { return  doNotWriteNULL_; }
	void doNotWriteNULL (bool b) { doNotWriteNULL_ = b; }

    const std::string& fieldDelimiter() const { return fieldDelimiter_; }
    void fieldDelimiter(const std::string& d) { fieldDelimiter_ = d; }

    const std::string& outputFile () const { return outputFile_; }
    void outputFile (const std::string& fn) { outputFile_ = fn; }

    const std::string& outputFormat () const { return outputFormat_; }
    void outputFormat (const std::string& s) { outputFormat_ = s; }

    bool displayBitfieldsBinary () const { return displayBitfieldsBinary_; }
    void displayBitfieldsBinary (bool b) { displayBitfieldsBinary_ = b; }

    bool disableAlignmentOfColumns () const { return disableAlignmentOfColumns_; }
    void disableAlignmentOfColumns (bool b) { disableAlignmentOfColumns_ = b; }

	static const SQLOutputConfig& defaultConfig() { return defaultConfig_; }

private:
	bool doNotWriteColumnNames_;
	bool doNotWriteNULL_;
    std::string fieldDelimiter_;
    std::string outputFile_;          // -o
    std::string outputFormat_;        // -f
    bool displayBitfieldsBinary_;     // --binary
    bool disableAlignmentOfColumns_;  // --no_alignment

	static const SQLOutputConfig defaultConfig_;
	static const char* defaultDelimiter_;
	static const char* defaultOutputFile_;
	static const char* defaultFormat_;
};

} // namespace sql
} // namespace odb

#endif
