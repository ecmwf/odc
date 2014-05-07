/// @file   DataSetBuilder.h
/// @author Tomas Kral

#ifndef DATASETBUILDER_H_
#define DATASETBUILDER_H_

#include "odb_api/odblib/DataTableMappings.h"
#include "odb_api/odblib/MetaData.h"

namespace odb {

class DataSet;
class DataTable;
class DataLink;

/*! Builds a dataset from a source metadata.
 *
 *  The DataSetBuilder class is responsible for building DataTable and
 *  DataLink objects of a DataSet given the metadata of a data source.
 *
 *  @ingroup data
 */
class DataSetBuilder
{
public:
    /// Creates a new dataset builder.
    DataSetBuilder(const odb::MetaData& metadata, bool buildLinks);

    /// Creates a new dataset builder providing the table mappings.
    DataSetBuilder(const odb::MetaData& metadata,
            const DataTableMappings& mapping, bool buildLinks);

    /// Builds dataset tables and links.
    void build(DataSet& dataset) const;

private:
    /// Builds dataset tables.
    void buildTables(DataSet& dataset) const;

    /// Builds dataset links.
    void buildLinks(DataSet& dataset) const;

private:
    DataSetBuilder(const DataSetBuilder&);
    DataSetBuilder& operator=(const DataSetBuilder&);

    odb::MetaData metadata_;
    DataTableMappings mapping_;
    bool buildLinks_;
};

} // namespace odb

#endif // DATASETBUILDER_H_
