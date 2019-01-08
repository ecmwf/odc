/*
 * (C) Copyright 2019- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/* n.b. Nothing in this file that CFFI will struggle to parse --> pure c, no preprocessor directives. */

/*---------------------------------------------------------------------------------------------------------------------*/

/* Types forward declared, but not defined (only accessible through function API). */

struct odb_t;

struct odb_table_t;

/*
 * Initialise API
 * @note This is only required if being used from a context where eckit::Main()
 *       is not otherwise initialised
*/

void odc_initialise_api();

/* Basic READ object API */

struct odb_t* odc_open_for_read(const char* filename);

void odc_close(struct odb_t* o);

/* Table handling */

int odc_num_tables(struct odb_t* o);

struct odb_table_t* odc_get_table(struct odb_t* o, int n);
void odc_free_table(struct odb_table_t* o);

int odc_table_num_rows(struct odb_table_t*);
int odc_table_num_columns(struct odb_table_t*);

/*---------------------------------------------------------------------------------------------------------------------*/
