#ifndef Array_H
#define Array_H

#include <strings.h>

/// \file Array.h
/// @author Piotr Kuchta, ECMWF August 2009

namespace odb {

template <typename T>
class Array {
public:
	Array() : size_(0), array_(0), owner_(true) {}

	Array(size_t n)
	: size_(n), array_(new T[n]), owner_(true)
	{
		ASSERT(array_);
		bzero(array_, size_);
	}

	~Array() { if (owner_) delete [] array_; }

	size_t size() { return size_; }

	void size(size_t n)
	{
		if (owner_) delete [] array_;
		array_ = new T[n];
		ASSERT(array_);
		owner_ = true;
		size_ = n;
		bzero(array_, size_);
	}

	void share(void *p, size_t size)
	{
		if (owner_) delete [] array_;
		array_ = reinterpret_cast<T *>(p);
		owner_ = false;
		size_ = size;
	}

	operator T* () { return array_; }

	bool operator== (const T* p) { return array_ == p; }
	bool operator<= (const T* p) { return array_ <= p; }
	bool operator< (const T* p) { return array_ < p; }
	bool operator> (const T* p) { return array_ > p; }
	bool operator>= (const T* p) { return array_ >= p; }

	template <typename P> P* cast() { return reinterpret_cast<P *>(array_); }
	
private:
	size_t size_;
	T *array_;
    bool owner_;
};

} // namespace odb

#endif 

