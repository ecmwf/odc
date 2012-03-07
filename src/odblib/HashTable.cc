/*
 * © Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#include "stdint.h"
#include "oda.h"
#include "DataStream.h"

namespace odb {
namespace codec {

HashTable::HashTable()
: nextIndex_(0),
  strings_(0)
{
	//Log::info() << "HashTable@" << this << "::HashTable()" << endl;

	for (size_t i = 0; i < sizeof(table) / sizeof(hashrec *); i++)
		table[i] = 0;
}

HashTable::HashTable(const HashTable& other)
: nextIndex_(other.nextIndex_),
  strings_(0)
{
	for (size_t i = 0; i < sizeof(table) / sizeof(hashrec *); i++)
		table[i] = other.table[i] ? new hashrec(*other.table[i]) : 0;

	if (other.strings_)
	{
		strings_ = new string*[nextIndex_];

		for (int32_t i = 0; i < other.nextIndex_; i++ )
			strings_[i] = other.strings_[i] ? new string(*other.strings_[i]) : 0;
	}
}

HashTable& HashTable::operator=(const HashTable& other)
{
	for (int32_t i = 0; i < nextIndex_; i++)
		delete strings_[i];

	delete [] strings_;

	nextIndex_ = other.nextIndex_;
	for (size_t i = 0; i < sizeof(table) / sizeof(hashrec *); i++)
	{
		delete table[i];
		table[i] = other.table[i] ? new hashrec(*other.table[i]) : 0;
	}
	
	if (other.strings_)
	{
		strings_ = new string*[nextIndex_];

		for (int32_t i = 0; i < other.nextIndex_; i++ )
			strings_[i] = other.strings_[i] ? new string(*other.strings_[i]) : 0;
	}
	return *this;
}

HashTable::~HashTable()
{
	//Log::info() << "HashTable@" << this << "::~HashTable()" << endl;

	if (strings_)
	{
		for (int32_t i = 0; i < nextIndex_; i++ )
			delete strings_[i];
		delete [] strings_;
	}
	for (size_t i = 0; i < sizeof(table) / sizeof(hashrec *); i++)
		delete table[i];
}

int HashTable::hash(const char *name)
{
	int32_t n = 0;

	while(*name)
		n +=  (*name++ - 'A') + (n << 5);

	if(n < 0)
	{
		int32_t m = -n / SIZE;
		n += (m + 1) * SIZE;
	}
	return n % SIZE;
}

void HashTable::store(const char *name)
{
	hashrec *h;
	int32_t    n;

	n = hash(name);
	h = table[n];

	while(h)
	{
		if(h->name == name)
		{
			h->cnt++;
			return;
		}
		h = h->next;
	}

	h = new hashrec(table[n], name, 1, nextIndex_++);
	table[n] = h;

	//Log::info() << "HashTable@" << this << "::store(" << name << "): n = " << n << ", nextIndex = " << nextIndex_  << endl;
}

int32_t HashTable::findIndex(const char *name)
{
	hashrec *h;
	int32_t n;

	n = hash(name);
	h = table[n];

	while(h)
	{
		if(h->name == name)
			return h->index;
		h = h->next;
	}

	Log::error() << "[" << name << "] not in hash" << endl;
	return -1;
}

void HashTable::dumpTable(ostream &out) const
{
	out << "HashTable@" << this << "::dumpTable: begin" << endl;

	for(int32_t i = 0; i < SIZE; i++)
	{
		hashrec *h  = table[i];

		while(h)
		{
			out << "[" << h->name << "] -> " << h->cnt << " " << h->index << endl;
			h = h->next;
		}
	}

	out << "HashTable@" << this << "::dumpTable: end" << endl;
}

template<typename BYTEORDER> 
void HashTable::save(DataStream<BYTEORDER> &f)
{
	//Log::info() << "HashTable@" << this << "::save BEGIN" << endl;

	int32_t n = 0;

	f.writeInt32(nextIndex_);

	for(int32_t i = 0; i < SIZE; i++)
	{
		hashrec *h  = table[i];

		while(h)
		{
			//Log::info() << "HashTable::save: '" << h->name << "'" << endl;

			f.writeString(h->name);
			f.writeInt32(h->cnt);
			f.writeInt32(h->index);
			n++;
			h = h->next;
		}
	}

	ASSERT(n == nextIndex_);

	//Log::info() << "HashTable@" << this << "::save END" << endl;
}

template<typename BYTEORDER> 
void HashTable::load(DataStream<BYTEORDER> &f)
{
	//Log::info() << "HashTable@" << this << "::load BEGIN" << endl;

	f.readInt32(nextIndex_);

	delete strings_;
	strings_ = new std::string* [nextIndex_];

	for(int32_t i = 0; i < nextIndex_; i++)
	{
		std::string s;
		f.readString(s);

		int32_t cnt;
		f.readInt32(cnt);

		int32_t index;
		f.readInt32(index);

		//Log::info() << "HashTable::load: " << index << " : '" << s << "'" << endl;

		ASSERT(index < nextIndex_);
		strings_[index] = new std::string(s);
		ASSERT(strings_[index]);
	}
	//Log::info() << "HashTable@" << this << "::load END" << endl;
}

template void HashTable::save<SameByteOrder>(DataStream<SameByteOrder>&);
template void HashTable::save<OtherByteOrder>(DataStream<OtherByteOrder>&);

template void HashTable::load<OtherByteOrder>(DataStream<OtherByteOrder>&);
template void HashTable::load<SameByteOrder>(DataStream<SameByteOrder>&);

} // namespace codec
} // namespace odb 
