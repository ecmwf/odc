/*
 * (C) Copyright 1996-2012 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#ifndef odc_tests_TemporaryODB_H
#define odc_tests_TemporaryODB_H

#include "eckit/filesystem/PathName.h"

#include "odc/Writer.h"

//----------------------------------------------------------------------------------------------------------------------

/// A class to be used as a fixture, which writes a temporary ODB file using the supplied
/// initialisation function, and then ensures that it is correctly cleaned up at the
/// end of the test.


class TemporaryFile {

public:  // methods

    TemporaryFile() : path_(eckit::PathName::unique("_temporary_testing_file")) {}

    virtual ~TemporaryFile() {
        if (path_.exists()) {
            path_.unlink();
        }
    }

    const eckit::PathName& path() const { return path_; }

private:  // members

    eckit::PathName path_;
};


//----------------------------------------------------------------------------------------------------------------------

#endif  // odc_tests_TemporaryODB_H
