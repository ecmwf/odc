/*
 * © Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#ifndef ReptypeGenIterator_H
#define ReptypeGenIterator_H

#include "ODBIterator.h"

namespace eckit { class PathName; }

namespace odb {
namespace tool {

class ODBIterator;

typedef std::vector<size_t> Indices;
typedef std::vector<double> Values;

typedef std::map<Values, int> ReptypeTableBase;

class ReptypeTable : public ReptypeTableBase
{
};

class ReptypeTableConfig {
public:
	static void load(const eckit::PathName&);

	template <typename I>
	static void addColumns(I begin, I end) { columns_.insert(columns_.end(), begin, end); }

    static const std::vector<std::string> columns() { return columns_; }
	static const ReptypeTable& reptypeTable() { return reptypeTable_; }

	static void reptypeTable(const ReptypeTable &rt) { reptypeTable_ = rt; }
private:
    static std::vector<std::string> columns_;
	static ReptypeTable reptypeTable_;
};

template<typename ITERATOR = ODBIterator, typename CONFIG = ReptypeTableConfig>
class ReptypeGenIterator : public odb::RowsReaderIterator {
public:

	ReptypeGenIterator(const eckit::PathName& db, const std::string& sql); 
	~ReptypeGenIterator ();

	odb::MetaData& columns();

	virtual bool isNewDataset();
	virtual double* data();

	const ReptypeGenIterator<ITERATOR,CONFIG>& end() { return *reinterpret_cast<ReptypeGenIterator<ITERATOR,CONFIG>*>(0); }
	//bool operator!=(const ReptypeGenIterator<ITERATOR,CONFIG>& o) { ASSERT(&o == 0); return iterator_ != iterator_.end(); }
	//ReptypeGenIterator<ITERATOR,CONFIG>& operator++() { next(); return *this; }

	int refCount_;
	bool noMore_;

//protected:
	virtual bool next();
	//void addColumn(std::string name);

private:
	ITERATOR iterator_;

	double* data_;

	size_t reptypeIndex_;

	Indices indices_;
	Values values_;
	ReptypeTable reptypeTable_;

	int lastIndex_;
};

} // namespace tool 
} // namespace odb 

#endif
