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

#ifndef odb_api_Partitions_H
#define odb_api_Partitions_H

#include "odb_api/Block.h"
#include "odb_api/Partition.h"

#include <vector>

namespace eckit { class PathName; }

namespace odb {

typedef std::vector<Partition> PartitionsBase;

class Partitions : public PartitionsBase {
public:
    // Write data to files fileNamePrefix.pool_[i] 
    std::vector<eckit::PathName> write(const eckit::PathName& fileNamePrefix) const;

    void addReport(const eckit::PathName& fileName, ullong blockStart, ullong blockLength, ullong seqno, ullong nRows, ullong rowsPerPartition); 

    // Write partiions info to a text file
    void save(const eckit::PathName&);

    std::string str() const;

private:
    friend std::ostream& operator<< (std::ostream&, const Partitions&);
};

} // namespace odb 

#endif 
