/*
 * (C) Copyright 1996-2012 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#ifndef odc_core_Codec_H
#define odc_core_Codec_H


#include <cstring>
#include <limits>

#include "odc/MDI.h"
#include "odc/api/ColumnType.h"
#include "odc/core/CodecFactory.h"
#include "odc/core/DataStream.h"

namespace eckit {
class DataHandle;
}

namespace odc {
namespace core {

//----------------------------------------------------------------------------------------------------------------------


class Codec {
public:

    Codec(const std::string& name, api::ColumnType type);
    virtual ~Codec();

    /// Creates a clone of this codec. NOTE: the clone is not really usefull for coding/decoding, but has the same
    /// stats/missing values as the original codec, which can be useful sometimes.
    virtual std::unique_ptr<Codec> clone();

    const std::string& name() const { return name_; }

    char* encode(char* p, const double& d) { return reinterpret_cast<char*>(encode(reinterpret_cast<uint8_t*>(p), d)); }
    virtual unsigned char* encode(unsigned char* p, const double& d) = 0;
    virtual void decode(double* out)                                 = 0;
    virtual void skip()                                              = 0;

    void setDataStream(GeneralDataStream& ds);
    virtual void setDataStream(DataStream<SameByteOrder>& ds);
    virtual void setDataStream(DataStream<OtherByteOrder>& ds);
    virtual void clearDataStream() = 0;

    void load(GeneralDataStream& ds);
    virtual void load(DataStream<SameByteOrder>& ds);
    virtual void load(DataStream<OtherByteOrder>& ds);
    void save(GeneralDataStream& ds);
    virtual void save(DataStream<SameByteOrder>& ds);
    virtual void save(DataStream<OtherByteOrder>& ds);

    void resetStats() {
        min_ = max_ = missingValue_;
        hasMissing_ = false;
    }

    virtual void gatherStats(const double& v);

    void hasMissing(bool h) { hasMissing_ = h; }
    int32_t hasMissing() const { return hasMissing_; }

    void min(double m) { min_ = m; }
    double min() const { return min_; }

    void max(double m) { max_ = m; }
    double max() const { return max_; }

    virtual void missingValue(double v);
    double rawMissingValue() const { return missingValue_; }
    virtual double missingValue() const { return missingValue_; }

    // Some special functions for string handling inside the CodecOptimizer
    virtual size_t numStrings() const { NOTIMP; }
    virtual void copyStrings(Codec& rhs) { NOTIMP; }

    virtual size_t dataSizeDoubles() const { return 1; }
    virtual void dataSizeDoubles(size_t count) {
        if (count != 1)
            throw eckit::SeriousBug("Data size cannot be changed from 1x8 bytes", Here());
    }

private:  // methods

    virtual void print(std::ostream& s) const;

    friend std::ostream& operator<<(std::ostream& s, const Codec& p) {
        p.print(s);
        return s;
    }

protected:

    std::string name_;

    int32_t hasMissing_;
    double missingValue_;
    double min_;
    double max_;

    api::ColumnType type_;

private:

    Codec(const Codec&);
    Codec& operator=(const Codec&);
};


// template <typename DATASTREAM>
// Codec* Codec::findCodec(const std::string& name, bool differentByteOrder)
//{
//	return AbstractCodecFactory<typename DATASTREAM::DataHandleType>::getCodec(name, differentByteOrder);
// }

/// We need somewhere to distinguish the behaviour for SameByteOrder vs OtherByteOrder. That
/// somewhere is here.

template <typename ByteOrder>
class DataStreamCodec : public Codec {

public:  // methods

    DataStreamCodec(const std::string& name, api::ColumnType type) : Codec(name, type), ds_(0) {}

    using Codec::setDataStream;
    void setDataStream(DataStream<ByteOrder>& ds) override { ds_ = &ds; }
    void clearDataStream() override { ds_ = 0; }

protected:  // methods

    using Codec::load;
    void load(DataStream<ByteOrder>& ds) override {
        // n.b. name read by the CodecFactory.
        ds.read(hasMissing_);
        ds.read(min_);
        ds.read(max_);
        ds.read(missingValue_);
    }

    using Codec::save;
    void save(DataStream<ByteOrder>& ds) override {
        // n.b. Name is written by the _column_ not the codec.
        // ds.write(name_);
        ds.write(hasMissing_);
        ds.write(min_);
        ds.write(max_);
        ds.write(missingValue_);
    }

protected:

    // n.b. ds_ MUST be initialised before it is used.
    DataStream<ByteOrder>& ds() {
        ASSERT(ds_);
        return *ds_;
    }
    DataStream<ByteOrder>* ds_;
};


//----------------------------------------------------------------------------------------------------------------------

}  // namespace core
}  // namespace odc

#endif
