/*
 * (C) Copyright 1996-2012 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#ifndef odb_api_tests_TemporaryODB_H
#define odb_api_tests_TemporaryODB_H

#include "eckit/filesystem/PathName.h"

#include "odb_api/Writer.h"

//----------------------------------------------------------------------------------------------------------------------

/// A class to be used as a fixture, which writes a temporary ODB file using the supplied
/// initialisation function, and then ensures that it is correctly cleaned up at the
/// end of the test.

class TemporaryODB {

public: // methods

    template <typename InitFunc>
    TemporaryODB(InitFunc f) :
        path_(eckit::PathName::unique("_temporary_select.odb")) {

//        eckit::Timer t("Writing test.odb");
        odb::Writer<> oda(path_);
        odb::Writer<>::iterator writer = oda.begin();
        f(writer);
    }

    ~TemporaryODB() {
        path_.unlink();
    }

    const eckit::PathName& path() const { return path_; }

private: // members

    eckit::PathName path_;
};



//----------------------------------------------------------------------------------------------------------------------

#endif // odb_api_tests_TemporaryODB_H
