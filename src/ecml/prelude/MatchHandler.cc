/*
 * (C) Copyright 1996-2016 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "ecml/core/ExecutionContext.h"
#include "ecml/core/Environment.h"
#include "eckit/parser/StringTools.h"
#include "eckit/utils/Regex.h"

#include "MatchHandler.h"

#include <string.h>

using namespace std;
using namespace ecml;

namespace ecml {

MatchHandler::MatchHandler(const std::string& name) : RequestHandler(name) {}

// match, regex = <pattern>, string = <strings>
Values MatchHandler::handle(ExecutionContext& context)
{
    vector<string> patterns (context.environment().lookupList("regex", context));
    vector<string> strings (context.environment().lookupList("strings", context));

    List r;
    for (size_t i(0); i < patterns.size(); ++i)
    {
        const string& pattern (patterns[i]);

        for (size_t j(0); j < strings.size(); ++j)
        {
            // Should this trimming be optional
            const string& s (eckit::StringTools::trim(strings[j]));

            if (eckit::Regex(pattern).match(s))
                r.append(s);
        }
    }

    return r;
}

} // namespace ecml
