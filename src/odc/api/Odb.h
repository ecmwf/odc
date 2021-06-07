/*
 * (C) Copyright 2019- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


/// @author Simon Smart
/// @date January 2019

/// @note The API reference of this code is generated automatically, however only existing members are included in the
/// resultant documentation. In case any new members that provide the public API are added, they must be manually
/// inserted into an appropriate section. Same goes for removal or renaming of existing members, otherwise
/// non-propagated changes can fail the documentation build. Please see <cpp-reference.rst> in this repository for more
/// information.

#ifndef odc_api_Odb_H
#define odc_api_Odb_H

#include <string>
#include <memory>
#include <vector>
#include <type_traits>
#include <set>
#include <map>

#include "eckit/io/Length.h"
#include "eckit/io/Offset.h"

#include "odc/api/ColumnType.h"
#include "odc/api/ColumnInfo.h"
#include "odc/api/StridedData.h"

namespace eckit {
    class DataHandle;
    class Buffer;
}


namespace odc {
namespace api {

//----------------------------------------------------------------------------------------------------------------------

/** Provides global settings and version utility methods */
class Settings {
public: // methods

    /** Sets treatment of integers in ODB-2 data as doubles
     * \param flag Whether to treat integers as doubles (*true*) or longs (*false*)
     */
    static void treatIntegersAsDoubles(bool flag);

    /** Returns the value that identifies a missing integer
     * \returns Missing integer value
     */
    static long integerMissingValue();
    /** Sets the value that identifies a missing integer
     * \param val Missing integer value
     */
    static void setIntegerMissingValue(long val);

    /** Returns the value that identifies a missing double
     * \returns Missing double value
     */
    static double doubleMissingValue();
    /** Sets the value that identifies a missing double
     * \param val Missing double value
     */
    static void setDoubleMissingValue(double val);
    /** Returns release version of the library in human-readable format, e.g. ``1.3.0``
     * \returns Release version
     */
    static const std::string& version();
    /** Returns version control checksum of the latest change, e.g. ``a88011c007a0db48a5d16e296934a197eac2050a``
     * \returns Version control checksum
     */
    static const std::string& gitsha1();
};

//----------------------------------------------------------------------------------------------------------------------

struct SpanImpl;

/** Provides functor interface for values spanned in specific columns within a frame */
class SpanVisitor {
public:

    virtual ~SpanVisitor();

    /** Operator for processing long values
     * \param columnName Column name
     * \param vals Long values
     */
    virtual void operator()(const std::string& columnName, const std::set<long>& vals) = 0;
    /** Operator for processing double values
     * \param columnName Column name
     * \param vals Double values
     */
    virtual void operator()(const std::string& columnName, const std::set<double>& vals) = 0;
    /** Operator for processing string values
     * \param columnName Column name
     * \param vals String values
     */
    virtual void operator()(const std::string& columnName, const std::set<std::string>& vals) = 0;
};


//----------------------------------------------------------------------------------------------------------------------

/** Provides details of the values spanned in specific columns within a frame */
class Span {

public: // methods

    Span(std::unique_ptr<SpanImpl>&& s);
    Span(Span&&);
    Span();
    ~Span();

    /** Activates the functor interface on spanned columns
     * \param visitor Span visitor instance
     */
    void visit(SpanVisitor& visitor) const;

    /** Returns integer values of a column
     * \param column Column name
     * \returns Integer values
     */
    const std::set<long>& getIntegerValues(const std::string& column) const;
    /** Returns real values of a column
     * \param column Column name
     * \returns Real values
     */
    const std::set<double>& getRealValues(const std::string& column) const;
    /** Returns string values of a column
     * \param column Column name
     * \returns String values
     */
    const std::set<std::string>& getStringValues(const std::string& column) const;

    /** Equal-to operator overload
     * \param rhs Reference to span object to compare with
     * \returns Returns *true* if span objects are equal
     */
    bool operator==(const Span& rhs) const;

    /** Assignment operator overload
     * \param rhs Reference to a span value to assign
     */
    Span& operator=(Span&& rhs);

    /** Returns current span offset
     * \returns Span offset
     */
    eckit::Offset offset() const;
    /** Returns current span length
     * \returns Span length
     */
    eckit::Length length() const;

private: // members

    std::unique_ptr<SpanImpl> impl_;
};


//----------------------------------------------------------------------------------------------------------------------

class Decoder;

class FrameImpl;

/** Provides a viewport into a chunk of contiguous data within the ODB-2 stream */
class Frame {

public: // methods

    Frame(); // Construct a null frame
    Frame(std::unique_ptr<FrameImpl>&&);
    Frame(const Frame&);
    Frame(Frame&&);
    ~Frame();

    Frame& operator=(const Frame&);
    Frame& operator=(Frame&&);

    explicit operator bool() const;

    /** Returns number of rows in current frame
     * \returns Number of rows
     */
    size_t rowCount() const;
    /** Returns number of columns in current frame
     * \returns Number of columns
     */
    size_t columnCount() const;

    /** Returns current frame data offset
     * \returns Frame offset
     */
    eckit::Offset offset() const;
    /** Returns current frame data length
     * \returns Frame length
     */
    eckit::Length length() const;

    /** Returns current frame column information
     * \returns Column information
     */
    const std::vector<ColumnInfo>& columnInfo() const;
    /** Checks if frame has a named column
     * \param name Column name
     * \returns *True* if named column exists, *false* otherwise
     */
    bool hasColumn(const std::string& name) const;

