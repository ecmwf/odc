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

struct odb_column_t { /* CURRENTLY UNUSED */
    const char name[64];
    int colNo;
    int type;
};

struct odb_strided_data_t {
    char* data;
    int nelem;
    int elemSize;
    int stride;
};

struct odb_decoded_t {

    char* ownedData;

    int ncolumns;
    int nrows;

    /* Arrays of length ncolumns */
    struct odb_column_t* columns;
    struct odb_strided_data_t* columnData;
};

/*
 * Initialise API
 * @note This is only required if being used from a context where eckit::Main()
 *       is not otherwise initialised
*/

const int ODC_INTEGERS_AS_DOUBLES = 1;
const int ODC_INTEGERS_AS_LONGS = 2;

void odc_initialise_api();
void odc_integer_behaviour(int integerBehaviour);

/* Constants and lookups */

extern const int ODC_NUM_TYPES;
extern const char* ODC_TYPE_NAMES[];

/* Error handling */

const int ODC_THROW = 1;
const int ODC_ERRORS_CHECKED = 2;
const int ODC_ERRORS_REPORT = 3;

void odc_error_handling(int handling_type);
void odc_reset_error();
const char* odc_error_string();
extern int odc_errno;

/* Basic READ object API */

struct odb_t* odc_open_for_read(const char* filename);
struct odb_t* odc_open_from_fd(int fd);

void odc_close(struct odb_t* o);

/* Table handling */

struct odb_table_t* odc_next_table(struct odb_t* o);
void odc_free_table(struct odb_table_t* o);

int odc_table_num_rows(struct odb_table_t* t);
int odc_table_num_columns(struct odb_table_t* t);
int odc_table_column_type(struct odb_table_t* t, int col);
const char* odc_table_column_name(struct odb_table_t* t, int col);

/* Decoding data */

const struct odb_decoded_t* odc_table_decode_all(const struct odb_table_t* t);
void odc_table_decode(const struct odb_table_t* t, struct odb_decoded_t* dt);

void odc_free_odb_decoded(const struct odb_decoded_t* dt);

/* Encoding data */

//void* odc_encode(const struct odb_decoded_t* dt, void* buffer, long* size);

/// @note if buffer = NULL, library will allocate one that is to be freed with free

void* odc_import_encode_text(const char* data, void* buffer, long* size);
void* odc_import_encode_file(int fd, void* buffer, long* size);
void odc_import_encode_file_to_file(int input_fd, int output_fd);

/*---------------------------------------------------------------------------------------------------------------------*/
