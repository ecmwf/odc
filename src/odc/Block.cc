/*
 * (C) Copyright 1996-2012 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <ostream>

#include "eckit/eckit.h"
#include "eckit/io/Length.h"
#include "eckit/io/Offset.h"

#include "odc/Block.h"
#include "odc/Reader.h"
#include "odc/Select.h"
#include "odc/core/MetaData.h"

using namespace eckit;
using namespace std;

namespace odc {

std::string Block::str() const {
    stringstream ss;
    ss << *this;
    return ss.str();
}

ostream& operator<<(ostream& o, const Block& b) {
    o << "block,file=\"" << b.fileName << "\","
      << "start=" << b.start << ","
      << "end=" << b.end << ","
      << "firstRow=" << b.firstRow << ","
      << "lastRow=" << b.lastRow;
    return o;
}

}  // namespace odc
