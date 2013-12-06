/// @file   DataSaver.h
/// @author Tomas Kral

#ifndef DATASAVER_H_
#define DATASAVER_H_

#include <eckit/eckit.h>
//#include <string>

//#include "odblib/Column.h"
//#include "odblib/MetaData.h"
#include "odblib/Writer.h"

namespace odb {

class DataSet;
class DataTable;

/*! Saves data from a dataset into a target file.
 *  
 *  @ingroup data
 */
class DataSaver
{
public:
    /// Creates a new saver and opens the target file.
    DataSaver(const std::string& path);

    /// Closes the target file and destroys the saver object.
   ~DataSaver();

    /*! Saves the @em dataset's tables in the target file.
     *
     *  @param dataset reference to a dataset of interest
     *  @param master  reference to a master table to be saved in the target
     *                 file with all its descendants
     */
    void save(const DataSet& dataset, const DataTable& master);

    /*! Saves the @em dataset's tables in the target file.
     *
     *  @param dataset reference to a dataset of interest
     *  @param master  name of the master table to be saved in the target file
     *                 with all its descendants
     */
    void save(const DataSet& dataset, const std::string& master);

    /// Closes the target file.
    void close();

private:
    DataSaver(const DataSaver&);
    DataSaver& operator=(const DataSaver&);

    Writer<> writer_;
    Writer<>::iterator target_;
};

} // namespace odb

#endif // DATASAVER_H_
