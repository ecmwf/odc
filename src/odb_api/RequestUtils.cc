/*
 * (C) Copyright 1996-2018 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <algorithm>
#include <iterator>

#include "eckit/parser/StringTools.h"
#include "eckit/exception/Exceptions.h"
#include "eckit/log/Log.h"
#include "eckit/types/Types.h"

#include "odb_api/RequestUtils.h"

namespace odb {

//----------------------------------------------------------------------------------------------------------------------


RequestDict unquoteRequestValues(const RequestDict& request) {

    RequestDict r;

    for (const auto& kv : request) {
        std::vector<std::string>& values(r[kv.first]);
        std::transform(kv.second.begin(), kv.second.end(), std::back_inserter(values), eckit::StringTools::unQuote);
    }

    return r;
}

void checkKeywordsHaveValues(const RequestDict& request, const std::vector<std::string>& keywords) {

    // Convert to lower case for lookup

    RequestDict lreq;
    for (const auto& kv : request) {
        lreq[eckit::StringTools::lower(kv.first)] = kv.second;
    }

    for (const std::string& kw : keywords) {
        if (lreq.find(kw) == lreq.end() || lreq[kw].size() < 1) {
            throw eckit::UserError("At least one value required for keyword: " + kw, Here());
        }

        eckit::Log::info() << ":: - " << kw << " " << lreq[kw] << std::endl;
    }
}

//----------------------------------------------------------------------------------------------------------------------

} // namespace odb

