/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

///
/// \file FixedSizeWriterIterator.cc
///
/// @author Piotr Kuchta, Feb 2009

#include "odc/api/ColumnType.h"
#include "odc/FixedSizeWriterIterator.h"

using namespace eckit;

namespace odc {

FixedSizeWriterIterator::FixedSizeWriterIterator(Owner &owner, DataHandle *dh)
: WriterBufferingIterator(owner, dh, true)
{}

int FixedSizeWriterIterator::writeRow(const double* values, unsigned long count)
{
	double* last = lastValues_;

	ASSERT(count == columns().size());

	unsigned int k = 0;
	unsigned char* p (encodedDataBuffer_);
	p = writeNumberOfRepeatedValues(p, k);

	for ( ; k < count; k++) 
	{
		Column* col = columns_[k];
		p = col->coder().encode(p, values[k]); //, *this->f);
		last[k] = values[k];
	}

	size_t len = p - encodedDataBuffer_;

	DataStream<SameByteOrder> f(this->f);
	f.writeBytes(encodedDataBuffer_.cast<char>(), len);

	nrows_++;

	return 0;
} 

} // namespace odc 
