/*
 * © Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// \file RggRegionCache.h
/// ECMWF July 2010

#ifndef RggRegionCache_H
#define RggRegionCache_H


#include "piconst.h"
#include "RegionCache.h"
#include <vector>

class RggRegionCache : public RegionCache {
public:

// -- Exceptions
	// None

// -- Contructors

	RggRegionCache();

// -- Destructor

	~RggRegionCache(); // Change to virtual if base class

// -- Convertors
	// None

// -- Operators
	// None

// -- Methods
	// None

// -- Overridden methods
	// None

// -- Class members
	// None

// -- Class methods
	// None

protected:

// -- Members
	// None

// -- Methods
	
	// void print(ostream&) const; // Change to virtual if base class	

// -- Overridden methods
	// None

// -- Class members
	// None

// -- Class methods
	// None

private:

// No copy allowed

	RggRegionCache(const RggRegionCache&);
	RggRegionCache& operator=(const RggRegionCache&);

// -- Members
	// None

// -- Methods
	// None

// -- Overridden methods
     virtual double get_resol(const double & val);
     virtual void create_cache(const double &, const int &);

// -- Class members
	// None


// -- Class methods
     int * read_rtablel_2_file(const int &, int *, int *);
     int gauaw(double [], double [], const int &);
     void bsslzr(double [], const int &); 

// -- Friends

	//friend ostream& operator<<(ostream& s,const RggRegionCache& p)
	//	{ p.print(s); return s; }

};

#endif
