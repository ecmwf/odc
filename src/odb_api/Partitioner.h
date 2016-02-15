/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @author Piotr Kuchta, ECMWF, January 2016

#ifndef odb_api_Partitioner_H
#define odb_api_Partitioner_H

#include "odb_api/Partition.h"
#include "odb_api/Partitions.h"
#include "odb_api/Indexer.h"

namespace odb {

class Partitioner {
public:
    static Partitions createPartitions(const std::vector<eckit::PathName>&, size_t);
    static Partitions createPartitions(const std::vector<eckit::PathName>&, const std::vector<eckit::PathName>&, size_t);

private:
    static ullong countRows(const std::vector<eckit::PathName>& files, const std::vector<eckit::PathName>& indices);
};

} // namespace odb 

#endif 
