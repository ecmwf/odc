/// @file   DataLoader.h
/// @author Tomas Kral

#ifndef DATALOADER_H_
#define DATALOADER_H_

#include <string>
#include <iosfwd>

#include "odblib/DataSet.h"
#include "odblib/DataTableMappings.h"
#include "odblib/DataSetBuilder.h"
#include "odblib/DataSetFiller.h"

namespace eclib { class DataHandle; }

namespace odb {

class Select;

/*! Loads data from a data source into a dataset.
 *  
 *  The DataLoader class is responsible for populating a DataSet with data
 *  from a data source. The DataLoader first inspects a schema of a data
 *  source and if necessary creates respective DataTable and DataLink
 *  objects. Then the loader fills all tables and links in a DataSet with
 *  data from the data source.
 *
 *  The following code snippet demonstrates how to use DataLoader to fill
 *  a DataSet with data from ODB file.
 *
 *  @code
 *  DataLoader loader("airep.odb");
 *  DataSet dataset("airep");
 *  loader.fill(dataset);
 *  @endcode
 *
 *  @ingroup data
 */
class DataLoader
{
public:
    /// Creates a new loader given the path to the source.
    DataLoader(const std::string& path);

    /// Creates a new loader given the handle to the source.
    DataLoader(eclib::DataHandle& handle);

    /// Creates a new loader given the input stream to the source.
    DataLoader(std::istream& stream);

    /// Destroys loader object.
   ~DataLoader();

    void mappings(const DataTableMappings& mappings)
    { mappings_ = mappings; }

    /// Opens a new data source for loading.
    void open(const std::string& path);

    /// Selects columns that will be used to populate the dataset.
    void select(const std::string& query);

    /// Populates @em dataset from the data source.
    void fill(DataSet& dataset);

    /// Populates @em dataset from the data source.
    void fill(DataSet& dataset, const std::string& tableName);

    /// Closes the data source.
    void close();

private:
    void fillFromText(DataSet& dataset);
    void fillFromBinary(DataSet& dataset);
    void fillFromQuery(DataSet& dataset, const std::string& name) const;

private:
    DataLoader(const DataLoader&);
    DataLoader& operator=(const DataLoader&);

    std::string query_;
    std::string path_;
    eclib::DataHandle* handle_;
    bool deleteHandle_;
    std::istream* stream_;
    bool deleteStream_;
    DataTableMappings mappings_;
    bool open_;
};

} // namespace odb

#endif // DATALOADER_H_
