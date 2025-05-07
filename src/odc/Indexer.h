/*
 * (C) Copyright 1996-2012 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @author Piotr Kuchta, ECMWF, November 2015

#ifndef odc_Indexer_H
#define odc_Indexer_H

#include <vector>

#include "eckit/filesystem/PathName.h"

namespace odc {

typedef std::vector<std::pair<eckit::Offset, eckit::Length> > BlockOffsets;
typedef unsigned long long ullong;

class Indexer {
public:

    static void createIndex(const eckit::PathName&, const eckit::PathName&);
    static std::vector<eckit::PathName> createIndex(const std::vector<eckit::PathName>&);

    static ullong countRows(const std::vector<eckit::PathName>&, const std::vector<eckit::PathName>&);

private:

    static BlockOffsets offsetsOfBlocks(const eckit::PathName&);
};

}  // namespace odc

#endif
