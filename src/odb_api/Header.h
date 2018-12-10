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
/// \file Header.h
///
/// @author Piotr Kuchta, April 2009

#ifndef Header_H
#define Header_H

#include <stdint.h>
#include "eckit/eckit.h"

namespace eckit { class PathName; }
namespace eckit { class DataHandle; }

namespace odc {

const int32_t BYTE_ORDER_INDICATOR = 1;
const uint16_t ODA_MAGIC_NUMBER = 0xffff;

const int32_t FORMAT_VERSION_NUMBER_MAJOR = 0;
const int32_t FORMAT_VERSION_NUMBER_MINOR = 5;

template <typename OWNER>
class Header 
{
public:
	Header (OWNER &owner);
	~Header ();

	size_t dataSize() const { return dataSize_; }
	void dataSize(size_t n) { dataSize_ = n; }

	size_t rowsNumber() const { return rowsNumber_; }
	void rowsNumber(size_t n) { rowsNumber_ = n; }

	int32_t byteOrder() { return byteOrder_; }
	//size_t dataSizeOffset() const { return sizeof(uint16_t) + 3*1 + 3*sizeof(int32_t) + /*MD5*/ sizeof(int32_t)+32 + sizeof(int32_t); }
	//size_t rowsNumberOffset() const { return dataSizeOffset() + 2 * sizeof(int64_t); }

	void load();

	//template <typename DATAHANDLE> void save(DATAHANDLE &);

	void loadAfterMagic();
private:
// No copy allowed.
    Header(const Header&);
    Header& operator=(const Header&);

	template <typename DATASTREAM> void load(DATASTREAM &);

	OWNER& owner_;
	size_t dataSize_;
	size_t rowsNumber_;

	int32_t byteOrder_;
};

} // namespace odc 

#include "odb_api/Header.cc"

#endif
