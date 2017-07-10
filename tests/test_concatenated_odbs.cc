#include "eckit/testing/Test.h"
#include "eckit/io/FileHandle.h"
#include "eckit/io/MultiHandle.h"

#include "TemporaryFiles.h"

#include "odb_api/Reader.h"
#include "odb_api/Writer.h"
#include "odb_api/Comparator.h"

#include <cstdint>

using namespace eckit::testing;


// ------------------------------------------------------------------------------------------------------

CASE("ODBs concatenated in a file are valid (columns change)") {

    SETUP("Write multiple ODBs, then concatenate them together") {

        // Construct four ODBs, with differing characteristics

        TemporaryODB tmpODB1([](odb::Writer<>::iterator& writer) {

            writer->setNumberOfColumns(3);
            writer->setColumn(0, "x", odb::REAL);
            writer->setColumn(1, "y", odb::REAL);
            writer->setColumn(2, "z", odb::INTEGER);
            writer->writeHeader();

            for (size_t i = 1; i <= 2; i++) {
                (*writer)[0] = i; // col 0
                (*writer)[1] = i; // col 1
                (*writer)[2] = i; // col 2
                ++writer;
            }
        });

        // Change column names

        TemporaryODB tmpODB2([](odb::Writer<>::iterator& writer) {

            writer->setNumberOfColumns(3);
            writer->setColumn(0, "x", odb::REAL);
            writer->setColumn(1, "y", odb::INTEGER);
            writer->setColumn(2, "v", odb::REAL);
            writer->writeHeader();

            for (size_t i = 1; i <= 2; i++) {
                (*writer)[0] = i * 10; // col 0
                (*writer)[1] = i * 100; // col 1
                (*writer)[2] = i * 1000; // col 2
                ++writer;
            }
        });

        // Increase number of columns

        TemporaryODB tmpODB3([](odb::Writer<>::iterator& writer) {

            writer->setNumberOfColumns(4);
            writer->setColumn(0, "x", odb::INTEGER);
            writer->setColumn(1, "v", odb::REAL);
            writer->setColumn(2, "y", odb::REAL);
            writer->setColumn(3, "z", odb::REAL);
            writer->writeHeader();

            for (size_t i = 1; i <= 2; i++) {
                (*writer)[0] = i * 10; // col 0
                (*writer)[1] = i * 1000; // col 1
                (*writer)[2] = i * 100; // col 2
                (*writer)[3] = 13;     // col 3
                ++writer;
            }
        });

        // Decrease number of columns

        TemporaryODB tmpODB4([](odb::Writer<>::iterator& writer) {

            writer->setNumberOfColumns(2);
            writer->setColumn(0, "x", odb::REAL);
            writer->setColumn(1, "v", odb::REAL);
            writer->writeHeader();

            for (size_t i = 1; i <= 2; i++) {
                (*writer)[0] = i * 5; // col 0
                (*writer)[1] = i * 7; // col 1
                ++writer;
            }
        });

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
            aggregateHandle.openForRead();
            aggregateHandle.saveInto(out_handle);
        }


        SECTION("The data in the concatenated files is correct") {

            odb::Reader in(combinedFile.path());
            odb::Reader::iterator it = in.begin();

            EXPECT(it->columns().size() == 3);
            EXPECT(it->columns()[0]->name() == "x");
            EXPECT(it->columns()[1]->name() == "y");
            EXPECT(it->columns()[2]->name() == "z");
            EXPECT(it->columns().rowsNumber() == 2);

            for (size_t i = 1; i < 3; i++) {
                for (size_t j = 0; j < 3; j++) { EXPECT((*it)[j] == i); }
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

            odb::Reader in(combinedFile.path());
            odb::Reader::iterator it = in.begin();
            odb::Reader::iterator end = in.end();

            TemporaryFile copyFile;
            odb::Writer<> out(copyFile.path());
            odb::Writer<>::iterator o = out.begin();
            o->pass1(it, end);

            // Check that ODB-API thinks the files are the same
            EXPECT_NO_THROW(odb::Comparator().compare(combinedFile.path(), copyFile.path()));
        }
    }
}

// ------------------------------------------------------------------------------------------------------

int main(int argc, char* argv[]) {
    return run_tests(argc, argv);
}

