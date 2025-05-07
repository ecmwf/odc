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

#include <map>
#include <memory>
#include <set>
#include <string>
#include <type_traits>
#include <vector>

#include "eckit/io/Length.h"
#include "eckit/io/Offset.h"

#include "odc/api/ColumnInfo.h"
#include "odc/api/ColumnType.h"
#include "odc/api/StridedData.h"

namespace eckit {
class DataHandle;
class Buffer;
}  // namespace eckit


namespace odc {
namespace api {

//----------------------------------------------------------------------------------------------------------------------

/** Provides access to global settings and version utility methods */
class Settings {
public:  // methods

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

/** Provides an interface for accessing the contents of a Span class using a visitor pattern.
 *  The appropriate operator() will be called for each column with its associated values. */
class SpanVisitor {
public:

    virtual ~SpanVisitor();

    /** Method for processing long values
     * \param columnName Column name
     * \param vals Long values
     */
    virtual void operator()(const std::string& columnName, const std::set<long>& vals) = 0;

    /** Method for processing double values
     * \param columnName Column name
     * \param vals Double values
     */
    virtual void operator()(const std::string& columnName, const std::set<double>& vals) = 0;

    /** Method for processing string values
     * \param columnName Column name
     * \param vals String values
     */
    virtual void operator()(const std::string& columnName, const std::set<std::string>& vals) = 0;
};


//----------------------------------------------------------------------------------------------------------------------

/** Provides details of the range of values spanned by each column within a given frame */
class Span {

public:  // methods

    Span(std::unique_ptr<SpanImpl>&& s);
    Span(Span&&);
    Span();
    ~Span();

    /** Visit the Span object with the specified visitor. Calls the appropriate typed method
     *  on the visitor for each column in the Span with the set of associated values.
     * \param visitor Span visitor instance
     */
    void visit(SpanVisitor& visitor) const;

    /** Returns integer values present in the specified column
     * \param column Column name
     * \returns Integer values
     */
    const std::set<long>& getIntegerValues(const std::string& column) const;

    /** Returns floating point values present in the specified column
     * \param column Column name
     * \returns Real values
     */
    const std::set<double>& getRealValues(const std::string& column) const;

    /** Returns string values present in the specified column
     * \param column Column name
     * \returns String values
     */
    const std::set<std::string>& getStringValues(const std::string& column) const;

    /** Compare Spans for equality
     * \param rhs Reference to span object to compare with
     * \returns Returns *true* if span objects are equal
     */
    bool operator==(const Span& rhs) const;

    /** Move assignment operator
     * \param rhs Reference to a span value to assign
     */
    Span& operator=(Span&& rhs);

    /** Returns the offset of the associated frame in the data stream
     * \returns Span offset
     */
    eckit::Offset offset() const;

    /** Returns the length of the encoded data of the associated frame in the data stream
     * \returns Span length
     */
    eckit::Length length() const;

private:  // members

    std::unique_ptr<SpanImpl> impl_;
};


//----------------------------------------------------------------------------------------------------------------------

class Decoder;

class FrameImpl;

/** Provides a viewport onto a chunk of contiguous, compatible data within the ODB-2 stream. This may be a logical Frame
 * comprising multiple underlying frames in the data stream. */
class Frame {

public:  // methods

    Frame();  // Construct a null frame
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

    /** Returns current frame data offset within the data stream owned by the Reader
     * \returns Frame offset
     */
    eckit::Offset offset() const;

    /** Returns the size of the encoded data in bytes
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

    /** Filters current frame according to an SQL-like query and returns another frame object
     *  (which owns its own attached memory buffer)
     *  buffer
     * \param sql SQL query
     * \returns Frame object attached to a memory buffer
     */
    Frame filter(const std::string& sql);

    /** Returns the encoded data of the current frame
     * \returns Encoded frame data
     */
    eckit::Buffer encodedData();

    /** Returns a Span object describing the range of values in the Frame associated with the specified columns,
     *  without decoding the frame.
     * \param columns List of column names
     * \param onlyConstantValues Introduces a constraint that a frame must have constant values in a column if *true*
     * \returns Span object
     */
    Span span(const std::vector<std::string>& columns, bool onlyConstantValues) const;

    /** Returns key/value properties encoded in the current frame
     * \returns Map object
     */
    const std::map<std::string, std::string>& properties() const;

private:  // members

    std::unique_ptr<FrameImpl> impl_;

    friend class Decoder;
};

//----------------------------------------------------------------------------------------------------------------------

class FrameImpl;

class ReaderImpl;

/** Owns the ODB-2 data stream and controls associated resources. The Reader object gives access to the sequence of
 * frames. */
class Reader {

public:  // methods

