/*
 * (C) Copyright 1996-2012 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @author Simon Smart
/// Aug 2018

#ifndef odc_sql_SQLOutputConfig_H
#define odc_sql_SQLOutputConfig_H

#include "eckit/sql/SQLOutputConfig.h"


namespace odc {
namespace sql {

//----------------------------------------------------------------------------------------------------------------------

class SQLOutputConfig : public eckit::sql::SQLOutputConfig {

public:  // methods

    SQLOutputConfig(bool noColumnNames = false, bool noNULL = false, const std::string& delimiter = defaultDelimiter,
                    const std::string& format = defaultOutputFormat, bool bitfieldsBinary = false,
                    bool noColumnAlignment = false, bool fullPrecision = false);

    SQLOutputConfig(const std::string& odbFilename);

    ~SQLOutputConfig() override;

    eckit::sql::SQLOutput* buildOutput(const eckit::PathName& path) const override;

    void setOutputStream(std::ostream& s);

private:  // members

    std::reference_wrapper<std::ostream> outStream_;
};


//----------------------------------------------------------------------------------------------------------------------

}  // namespace sql
}  // namespace odc

#endif
