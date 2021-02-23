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

/// @note This test should be in test_codecs_end_to_end.cc, but it was causing segfaults with the cray
///       compiler to put them in the same file. Sigh

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

    // A constant string value (shorter than 8 bytes)

    // too-big, aligned storage --> undefined sanitizer is happy with access as casted double
    alignas(sizeof(double)) const char const_string_2[16] = "pies\0\0\0\0";

    struct MockReadIteratorConstString2 : public MockReadIterator {
        MockReadIteratorConstString2() : MockReadIterator(odc::api::STRING, *reinterpret_cast<const double*>(const_string_2)) {
            columns_[0]->coder(std::unique_ptr<odc::core::Codec>(new odc::codec::CodecChars<odc::core::SameByteOrder>(odc::api::STRING)));
        }
    };
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

// ------------------------------------------------------------------------------------------------------

int main(int argc, char* argv[]) {
    return run_tests(argc, argv);
}

