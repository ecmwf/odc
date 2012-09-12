/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#ifndef HashTable_H
#define HashTable_H

#include "odblib/DataStream.h"

namespace odb {

class Reader;
namespace sql { class SQLSelect; }

namespace codec {

struct hashrec {
	hashrec(hashrec *next, const char *name, int cnt, int index)
	: next(next), name(name), cnt(cnt), index(index)
	{}

	~hashrec() { delete next; }

	hashrec *next;
	std::string name;
	int32_t cnt;
	int32_t index;

//private:
//	// No copy allowed.
	explicit hashrec(const hashrec& other)
	: next(other.next ? new hashrec(*other.next) : 0),
	  name(other.name),
	  cnt(other.cnt),
	  index(other.index)
	{}

	hashrec& operator=(const hashrec& other)
	{
		if (&other == this) return *this;
		
		name = other.name;
		cnt = other.cnt;
		index = other.index;
		next = other.next ? new hashrec(*other.next) : 0;
		return *this;
	}
};

class HashTable {
	static int const SIZE = 65535;
public:
	HashTable();
	~HashTable();
	HashTable* clone();

	template<typename BYTEORDER> void save(DataStream<BYTEORDER> &);
	template<typename BYTEORDER> void load(DataStream<BYTEORDER> &);

	void dumpTable(ostream &out) const;
	void store(const char *name);
	int32_t findIndex(const char *name);
	const vector<std::string>& strings() const { return strings_; }
	int32_t nextIndex() const { return nextIndex_; }

//private:
	HashTable(const HashTable&);
	HashTable& operator=(const HashTable&);

protected:
	int32_t nextIndex_;
	hashrec* table[SIZE];
	vector<std::string> strings_;
	
	int32_t hash(const char *);
private:
	bool cloned_;
};

} // namespace codec
} // namespace odb

#endif