    /** Filters current frame according to an SQL-like query and returns another frame object attached to a memory
     *  buffer
     * \param sql SQL query
     * \returns Frame object attached to a memory buffer
     */
    Frame filter(const std::string& sql);
    /** Returns encoded data of the current frame
     * \returns Encoded frame data
     */
    eckit::Buffer encodedData();

    /** Returns a span object for a glimpse into column values without decoding them
     * \param columns List of column names
     * \param onlyConstantValues Introduces a constraint that a frame must have constant values in a column if *true*
     * \returns Span object
     */
    Span span(const std::vector<std::string>& columns, bool onlyConstantValues) const;

    /** Returns key/value properties encoded in the current frame
     * \returns Map object
     */
    const std::map<std::string, std::string>& properties() const;

private: // members

    std::unique_ptr<FrameImpl> impl_;

    friend class Decoder;
};

//----------------------------------------------------------------------------------------------------------------------

class FrameImpl;

class ReaderImpl;

/** Controls the ODB-2 file resources, and gives access to the underlying frames */
class Reader {

public: // methods

    /** Constructor via file path
     * \param path File path to open
     * \param aggregated Whether to aggregate compatible data into a logical frame
     * \param rowlimit Maximum number of aggregated rows
     */
    Reader(const std::string& path, bool aggregated=true, long rowlimit=-1);
    /** Constructor via data handle reference
     * \param dh Data handle (eckit)
     * \param aggregated Whether to aggregate compatible data into a logical frame
     * \param rowlimit Maximum number of aggregated rows
     */
    Reader(eckit::DataHandle& dh, bool aggregated=true, long rowlimit=-1);
    /** Constructor via data handle pointer
     * \param dh Data handle (eckit)
     * \param aggregated Whether to aggregate compatible data into a logical frame
     * \param rowlimit Maximum number of aggregated rows
     */
    Reader(eckit::DataHandle* dh, bool aggregated=true, long rowlimit=-1); // takes ownership
    ~Reader();

    /** Advances to the next frame in the stream
     * \returns Next frame
     */
    Frame next();

private: // members

    std::unique_ptr<ReaderImpl> impl_;
};

//----------------------------------------------------------------------------------------------------------------------

struct DecoderImpl;

/** Specifies which ODB-2 columns should be decoded and the memory that the decoded data should be put into */
class Decoder {

public: // methods

    /** Constructor
     * \param columns Column names
     * \param columnFacades Column data
     */
    Decoder(const std::vector<std::string>& columns,
            std::vector<StridedData>& columnFacades);
    ~Decoder();

    /** Slices provided number of rows
     * \param rowOffset Row offset where to start slicing
     * \param nrows Number of rows to slice
     * \returns Sliced decoder
     */
    Decoder slice(size_t rowOffset, size_t nrows) const;

    /** Decodes passed frame according to current configuration
     * \param frame Frame object
     * \param nthreads Number of threads
     */
    void decode(const Frame& frame, size_t nthreads=1);

private: // members

    std::unique_ptr<DecoderImpl> impl_;
};

//----------------------------------------------------------------------------------------------------------------------

/** Returns a human-readable name of a column data type
 * \param type Column data type
 * \returns Column data type name
 */
const std::string columnTypeName(const ColumnType& type);

//----------------------------------------------------------------------------------------------------------------------

/** Encodes ODB-2 into a data handle
 * \param out Data handle (eckit)
 * \param columns Column information
 * \param data Column data
 * \param properties Additional key/value properties to encode
 * \param maxRowsPerFrame Maximum number of rows per frame
 */
void encode(eckit::DataHandle& out,
            const std::vector<ColumnInfo>& columns,
            const std::vector<ConstStridedData>& data,
            const std::map<std::string, std::string>& properties = {},
            size_t maxRowsPerFrame=10000);

//----------------------------------------------------------------------------------------------------------------------

/** Filters ODB-2 data according to an SQL-like query and writes result into another data handle
 * \note SQL filtering may not be appropriate to do on a per-Frame basis, as aggregate values won't behave properly.
 * \param sql SQL query
 * \param in Source data handle
 * \param out Target data handle
 * \returns Number of rows written
 */
size_t filter(const std::string& sql, eckit::DataHandle& in, eckit::DataHandle& out);

//----------------------------------------------------------------------------------------------------------------------

/** Imports CSV from a source data handle into a ODB-2 data handle
 * \param dh_in CSV data handle (eckit)
 * \param dh_out ODB-2 data handle (eckit)
 * \param delimiter CSV delimiter
 * \returns Number of imported lines
 */
size_t odbFromCSV(eckit::DataHandle& dh_in, eckit::DataHandle& dh_out, const std::string& delimiter=",");

/** Imports CSV from a stream handle into a ODB-2 data handle
 * \param is Stream handle
 * \param dh_out ODB-2 data handle (eckit)
 * \param delimiter CSV delimiter
 * \returns Number of imported lines
 */
size_t odbFromCSV(std::istream& is, eckit::DataHandle& dh_out, const std::string& delimiter=",");

/** Imports CSV from a string into a ODB-2 data handle
 * \param in Stream handle
 * \param dh_out ODB-2 data handle (eckit)
 * \param delimiter CSV delimiter
 * \returns Number of imported lines
 */
size_t odbFromCSV(const std::string& in, eckit::DataHandle& dh_out, const std::string& delimiter=",");

//----------------------------------------------------------------------------------------------------------------------

} // namespace api
} // namespace odc

#endif // odc_api_Odb_H
