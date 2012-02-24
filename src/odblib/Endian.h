// File Endian.h
// Baudouin Raoult - ECMWF Dec 03

#ifndef Endian_H
#define Endian_H

#ifndef   machine_H
#include <machine.h>
#endif

#include "Exceptions.h"

namespace odb {

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

#ifdef linux
	template<class T> static T transform(T x)  { return Swap<T>()(x); }
#else
	template<class T> static T transform(T x)  { return x; }
#endif

};

} // namespace odb 

#endif
