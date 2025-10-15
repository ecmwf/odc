/*
 * (C) Copyright 1996-2012 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/io/FileHandle.h"
#include "eckit/io/MemoryHandle.h"
#include "eckit/io/MultiHandle.h"
#include "eckit/testing/Test.h"

#include "TemporaryFiles.h"

#include "odc/Comparator.h"
#include "odc/Reader.h"
#include "odc/Writer.h"
#include "odc/core/Exceptions.h"

#include <stdint.h>

using namespace eckit::testing;


// ------------------------------------------------------------------------------------------------------

CASE("ODBs concatenated in a file are valid (columns change)") {

    SETUP("Write multiple ODBs, then concatenate them together") {

        // Construct four ODBs, with differing characteristics

        class TemporaryODB1 : public TemporaryFile {
        public:

            TemporaryODB1() {
                odc::Writer<> oda(path());
                odc::Writer<>::iterator writer = oda.begin();

                writer->setNumberOfColumns(3);
                writer->setColumn(0, "x", odc::api::REAL);
                writer->setColumn(1, "y", odc::api::REAL);
                writer->setColumn(2, "z", odc::api::INTEGER);
                writer->writeHeader();

                for (size_t i = 1; i <= 2; i++) {
                    (*writer)[0] = i;  // col 0
                    (*writer)[1] = i;  // col 1
                    (*writer)[2] = i;  // col 2
                    ++writer;
                }
            }
        };

        // Change column names

        class TemporaryODB2 : public TemporaryFile {
        public:

            TemporaryODB2() {
                odc::Writer<> oda(path());
                odc::Writer<>::iterator writer = oda.begin();

                writer->setNumberOfColumns(3);
                writer->setColumn(0, "x", odc::api::REAL);
                writer->setColumn(1, "y", odc::api::INTEGER);
                writer->setColumn(2, "v", odc::api::REAL);
                writer->writeHeader();

                for (size_t i = 1; i <= 2; i++) {
                    (*writer)[0] = i * 10;    // col 0
                    (*writer)[1] = i * 100;   // col 1
                    (*writer)[2] = i * 1000;  // col 2
                    ++writer;
                }
            }
        };

        // Increase number of columns

        class TemporaryODB3 : public TemporaryFile {
        public:

            TemporaryODB3() {
                odc::Writer<> oda(path());
                odc::Writer<>::iterator writer = oda.begin();

                writer->setNumberOfColumns(4);
                writer->setColumn(0, "x", odc::api::INTEGER);
                writer->setColumn(1, "v", odc::api::REAL);
                writer->setColumn(2, "y", odc::api::REAL);
                writer->setColumn(3, "z", odc::api::REAL);
                writer->writeHeader();

                for (size_t i = 1; i <= 2; i++) {
                    (*writer)[0] = i * 10;    // col 0
                    (*writer)[1] = i * 1000;  // col 1
                    (*writer)[2] = i * 100;   // col 2
                    (*writer)[3] = 13;        // col 3
                    ++writer;
                }
            }
        };

        // Decrease number of columns

        class TemporaryODB4 : public TemporaryFile {
        public:

            TemporaryODB4() {
                odc::Writer<> oda(path());
                odc::Writer<>::iterator writer = oda.begin();

                writer->setNumberOfColumns(2);
                writer->setColumn(0, "x", odc::api::REAL);
                writer->setColumn(1, "v", odc::api::REAL);
                writer->writeHeader();

                for (size_t i = 1; i <= 2; i++) {
                    (*writer)[0] = i * 5;  // col 0
                    (*writer)[1] = i * 7;  // col 1
                    ++writer;
                }
            }
        };

        TemporaryODB1 tmpODB1;
        TemporaryODB2 tmpODB2;
        TemporaryODB3 tmpODB3;
        TemporaryODB4 tmpODB4;

        // Directly concatenate files (append them to the first one)

        TemporaryFile combinedFile;

        {
            std::vector<eckit::DataHandle*> readHandles;
            readHandles.push_back(new eckit::FileHandle(tmpODB1.path()));
            readHandles.push_back(new eckit::FileHandle(tmpODB2.path()));
            readHandles.push_back(new eckit::FileHandle(tmpODB3.path()));
            readHandles.push_back(new eckit::FileHandle(tmpODB4.path()));

            eckit::MultiHandle aggregateHandle(readHandles);
            eckit::FileHandle out_handle(combinedFile.path());
            aggregateHandle.saveInto(out_handle);
        }


        SECTION("The data in the concatenated files is correct") {

            odc::Reader in(combinedFile.path());
            odc::Reader::iterator it = in.begin();

            EXPECT(it->columns().size() == 3);
            EXPECT(it->columns()[0]->name() == "x");
            EXPECT(it->columns()[1]->name() == "y");
            EXPECT(it->columns()[2]->name() == "z");
            EXPECT(it->columns().rowsNumber() == 2);

            for (size_t i = 1; i < 3; i++) {
                for (size_t j = 0; j < 3; j++) {
                    EXPECT((*it)[j] == i);
                }
                ++it;
            }

            // Check that we have changed the name of a column

            EXPECT(it->columns().size() == 3);
            EXPECT(it->columns()[0]->name() == "x");
            EXPECT(it->columns()[1]->name() == "y");
            EXPECT(it->columns()[2]->name() == "v");
            EXPECT(!it->columns().hasColumn("z"));
            EXPECT(it->columns().rowsNumber() == 2);

            for (size_t i = 1; i < 3; i++) {
                EXPECT((*it)[0] == 10 * i);
                EXPECT((*it)[1] == 100 * i);
                EXPECT((*it)[2] == 1000 * i);
                ++it;
            }

            // Now we have 4 columns

            EXPECT(it->columns().size() == 4);
            EXPECT(it->columns()[0]->name() == "x");
            EXPECT(it->columns()[1]->name() == "v");
            EXPECT(it->columns()[2]->name() == "y");
            EXPECT(it->columns()[3]->name() == "z");
            EXPECT(it->columns().rowsNumber() == 2);

            for (size_t i = 1; i < 3; i++) {
                EXPECT((*it)[0] == 10 * i);
                EXPECT((*it)[1] == 1000 * i);
                EXPECT((*it)[2] == 100 * i);
                ++it;
            }

            // And back to 2 columns

            EXPECT(it->columns().size() == 2);
            EXPECT(it->columns()[0]->name() == "x");
            EXPECT(it->columns()[1]->name() == "v");
            EXPECT(it->columns().rowsNumber() == 2);

            for (size_t i = 1; i < 3; i++) {
                EXPECT((*it)[0] == 5 * i);
                EXPECT((*it)[1] == 7 * i);
                ++it;
            }
        }

        SECTION("A copy of the concatenated file is identical") {

            odc::Reader in(combinedFile.path());
            odc::Reader::iterator it  = in.begin();
            odc::Reader::iterator end = in.end();

            TemporaryFile copyFile;
            odc::Writer<> out(copyFile.path());
            odc::Writer<>::iterator o = out.begin();
            o->pass1(it, end);

            // Check that ODB-API thinks the files are the same
            EXPECT_NO_THROW(odc::Comparator().compare(combinedFile.path(), copyFile.path()));
        }
    }
}


CASE("If corrupt data follows a valid ODB this should not be treated as a new ODB") {

    // See ODB-376

    // Construct a valid ODB in a buffer, followed by some invalid data

    eckit::Buffer buf(4096);

    eckit::MemoryHandle writeDH(buf);

    {
        odc::Writer<> oda(writeDH);
        odc::Writer<>::iterator writer = oda.begin();

        writer->setNumberOfColumns(3);
        writer->setColumn(0, "x", odc::api::REAL);
        writer->setColumn(1, "y", odc::api::REAL);
        writer->setColumn(2, "z", odc::api::INTEGER);
        writer->writeHeader();

        for (size_t i = 1; i <= 2; i++) {
            (*writer)[0] = i;  // col 0
            (*writer)[1] = i;  // col 1
            (*writer)[2] = i;  // col 2
            ++writer;
        }
    }

    // And write some invalid data on the end of the buffer

    const uint32_t invalid_data = 0xBAADF00D;
    writeDH.write(&invalid_data, sizeof(invalid_data));

    // Now read the data. We should get the data back, and then an error...

    eckit::MemoryHandle readDH(buf);
    readDH.openForRead();
    eckit::AutoClose close(readDH);

    odc::Reader in(readDH);
    odc::Reader::iterator it = in.begin();

    EXPECT(static_cast<long>(it->data()[0]) == 1);
    EXPECT(static_cast<long>(it->data()[1]) == 1);
    EXPECT(static_cast<long>(it->data()[2]) == 1);
    ++it;

    EXPECT(static_cast<long>(it->data()[0]) == 2);
    EXPECT(static_cast<long>(it->data()[1]) == 2);
    EXPECT(static_cast<long>(it->data()[2]) == 2);

    // Where we would expect an EOF, or a new table, we now have corrupt data. This increment should
    // NOT succeed, but should complain vociferously!!!

    EXPECT_THROWS_AS(++it, odc::core::ODBInvalid);
}

CASE("If a corrupted ODB (with no row data following the header) then report an error") {

    // See ODB-376

    // Construct a valid ODB in a buffer, followed by some invalid data

    eckit::Buffer buf(4096);

    eckit::MemoryHandle writeDH(buf);

    {
        odc::Writer<> oda(writeDH);
        odc::Writer<>::iterator writer = oda.begin();

        writer->setNumberOfColumns(3);
        writer->setColumn(0, "x", odc::api::REAL);
        writer->setColumn(1, "y", odc::api::REAL);
        writer->setColumn(2, "z", odc::api::INTEGER);
        writer->writeHeader();

        for (size_t i = 1; i <= 2; i++) {
            (*writer)[0] = i;  // col 0
            (*writer)[1] = i;  // col 1
            (*writer)[2] = i;  // col 2
            ++writer;
        }
    }

    // The header size is 320 bytes. Copy the data from the start...

    //    writeDH.write(buf.data(), 322);
    writeDH.write(buf.data(), 322 - 80);  // -80 == we no longer encode (empty) flags

    // Now read the data. We should get the data back, and then an error...

    eckit::MemoryHandle readDH(buf.data(), static_cast<size_t>(writeDH.position()));
    readDH.openForRead();
    eckit::AutoClose close(readDH);

    odc::Reader in(readDH);
    odc::Reader::iterator it = in.begin();

    EXPECT(static_cast<long>(it->data()[0]) == 1);
    EXPECT(static_cast<long>(it->data()[1]) == 1);
    EXPECT(static_cast<long>(it->data()[2]) == 1);
    ++it;

    EXPECT(static_cast<long>(it->data()[0]) == 2);
    EXPECT(static_cast<long>(it->data()[1]) == 2);
    EXPECT(static_cast<long>(it->data()[2]) == 2);

    // Where we would expect an EOF, or a new table, we now have corrupt data. This increment should
    // NOT succeed, but should complain vociferously!!!

    EXPECT_THROWS_AS(++it, eckit::SeriousBug);
}


// ------------------------------------------------------------------------------------------------------

int main(int argc, char* argv[]) {
    return run_tests(argc, argv);
}
