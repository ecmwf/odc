/*
 * (C) Copyright 1996-2012 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/io/Buffer.h"
#include "eckit/io/DataHandle.h"
#include "eckit/io/MemoryHandle.h"
#include "eckit/testing/Test.h"

#include "odc/core/MetaData.h"
#include "odc/Reader.h"
#include "odc/Writer.h"
#include "odc/tools/MockReader.h"
#include "odc/codec/Integer.h"
#include "odc/codec/String.h"

using namespace eckit::testing;

// Constant codecs are a little different from the others, as they can store multiple
// different types within the same codec...


/// Encoding/decoding using codecs and the reader/writer are tested elsewhere.
/// This file is for miscelaneous tests, in case the edge cases elsewhere are insufficient.
///
/// @note This is mainly SDS being paranoid about removing apparently duplicate tests when restructuring

// ------------------------------------------------------------------------------------------------------

namespace {

    // This looks-like a read-iterator. It isn't, but that doesn't matter!

    const int num_rows_to_write = 10;

    class MockReadIterator {
    public:
        MockReadIterator(odc::api::ColumnType type, double data) :
            columns_(1),
            type_(type),
            data_(data),
            nRows_(num_rows_to_write),
            refCount_(0),
            noMore_(false) {

            columns_[0] = new odc::core::Column(columns_);
            ASSERT(columns_[0]);

            columns_[0]->name("a-col");
            columns_[0]->type<odc::core::SameByteOrder>(type_);
            columns_[0]->hasMissing(false);
        }

        odc::core::MetaData& columns() { return columns_; }
        bool isNewDataset()      { return false; }
        double* data()           { return &data_; }

        bool next() {
            if (nRows_ == 0) return false;
            nRows_--;
            if (nRows_ == 0) noMore_ = true;
            return true;
        }

    protected:
        odc::core::MetaData columns_;
        odc::api::ColumnType type_;
        double data_;
        int nRows_;

    public: // Required for IteratorProxy
        int refCount_;
        bool noMore_;
    };

    // n.b. Cannot use local classes as template arguments until c++11, so declare it here.

    // A constant integer value ...

    const long the_const_value = 20090624;

    struct MockReadIteratorConstInt : public MockReadIterator {
        MockReadIteratorConstInt() : MockReadIterator(odc::api::INTEGER, the_const_value) {
            columns_[0]->coder(std::unique_ptr<odc::core::Codec>(new odc::codec::CodecInt32<odc::core::SameByteOrder>));
        }
    };

    // A constant string value (the full 8 bytes)

    const char* const_string_1 = "a-string";

    struct MockReadIteratorConstString1 : public MockReadIterator {
        MockReadIteratorConstString1() : MockReadIterator(odc::api::STRING, *reinterpret_cast<const double*>(const_string_1)) {
            columns_[0]->coder(std::unique_ptr<odc::core::Codec>(new odc::codec::CodecChars<odc::core::SameByteOrder>));
        }
    };

    // A constant string value (shorter than 8 bytes)

    const char* const_string_2 = "pies\0\0\0\0";

    struct MockReadIteratorConstString2 : public MockReadIterator {
        MockReadIteratorConstString2() : MockReadIterator(odc::api::STRING, *reinterpret_cast<const double*>(const_string_2)) {
            columns_[0]->coder(std::unique_ptr<odc::core::Codec>(new odc::codec::CodecChars<odc::core::SameByteOrder>));
        }
    };
}


CASE("The constant integer codec stores a constant integer") {

    // Construct the encoded stuff

    eckit::Buffer buf(4096);

    eckit::MemoryHandle writeDH(buf);

    {
        odc::Writer<> oda(writeDH);
        odc::Writer<>::iterator outit = oda.begin();

        odc::tool::MockReader<MockReadIteratorConstInt> reader;
        outit->pass1(reader.begin(), reader.end());
    }

    // And test that this decodes correctly

    {
        eckit::MemoryHandle dh(buf.data(), static_cast<size_t>(writeDH.position()));
        dh.openForRead();
        odc::Reader oda(dh);

        odc::Reader::iterator it = oda.begin();
        odc::Reader::iterator end = oda.end();

        EXPECT(it->columns()[0]->name() == "a-col");

        size_t count = 0;
        for ( ; it != end; ++it) {
            EXPECT(static_cast<long>((*it)[0]) == the_const_value);
            EXPECT((*it)[0] == static_cast<double>(the_const_value));
            count++;
        }

        EXPECT(count == num_rows_to_write);

        // Check that this has used the constant codec.
        EXPECT(it->columns()[0]->coder().name() == "constant");
        EXPECT(it->columns()[0]->type() == odc::api::INTEGER);
    }
}


CASE("The constant codec can also store strings") {

    // Construct the encoded stuff

    eckit::Buffer buf(4096);

    eckit::MemoryHandle writeDH(buf);

    {
        odc::Writer<> oda(writeDH);
        odc::Writer<>::iterator outit = oda.begin();

        odc::tool::MockReader<MockReadIteratorConstString1> reader;
        outit->pass1(reader.begin(), reader.end());
    }

    // And test that this decodes correctly

    {
        eckit::MemoryHandle dh(buf.data(), static_cast<size_t>(writeDH.position()));
        dh.openForRead();
        odc::Reader oda(dh);

        odc::Reader::iterator it = oda.begin();
        odc::Reader::iterator end = oda.end();

        EXPECT(it->columns()[0]->name() == "a-col");

        size_t count = 0;
        for ( ; it != end; ++it) {
            double val = (*it)[0];
            EXPECT(::memcmp(const_string_1, &val, sizeof(val)) == 0);
            count++;
        }

        EXPECT(count == num_rows_to_write);

        // Check that this has used the constant codec.
        EXPECT(it->columns()[0]->coder().name() == "constant_string");
        EXPECT(it->columns()[0]->type() == odc::api::STRING);
    }
}


CASE("The constant codec can also store strings shorter than 8 bytes") {

    // Construct the encoded stuff

    eckit::Buffer buf(4096);

    eckit::MemoryHandle writeDH(buf);

    {
        odc::Writer<> oda(writeDH);
        odc::Writer<>::iterator outit = oda.begin();

        odc::tool::MockReader<MockReadIteratorConstString2> reader;
        outit->pass1(reader.begin(), reader.end());
    }

    // And test that this decodes correctly

    {
        eckit::MemoryHandle dh(buf.data(), static_cast<size_t>(writeDH.position()));
        dh.openForRead();
        odc::Reader oda(dh);

        odc::Reader::iterator it = oda.begin();
        odc::Reader::iterator end = oda.end();

        EXPECT(it->columns()[0]->name() == "a-col");

        size_t count = 0;
        for ( ; it != end; ++it) {
            double val = (*it)[0];
            EXPECT(::memcmp(const_string_2, &val, sizeof(val)) == 0);
            count++;
        }

        EXPECT(count == num_rows_to_write);

        // Check that this has used the constant codec.
        EXPECT(it->columns()[0]->coder().name() == "constant_string");
        EXPECT(it->columns()[0]->type() == odc::api::STRING);
    }
}


CASE("The constant codec can also store doubles") {

    const double constant_value = -987654321.4321e-34;

    // Don't use the pass1 mechanism here. Build a writer explicitly to show that we can

    odc::api::ColumnType types[] = { odc::api::REAL, odc::api::DOUBLE };

    for (size_t i = 0; i < 2; i++) {

        odc::api::ColumnType type = types[i];

        // Construct the encoded stuff

        eckit::Buffer buf(4096);

        eckit::MemoryHandle writeDH(buf);

        {
            odc::Writer<> oda(writeDH);
            odc::Writer<>::iterator outit = oda.begin();

            outit->setNumberOfColumns(1);
            outit->setColumn(0, "abcdefg", type);
            outit->writeHeader();

            for (size_t i = 0; i < num_rows_to_write; i++) {
                (*outit)[0] = constant_value;
                ++outit;
            }
        }

        // And test that this decodes correctly

        {
            eckit::MemoryHandle dh(buf.data(), static_cast<size_t>(writeDH.position()));
            dh.openForRead();
            odc::Reader oda(dh);

            odc::Reader::iterator it = oda.begin();
            odc::Reader::iterator end = oda.end();

            EXPECT(it->columns()[0]->name() == "abcdefg");

            size_t count = 0;
            for ( ; it != end; ++it) {
                double val = (*it)[0];
                EXPECT(val == constant_value);
                count++;
            }

            EXPECT(count == num_rows_to_write);

            // Check that this has used the constant codec.
            EXPECT(it->columns()[0]->coder().name() == "constant");
            EXPECT(it->columns()[0]->type() == type);
        }
    }
}


CASE("Missing values are encoded and decoded correctly") {

    // Create a mapping between the codecs, their associated missing values, and encoded data sizes

    typedef std::map<std::string, std::pair<double, int> > MapType;
    MapType codec_value_map;

    codec_value_map["short_real"]    = std::make_pair(odc::MDI::realMDI(), sizeof(float));
    codec_value_map["short_real2"]   = std::make_pair(odc::MDI::realMDI(), sizeof(float));
    codec_value_map["long_real"]     = std::make_pair(odc::MDI::realMDI(), sizeof(double));
    codec_value_map["int8_missing"]  = std::make_pair(odc::MDI::integerMDI(), sizeof(int8_t));
    codec_value_map["int16_missing"]  = std::make_pair(odc::MDI::integerMDI(), sizeof(int16_t));

    for (MapType::const_iterator it = codec_value_map.begin(); it != codec_value_map.end(); ++it) {

        const std::string& codec_name(it->first);
        double missing_value = it->second.first;
        int encoded_size = it->second.second;

        // Get the appropriate codec

        std::unique_ptr<odc::core::Codec> c(odc::core::CodecFactory::instance().build<odc::core::SameByteOrder>(codec_name));

        EXPECT(c->name() == codec_name);

        // Write data into a buffer

        unsigned char buffer[256];

        unsigned char* next_pos = c->encode(buffer, missing_value);

        EXPECT((next_pos - buffer) == encoded_size);

        // And check that we can decode it again!

        odc::core::DataStream<odc::core::SameByteOrder> ds(buffer, sizeof(buffer));
        c->setDataStream(ds);

        double decoded;
        c->decode(&decoded);

        ASSERT(ds.position() == eckit::Offset(encoded_size));
        ASSERT(decoded == missing_value);
    }
}


// ------------------------------------------------------------------------------------------------------

int main(int argc, char* argv[]) {
    return run_tests(argc, argv);
}

