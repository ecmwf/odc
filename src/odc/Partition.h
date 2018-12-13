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

#ifndef odc_Partition_H
#define odc_Partition_H

#include "odc/Block.h"

#include <vector>

namespace eckit { class PathName; }

namespace odc {

class Partition {
public:
    Partition();
    /// Read pool info from partitions info file
    Partition(const eckit::PathName&, size_t);
    Partition(const Partition&);
    Partition& operator=(const Partition&);
   
    //void add(const eckit::PathName&, ullong start, ullong end, ullong seqno, ullong firstRow, ullong nRows);
    void add(const Block&);

    ullong numberOfRows() const;
    ullong numberOfRowsOnLastBlock() const;

    std::vector<Block>& blocks() { return blocks_; }

    ullong rowsOnLastBlock() const { return rowsOnLastBlock_; }
    void rowsOnLastBlock(ullong n) { rowsOnLastBlock_ = n; }

    ullong startOfLastBlock() { return startOfLastBlock_; }
    void startOfLastBlock(ullong n) { startOfLastBlock_ = n; }

    ullong write(const eckit::PathName& fileName) const;
    ullong write(eckit::DataHandle& dh) const;

    std::string str() const;
    std::ostream& save(std::ostream&, size_t poolNumber) const;

private:
    std::vector<Block> blocks_;
    ullong startOfLastBlock_;
    ullong rowsOnLastBlock_;

    friend std::ostream& operator<< (std::ostream&, const Partition&);
};

} // namespace odc 

#endif 
