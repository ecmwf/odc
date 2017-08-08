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

#include "odb_api/MetaData.h"
#include "odb_api/Reader.h"
#include "odb_api/Writer.h"
#include "odb_api/tools/MockReader.h"

using namespace eckit::testing;

// Constant codecs are a little different from the others, as they can store multiple
// different types within the same codec...

// ------------------------------------------------------------------------------------------------------

namespace {

    // This looks-like a read-iterator. It isn't, but that doesn't matter!

    const int num_rows_to_write = 10;

    class MockReadIterator {
    public:
        MockReadIterator(odb::ColumnType type, double data) :
            columns_(1),
            type_(type),
            data_(data),
            nRows_(num_rows_to_write),
            refCount_(0),
            noMore_(false),
            context_(0) {

            columns_[0] = new odb::Column(columns_);
            ASSERT(columns_[0]);

            columns_[0]->name("a-col");
            columns_[0]->type<odb::DataStream<odb::SameByteOrder, eckit::DataHandle> >(type_, false);
            columns_[0]->hasMissing(false);
        }

        odb::MetaData& columns() { return columns_; }
        bool isNewDataset()      { return false; }
        double* data()           { return &data_; }

        bool next(ecml::ExecutionContext*) {
            if (nRows_ == 0) return false;
            nRows_--;
            if (nRows_ == 0) noMore_ = true;
            return true;
        }

    protected:
        odb::MetaData columns_;
        odb::ColumnType type_;
        double data_;
        int nRows_;

    public: // Required for IteratorProxy
        int refCount_;
        bool noMore_;
        ecml::ExecutionContext* context_;
    };

    // n.b. Cannot use local classes as template arguments until c++11, so declare it here.

    // A constant integer value ...

    const long the_const_value = 20090624;

    struct MockReadIteratorConstInt : public MockReadIterator {
        MockReadIteratorConstInt() : MockReadIterator(odb::INTEGER, the_const_value) {
            columns_[0]->coder(new odb::codec::CodecInt32<odb::SameByteOrder>);
        }
    };

    // A constant string value (the full 8 bytes)

    const char* const_string_1 = "a-string";

    struct MockReadIteratorConstString1 : public MockReadIterator {
        MockReadIteratorConstString1() : MockReadIterator(odb::STRING, *reinterpret_cast<const double*>(const_string_1)) {
            columns_[0]->coder(new odb::codec::CodecChars<odb::SameByteOrder>);
        }
    };

    // A constant string value (shorter than 8 bytes)

    const char* const_string_2 = "pies\0\0\0\0";

    struct MockReadIteratorConstString2 : public MockReadIterator {
        MockReadIteratorConstString2() : MockReadIterator(odb::STRING, *reinterpret_cast<const double*>(const_string_2)) {
            columns_[0]->coder(new odb::codec::CodecChars<odb::SameByteOrder>);
        }
    };
}


CASE("The constant integer codec stores a constant integer") {

    // Construct the encoded stuff

    eckit::Buffer buf(4096);

    eckit::MemoryHandle writeDH(buf);

    {
        odb::Writer<> oda(writeDH);
        odb::Writer<>::iterator outit = oda.begin();

        odb::tool::MockReader<MockReadIteratorConstInt> reader;
        outit->pass1(reader.begin(), reader.end());
    }

    // And test that this decodes correctly

    {
        eckit::MemoryHandle dh(buf.data(), static_cast<size_t>(writeDH.position()));
        dh.openForRead();
        odb::Reader oda(dh);

        odb::Reader::iterator it = oda.begin();
        odb::Reader::iterator end = oda.end();

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
    }
}


CASE("The constant codec can also store strings") {

    // Construct the encoded stuff

    eckit::Buffer buf(4096);

    eckit::MemoryHandle writeDH(buf);

    {
        odb::Writer<> oda(writeDH);
        odb::Writer<>::iterator outit = oda.begin();

        odb::tool::MockReader<MockReadIteratorConstString1> reader;
        outit->pass1(reader.begin(), reader.end());
    }

    // And test that this decodes correctly

    {
        eckit::MemoryHandle dh(buf.data(), static_cast<size_t>(writeDH.position()));
        dh.openForRead();
        odb::Reader oda(dh);

        odb::Reader::iterator it = oda.begin();
        odb::Reader::iterator end = oda.end();

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
    }
}


CASE("The constant codec can also store strings shorter than 8 bytes") {

    // Construct the encoded stuff

    eckit::Buffer buf(4096);

    eckit::MemoryHandle writeDH(buf);

    {
        odb::Writer<> oda(writeDH);
        odb::Writer<>::iterator outit = oda.begin();

        odb::tool::MockReader<MockReadIteratorConstString2> reader;
        outit->pass1(reader.begin(), reader.end());
    }

    // And test that this decodes correctly

    {
        eckit::MemoryHandle dh(buf.data(), static_cast<size_t>(writeDH.position()));
        dh.openForRead();
        odb::Reader oda(dh);

        odb::Reader::iterator it = oda.begin();
        odb::Reader::iterator end = oda.end();

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
    }
}


// ------------------------------------------------------------------------------------------------------

int main(int argc, char* argv[]) {
    return run_tests(argc, argv);
}

