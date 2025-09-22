/*
 * (C) Copyright 1996-2012 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @author Piotr Kuchta, ECMWF, Oct 2015

#ifndef Block_H
#define Block_H

#include "eckit/filesystem/PathName.h"
#include "eckit/io/Length.h"
#include "eckit/io/MultiHandle.h"
#include "eckit/io/Offset.h"

#include "odc/tools/Tool.h"

#include <vector>

namespace odc {

typedef unsigned long long ullong;

class Block {
public:

    Block() : fileName(), start(), end(), firstRow(), lastRow() {}

    Block(const eckit::PathName& fileName, const eckit::Offset& start, const eckit::Offset& end, const ullong firstRow,
          const ullong lastRow) :
        fileName(fileName), start(start), end(end), firstRow(firstRow), lastRow(lastRow) {}

    Block(const Block& other) :
        fileName(other.fileName),
        start(other.start),
        end(other.end),
        firstRow(other.firstRow),
        lastRow(other.lastRow) {}

    Block& operator=(const Block& other) {
        fileName = other.fileName;
        start    = other.start;
        end      = other.end;
        firstRow = other.firstRow;
        lastRow  = other.lastRow;
        return *this;
    }

    eckit::PathName fileName;
    eckit::Offset start;
    eckit::Offset end;
    ullong firstRow;
    ullong lastRow;

    std::string str() const;
};

std::ostream& operator<<(std::ostream&, const Block&);

}  // namespace odc

#endif
