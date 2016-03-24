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
      displayBitfieldsHexadecimal_(that.displayBitfieldsHexadecimal_),      
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
        displayBitfieldsHexadecimal_ = that.displayBitfieldsHexadecimal_;        
        disableAlignmentOfColumns_ = that.disableAlignmentOfColumns_;
		return *this;
	}


	SQLOutputConfig(bool cn = false,
                    bool n = false,
                    const std::string& d = defaultDelimiter(),
                    const std::string& output = defaultOutputFile(),
                    const std::string& format = defaultFormat(),
                    bool displayBitfieldsBinary = false,
                    bool displayBitfieldsHexadecimal = false,                    
                    bool disableAlignmentOfColumns = false)
	: doNotWriteColumnNames_(cn),
	  doNotWriteNULL_(n),
	  fieldDelimiter_(d),
      outputFile_(output),
      outputFormat_(format),
      displayBitfieldsBinary_(displayBitfieldsBinary),
      displayBitfieldsHexadecimal_(displayBitfieldsHexadecimal),
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

    bool displayBitfieldsHexadecimal () const { return displayBitfieldsHexadecimal_; }
    void displayBitfieldsHexadecimal (bool b) { displayBitfieldsHexadecimal_ = b; }
    

    bool disableAlignmentOfColumns () const { return disableAlignmentOfColumns_; }
    void disableAlignmentOfColumns (bool b) { disableAlignmentOfColumns_ = b; }

	static const SQLOutputConfig defaultConfig() { return SQLOutputConfig(); }

private:
	bool doNotWriteColumnNames_;
	bool doNotWriteNULL_;
    std::string fieldDelimiter_;
    std::string outputFile_;          // -o
    std::string outputFormat_;        // -f
    bool displayBitfieldsBinary_;     // --binary
    bool displayBitfieldsHexadecimal_; // --hex
    bool disableAlignmentOfColumns_;  // --no_alignment

    static const char* defaultDelimiter() { return "	"; }
    static const char* defaultOutputFile() { return "output.odb"; }
    static const char* defaultFormat() { return "default"; }
};

} // namespace sql
} // namespace odb

#endif
