/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#ifndef TODATableIterator_H
#define TODATableIterator_H

namespace odb {
namespace sql {

template <typename T>
class TODATableIterator : public SQLTableIterator {
	typedef T Table;
	typedef typename Table::TReader::iterator iterator;

public:
	TODATableIterator(Table&, iterator, iterator, double*, const std::vector<odb::sql::SQLColumn*>&);
	virtual ~TODATableIterator();
	virtual void rewind();
	virtual bool next();

private:

	Table &parent;
	iterator reader_;
	iterator end_;
	double* data_;
	const std::vector<SQLColumn*>& columns_;

	bool firstRow_;

	void updateMetaData();
	void copyRow();
};

} // namespace sql
} // namespace odb

#include "odb_api/TODATableIterator.cc"

#endif
