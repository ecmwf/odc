/*
 * (C) Copyright 1996-2012 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/testing/Test.h"
#include "eckit/value/Value.h"
#include "eckit/types/FloatCompare.h"

#include "odb_api/Reader.h"

// Some of the math.h/cmath functions are not clean when switching to C++11
#if __cplusplus <= 199711L
#include <math.h>
#else
#include <cmath>
#define fabs(x) std::fabs((x))
#define modf(x) std::modf((x))
#endif

using namespace eckit::testing;
using eckit::types::is_approximately_equal;


// ------------------------------------------------------------------------------------------------------

struct CellData {
    template <typename T>
    CellData(const std::string n, const T& v, bool m=false) : name(n), value(v), missing(m) {}

    std::string name;
    eckit::Value value;
    bool missing;
};


// Checker picks a couple of rows in 2000010106.odb to test.
// Can be extended to check anything depending on initialisation
// TODO: Build a random ODB, and test it.

class ODBChecker {

public: // types

    typedef std::map<size_t, std::vector<CellData> > RowStore;

public: // methods

    ODBChecker() {

        std::vector<CellData> rowData;

        rowData.push_back(CellData("expver@desc",             std::string("0018    ")));
        rowData.push_back(CellData("andate@desc",             20000101));
        rowData.push_back(CellData("antime@desc",             60000));
        rowData.push_back(CellData("seqno@hdr",               66969));
        rowData.push_back(CellData("obstype@hdr",             2));
        rowData.push_back(CellData("obschar@hdr",             67132561));
        rowData.push_back(CellData("subtype@hdr",             145));
        rowData.push_back(CellData("date@hdr",                20000101));
        rowData.push_back(CellData("time@hdr",                32200));
        rowData.push_back(CellData("rdbflag@hdr",             0));
        rowData.push_back(CellData("status@hdr",              4));
        rowData.push_back(CellData("event1@hdr",              512));
        rowData.push_back(CellData("blacklist@hdr",           0));
        rowData.push_back(CellData("sortbox@hdr",             2147483647,                 true));
        rowData.push_back(CellData("sitedep@hdr",             0));
        rowData.push_back(CellData("statid@hdr",              std::string("MR413SRA")));
        rowData.push_back(CellData("ident@hdr",               0));
        rowData.push_back(CellData("lat@hdr",                 0.831300));
        rowData.push_back(CellData("lon@hdr",                 -2.057394));
        rowData.push_back(CellData("stalt@hdr",               -2147483647.000000,         true));
        rowData.push_back(CellData("modoro@hdr",              717.562744));
        rowData.push_back(CellData("trlat@hdr",               0.831300));
        rowData.push_back(CellData("trlon@hdr",               4.225791));
        rowData.push_back(CellData("instspec@hdr",            3095));
        rowData.push_back(CellData("event2@hdr",              0));
        rowData.push_back(CellData("anemoht@hdr",             0.000000));
        rowData.push_back(CellData("baroht@hdr",              0.000000));
        rowData.push_back(CellData("sensor@hdr",              0));
        rowData.push_back(CellData("numlev@hdr",              1));
        rowData.push_back(CellData("varno_presence@hdr",      12));
        rowData.push_back(CellData("varno@body",              3));
        rowData.push_back(CellData("vertco_type@body",        1));
        rowData.push_back(CellData("rdbflag@body",            0));
        rowData.push_back(CellData("anflag@body",             0));
        rowData.push_back(CellData("status@body",             4));
        rowData.push_back(CellData("event1@body",             33554432));
        rowData.push_back(CellData("blacklist@body",          0));
        rowData.push_back(CellData("entryno@body",            1));
        rowData.push_back(CellData("press@body",              23840.000000));
        rowData.push_back(CellData("press_rl@body",           -2147483647.000000,         true));
        rowData.push_back(CellData("obsvalue@body",           34.739117));
        rowData.push_back(CellData("aux1@body",               35.000000));
        rowData.push_back(CellData("event2@body",             0));
        rowData.push_back(CellData("ppcode@body",             0));
        rowData.push_back(CellData("level@body",              0));
        rowData.push_back(CellData("biascorr@body",           0.000000));
        rowData.push_back(CellData("final_obs_error@errstat", 2.920646));
        rowData.push_back(CellData("obs_error@errstat",       2.920646));
        rowData.push_back(CellData("repres_error@errstat",    -2147483647.000000,         true));
        rowData.push_back(CellData("pers_error@errstat",      -2147483647.000000,         true));
        rowData.push_back(CellData("fg_error@errstat",        3.002484));

        data_[0] = rowData;

        rowData.clear();
        rowData.push_back(CellData("expver@desc",             std::string("0018    ")));
        rowData.push_back(CellData("andate@desc",             20000101));
        rowData.push_back(CellData("antime@desc",             60000));
        rowData.push_back(CellData("seqno@hdr",               6020684));
        rowData.push_back(CellData("obstype@hdr",             7));
        rowData.push_back(CellData("obschar@hdr",             135265490));
        rowData.push_back(CellData("subtype@hdr",             54));
        rowData.push_back(CellData("date@hdr",                20000101));
        rowData.push_back(CellData("time@hdr",                54533));
        rowData.push_back(CellData("rdbflag@hdr",             0));
        rowData.push_back(CellData("status@hdr",              44));
        rowData.push_back(CellData("event1@hdr",              2));
        rowData.push_back(CellData("blacklist@hdr",           16777223));
        rowData.push_back(CellData("sortbox@hdr",             2147483647,                 true));
        rowData.push_back(CellData("sitedep@hdr",             2147483647,                 true));
        rowData.push_back(CellData("statid@hdr",              std::string("     203")));
        rowData.push_back(CellData("ident@hdr",               203));
        rowData.push_back(CellData("lat@hdr",                 -0.933479));
        rowData.push_back(CellData("lon@hdr",                 -2.107894));
        rowData.push_back(CellData("stalt@hdr",               870000.000000));
        rowData.push_back(CellData("modoro@hdr",              0.963609));
        rowData.push_back(CellData("trlat@hdr",               -0.933479));
        rowData.push_back(CellData("trlon@hdr",               4.175291));
        rowData.push_back(CellData("instspec@hdr",            9215));
        rowData.push_back(CellData("event2@hdr",              0));
        rowData.push_back(CellData("anemoht@hdr",             0.000000));
        rowData.push_back(CellData("baroht@hdr",              0.000000));
        rowData.push_back(CellData("sensor@hdr",              0));
        rowData.push_back(CellData("numlev@hdr",              0));
        rowData.push_back(CellData("varno_presence@hdr",      1032));
        rowData.push_back(CellData("varno@body",              119));
        rowData.push_back(CellData("vertco_type@body",        3));
        rowData.push_back(CellData("rdbflag@body",            0));
        rowData.push_back(CellData("anflag@body",             48));
        rowData.push_back(CellData("status@body",             44));
        rowData.push_back(CellData("event1@body",             512));
        rowData.push_back(CellData("blacklist@body",          0));
        rowData.push_back(CellData("entryno@body",            6));
        rowData.push_back(CellData("press@body",              6.000000));
        rowData.push_back(CellData("press_rl@body",           -2147483647.000000,         true));
        rowData.push_back(CellData("obsvalue@body",           243.490005));
        rowData.push_back(CellData("aux1@body",               -2147483647.000000,         true));
        rowData.push_back(CellData("event2@body",             0));
        rowData.push_back(CellData("ppcode@body",             0));
        rowData.push_back(CellData("level@body",              0));
        rowData.push_back(CellData("biascorr@body",           0.493023));
        rowData.push_back(CellData("final_obs_error@errstat", 0.600000));
        rowData.push_back(CellData("obs_error@errstat",       0.600000));
        rowData.push_back(CellData("repres_error@errstat",    -2147483647.000000,         true));
        rowData.push_back(CellData("pers_error@errstat",      0.958788));
        rowData.push_back(CellData("fg_error@errstat",        0.269232));

        data_[371426] = rowData;
    }

    ~ODBChecker() {}

    void checkRow(size_t num, const odb::Reader::iterator& row) {

        if (data_.find(num) != data_.end()) {
            std::vector<CellData>& reference(data_[num]);
            EXPECT(reference.size() == row->columns().size());

            for (size_t i = 0; i < reference.size(); i++) {
                EXPECT(row->columns()[i]->name() == reference[i].name);

                // Data is always returned in an array of (8-byte) doubles. Actual data is of types
                // of size <= 8 byte. Needs some casting to access.

                if (reference[i].value.isString()) {
                    std::string s(reinterpret_cast<const char*>(&row->data()[i]), 8);
                    EXPECT(reference[i].value == s);
                } else if (reference[i].value.isNumber()) {
                    double intpart;
                    EXPECT(modf(row->data()[i], &intpart) == 0.0);
                    EXPECT(static_cast<long long>(reference[i].value) == static_cast<long long>(intpart));
                } else if (reference[i].value.isDouble()) {
                    EXPECT(is_approximately_equal(static_cast<double>(reference[i].value), row->data()[i],
                                                  fabs(1.0e-5 * static_cast<double>(reference[i].value))));
                } else {
                    // We don't want unknown data types slipping in here!!!
                    EXPECT(false);
                }

                // Check that the missing values are reported correctly
                EXPECT(reference[i].missing == (row->data()[i] == row->columns()[i]->missingValue()));
            }
        }
    }

    size_t highestRow() const {
        size_t biggestRow = 0;
        for (RowStore::const_iterator it = data_.begin(); it != data_.end(); ++it) {
            biggestRow = (it->first > biggestRow) ? it->first : biggestRow;
        }
        return biggestRow;
    }

private: // members

    RowStore data_;
};


// ------------------------------------------------------------------------------------------------------

CASE("The correct number of rows are decoded") {

    eckit::PathName filename("2000010106.odb");

    odb::Reader in(filename);
    odb::Reader::iterator it = in.begin();

    // Each table in the ODB will contain a maximum of 10000 rows!
    EXPECT(it->columns().rowsNumber() == 10000);

    size_t count = 0;
    for (; it != in.end(); ++it) {

        if (count < 3320000) {
            EXPECT(it->columns().rowsNumber() == 10000);
        } else {
            EXPECT(it->columns().rowsNumber() == 1753);
        }
        count++;
    }

    // All of the lines correctly decoded
    EXPECT(count == 3321753);
}



CASE("The correct data is present in a selection of random rows") {

    eckit::PathName filename("2000010106.odb");

    odb::Reader in(filename);
    odb::Reader::iterator it = in.begin();

    ODBChecker checker;

    size_t biggestRow = checker.highestRow();
    size_t count = 0;

    for (; it != in.end() && count <= biggestRow; ++it) {
        checker.checkRow(count, it);
        count++;
    }
}

// TODO: Test missing values
// TODO: Test reading a randomly created ODB.

// ------------------------------------------------------------------------------------------------------

int main(int argc, char* argv[]) {
    return run_tests(argc, argv);
}

