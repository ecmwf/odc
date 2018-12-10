/*
 * (C) Copyright 1996-2012 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// File Endian.h
// Baudouin Raoult - ECMWF Dec 03

#ifndef Endian_H
#define Endian_H

#include "odb_api_config.h"
#include "eckit/eckit_config.h"

namespace odc {

template<class T> struct Swap {

	static const int half = sizeof(T) >> 1;
	static const int last = sizeof(T) - 1;
	T operator()(T v)
	{
		unsigned char *p = (unsigned char*)&v;
		for(int i = 0; i < half ; i++) std::swap(p[i],p[last-i]);
		return v;
	}
};

// xlc needs this,
// others will complain about double initialization
#if defined( __xlC__ )
template<class T> const int Swap<T>::half = sizeof(T) >> 1;
template<class T> const int Swap<T>::last = sizeof(T) - 1;
#endif

class Endian {
public:

#ifdef ECKIT_LITTLE_ENDIAN
	template<class T> static T transform(T x)  { return Swap<T>()(x); }
#else
	template<class T> static T transform(T x)  { return x; }
#endif

};

} // namespace odc

#endif
