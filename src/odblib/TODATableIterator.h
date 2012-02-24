#ifndef TODATableIterator_H
#define TODATableIterator_H

#include "oda.h"

#include "SQLDatabase.h"
#include "SQLType.h"
#include "TODATable.h"
#include "ODAColumn.h"

#define SRC __FILE__,__LINE__

namespace odb {
namespace sql {

template <typename T>
class TODATableIterator : public SQLTableIterator {
	typedef T Table;
	typedef typename Table::TReader::iterator iterator;

public:
	TODATableIterator(Table&, iterator, iterator, double*, const vector<odb::sql::SQLColumn*>&);
	virtual ~TODATableIterator();
	virtual void rewind();
	virtual bool next();

private:

	Table &parent;
	iterator reader_;
	iterator end_;
	double* data_;
	const vector<SQLColumn*>& columns_;

	bool firstRow_;

	void updateMetaData();
	void copyRow();
};

} // namespace sql
} // namespace odb

#include "TODATableIterator.cc"

#endif
