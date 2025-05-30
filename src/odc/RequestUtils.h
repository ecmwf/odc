/*
 * (C) Copyright 1996-2018 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


/// @author Simon Smart
/// @date Aug 2018

#ifndef odc_RequestUtils_h
#define odc_RequestUtils_h

#include <map>
#include <string>
#include <vector>


namespace odc {

//----------------------------------------------------------------------------------------------------------------------

typedef std::map<std::string, std::vector<std::string>> RequestDict;

RequestDict unquoteRequestValues(const RequestDict& r);

void checkKeywordsHaveValues(const RequestDict& request, const std::vector<std::string>& keywords);

//----------------------------------------------------------------------------------------------------------------------

}  // namespace odc

#endif
