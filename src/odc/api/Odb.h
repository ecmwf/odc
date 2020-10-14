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
}


namespace odc {
namespace api {

//----------------------------------------------------------------------------------------------------------------------

// Global settings

class Settings {
public: // methods

    static void treatIntegersAsDoubles(bool flag);

    static long integerMissingValue();
    static void setIntegerMissingValue(long val);

    static double doubleMissingValue();
    static void setDoubleMissingValue(double val);

    static const std::string& version();
    static const std::string& gitsha1();
};

//----------------------------------------------------------------------------------------------------------------------

struct SpanImpl;

class SpanVisitor {
public:

    virtual ~SpanVisitor();

    virtual void operator()(const std::string& columnName, const std::set<long>& vals) = 0;
    virtual void operator()(const std::string& columnName, const std::set<double>& vals) = 0;
    virtual void operator()(const std::string& columnName, const std::set<std::string>& vals) = 0;
};


//----------------------------------------------------------------------------------------------------------------------

class Span {

public: // methods

    Span(std::unique_ptr<SpanImpl>&& s);
    Span(Span&&);
    ~Span();

    void visit(SpanVisitor& visitor) const;

    eckit::Offset offset() const;
    eckit::Length length() const;

private: // members

    std::unique_ptr<SpanImpl> impl_;
};


//----------------------------------------------------------------------------------------------------------------------

class Decoder;

class FrameImpl;

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

    size_t rowCount() const;
    size_t columnCount() const;

    eckit::Offset offset() const;
    eckit::Length length() const;

    const std::vector<ColumnInfo>& columnInfo() const;

    Span span(const std::vector<std::string>& columns, bool onlyConstantValues) const;

private: // members

    std::unique_ptr<FrameImpl> impl_;

    friend class Decoder;
};

//----------------------------------------------------------------------------------------------------------------------

struct FrameImpl;

class ReaderImpl;

class Reader {

public: // methods

    Reader(const std::string& path, bool aggregated=true, long rowlimit=-1);
    Reader(eckit::DataHandle& dh, bool aggregated=true, long rowlimit=-1);
    Reader(eckit::DataHandle* dh, bool aggregated=true, long rowlimit=-1); // takes ownership
    ~Reader();

    Frame next();

private: // members

    std::unique_ptr<ReaderImpl> impl_;
};

//----------------------------------------------------------------------------------------------------------------------

struct DecoderImpl;

class Decoder {

public: // methods

    Decoder(const std::vector<std::string>& columns,
            std::vector<StridedData>& columnFacades);
    ~Decoder();

    Decoder slice(size_t rowOffset, size_t nrows) const;

    void decode(const Frame& frame, size_t nthreads=1);

private: // members

    std::unique_ptr<DecoderImpl> impl_;
};

//----------------------------------------------------------------------------------------------------------------------

void encode(eckit::DataHandle& out,
            const std::vector<ColumnInfo>& columns,
            const std::vector<ConstStridedData>& data,
            const std::map<std::string, std::string>& properties = {},
            size_t maxRowsPerFrame=10000);

//----------------------------------------------------------------------------------------------------------------------

/// odbFromCSV returns number of lines imported

size_t odbFromCSV(eckit::DataHandle& dh_in, eckit::DataHandle& dh_out, const std::string& delimiter=",");
size_t odbFromCSV(std::istream& is, eckit::DataHandle& dh_out, const std::string& delimiter=",");
size_t odbFromCSV(const std::string& in, eckit::DataHandle& dh_out, const std::string& delimiter=",");

//----------------------------------------------------------------------------------------------------------------------

} // namespace api
} // namespace odc

#endif // odc_api_Odb_H
