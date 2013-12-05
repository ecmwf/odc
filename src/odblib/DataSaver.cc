/// @file   DataSaver.cc
/// @author Tomas Kral

//#include "odblib/DataSaver.h"
//#include "odblib/DataTable.h"
//#include "odblib/DataView.h"

using namespace std;
using namespace eckit;

namespace odb {

DataSaver::DataSaver(const std::string& path)
  : writer_(path),
    target_(writer_.begin())
{}

DataSaver::~DataSaver()
{
    close();
}

void DataSaver::save(const DataSet& dataset, const DataTable& master)
{
    ASSERT(&dataset == master.dataset()
           && "Master table must be owned by the dataset!");
    save(dataset, master.name());
}

void DataSaver::save(const DataSet& dataset, const std::string& master)
{
    DataView view(dataset, master, true);
    odb::MetaData metadata(0);

    for (size_t i = 0; i < view.columns().size(); ++i)
    {
        typedef odb::DataStream<odb::SameByteOrder, DataHandle> DataStream;

        DataColumn& column = view.columns()[i];
        odb::Column* c = new odb::Column(metadata);

        ASSERT(c);

        c->name(column.name());
        c->type<DataStream>(column.type(), false);
        c->missingValue(column.missingValue());
        c->bitfieldDef(column.bitfieldDef());

        metadata.push_back(c);
    }

    target_->columns() = metadata;
    target_->writeHeader();
    size_t size = view.columns().size();

    for (DataView::iterator source = view.begin();
            source != view.end(); ++source, ++target_)
        copy(*source, *source + size, target_->data()); 
}

void DataSaver::close()
{
    target_->close();
}

} // namespace odb
