/*
 * (C) Copyright 2019- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/exception/Exceptions.h"
#include "eckit/runtime/Main.h"

#include "odc/api/odc.h"

#include "odc/api/Odb.h"
#include "odc/api/Table.h"

using namespace odc::api;

extern "C" {

//----------------------------------------------------------------------------------------------------------------------

struct odb_t {
    odb_t(const char* f) : internal(f) {}
    Odb internal;
};

struct odb_table_t {
    odb_table_t(const Table& t) : internal(t) {}
    const Table internal;
};

//----------------------------------------------------------------------------------------------------------------------

// TODO: In a sensible, templated, way catch all exceptions.
//       --> We ought to have a standardised error return process.


/*
 * Initialise API
 * @note This is only required if being used from a context where eckit::Main()
 *       is not otherwise initialised
*/

void odc_initialise_api() {
    static bool initialised = false;

    if (!initialised) {
        const char* argv[2] = {"odc-api", 0};
        eckit::Main::initialise(1, const_cast<char**>(argv));
        initialised = true;
    }
}

/* Basic READ objects */

odb_t* odc_open_for_read(const char* filename) {

    return new odb_t {filename};
}


void odc_close(odb_t* o) {

    ASSERT(o);
    delete o;
}

/*
 * Table handling
 */

int odc_num_tables(struct odb_t* o) {
    return o->internal.numTables();
}

odb_table_t* odc_get_table(odb_t* o, int n) {
    ASSERT(n >= 0 && n < o->internal.numTables());
    return new odb_table_t(o->internal.tables()[n]);
}

void odc_free_table(odb_table_t* t) {
    ASSERT(t);
    delete(t);
}

int odc_table_num_rows(struct odb_table_t* t) {
    ASSERT(t);
    return t->internal.numRows();
}

int odc_table_num_columns(struct odb_table_t* t) {
    ASSERT(t);
    return t->internal.numColumns();
}


//----------------------------------------------------------------------------------------------------------------------

} // extern "C"