    /** Construct from file path
     * \param path File path to open
     * \param aggregated Whether to aggregate compatible data into a logical frame
     * \param rowlimit Maximum number of rows to aggregate into one logical frame
     */
    Reader(const std::string& path, bool aggregated = true, long rowlimit = -1);
    /** Construct from data handle reference. This does not take ownership of the data handle,
     *  and managing the lifetime of this data handle is the responsibility of the caller.
     * \param dh Data handle (eckit)
     * \param aggregated Whether to aggregate compatible data into a logical frame
     * \param rowlimit Maximum number of rows to aggregate into one logical frame
     */
    Reader(eckit::DataHandle& dh, bool aggregated = true, long rowlimit = -1);
    /** Construct via data handle pointer. This takes ownership of the DataHandle.
     * \param dh Data handle (eckit)
     * \param aggregated Whether to aggregate compatible data into a logical frame
     * \param rowlimit Maximum number of rows to aggregate into one logical frame
     */
    Reader(eckit::DataHandle* dh, bool aggregated = true, long rowlimit = -1);  // takes ownership
    ~Reader();

    /** Advances to the next frame in the stream
     * \returns Next frame
     */
    Frame next();

private:  // members

    std::unique_ptr<ReaderImpl> impl_;
};

//----------------------------------------------------------------------------------------------------------------------

struct DecoderImpl;

/** Specifies the ODB-2 columns to decode and the memory layout for the decoded data */
class Decoder {

public:  // methods

    /** Constructor
     * \param columns The names of the columns to decode
     * \param columnFacades A description of the periodic data layout for each named column
     */
    Decoder(const std::vector<std::string>& columns, std::vector<StridedData>& columnFacades);
    ~Decoder();

    /** Obtain a sub-decoder associated with a contiguous subset of the rows reference by
     *  the main decoder
     * \param rowOffset Row offset where to start slicing
     * \param nrows Number of rows to slice
     * \returns Sliced decoder
     */
    Decoder slice(size_t rowOffset, size_t nrows) const;

    /** Decodes passed frame according to current configuration
     * \param frame Frame object
     * \param nthreads Number of threads
     */
    void decode(const Frame& frame, size_t nthreads = 1);

private:  // members

    std::unique_ptr<DecoderImpl> impl_;
};

//----------------------------------------------------------------------------------------------------------------------

/** Returns a human-readable name of a column data type
 * \param type Column data type
 * \returns Column data type name
 */
const char* columnTypeName(const ColumnType& type);

//----------------------------------------------------------------------------------------------------------------------

/** Encodes ODB-2 into a data handle
 * \param out Data handle (eckit)
 * \param columns Name and type specification for each column to encode
 * \param data Description of the periodic data layout for each column to encode
 * \param properties Dictionary of key/value properties to encode
 * \param maxRowsPerFrame Maximum number of rows per frame
 */
void encode(eckit::DataHandle& out, const std::vector<ColumnInfo>& columns, const std::vector<ConstStridedData>& data,
            const std::map<std::string, std::string>& properties = {}, size_t maxRowsPerFrame = 10000);

//----------------------------------------------------------------------------------------------------------------------

/** Filters ODB-2 data according to an SQL-like query and writes result into another data handle
 * \note Depending on the query, SQL filtering may not be appropriate to do on a per-Frame basis, as aggregate
 *       values won't behave properly. This function allows filtering of an entire data stream.
 * \param sql SQL query
 * \param in Source data handle
 * \param out Target data handle
 * \returns Number of rows written
 */
size_t filter(const std::string& sql, eckit::DataHandle& in, eckit::DataHandle& out);

//----------------------------------------------------------------------------------------------------------------------

/** Imports CSV from a source data handle into a ODB-2 data handle
 * \param dh_in Input data stream (CSV data)
 * \param dh_out ODB-2 data handle (eckit)
 * \param delimiter CSV delimiter
 * \returns Number of imported lines
 */
size_t odbFromCSV(eckit::DataHandle& dh_in, eckit::DataHandle& dh_out, const std::string& delimiter = ",");

/** Imports CSV from a stream handle into a ODB-2 data handle
 * \param is Input data stream (CSV data)
 * \param dh_out ODB-2 data handle (eckit)
 * \param delimiter CSV delimiter
 * \returns Number of imported lines
 */
size_t odbFromCSV(std::istream& is, eckit::DataHandle& dh_out, const std::string& delimiter = ",");

/** Imports CSV from a string into a ODB-2 data handle
 * \param is String containing CSV data
 * \param dh_out ODB-2 data handle (eckit)
 * \param delimiter CSV delimiter
 * \returns Number of imported lines
 */
size_t odbFromCSV(const std::string& in, eckit::DataHandle& dh_out, const std::string& delimiter = ",");

//----------------------------------------------------------------------------------------------------------------------

}  // namespace api
}  // namespace odc

#endif  // odc_api_Odb_H
