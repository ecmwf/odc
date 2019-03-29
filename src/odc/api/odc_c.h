/*
 * (C) Copyright 2019- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/** @note Nothing in this file that CFFI will struggle to parse --> pure c, no preprocessor directives. */
/**       This file should not be included in compile code directly. Include odc/api/odc.h instead.     */

/*--------------------------------------------------------------------------------------------------------------------*/

/* Types forward declared, but not defined (only accessible through function API). */

struct odb_t;

struct odb_table_t;

struct odb_strided_column_t {
    const char name[64];
    int type;       // Only used for encode. Not used in decode at moment
    char* data;
    int nelem;
    int elemSize;
    int stride;
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

const int ODC_NUM_TYPES = 6;
extern const char* ODC_TYPE_NAMES[];
const char* odc_type_name(int type);
long odc_missing_integer();
const char* odc_version();
const char* odc_git_sha1();

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

struct odb_table_t* odc_next_table(struct odb_t* o, bool aggregated);
void odc_free_table(struct odb_table_t* o);

int odc_table_num_rows(const struct odb_table_t* t);
int odc_table_num_columns(const struct odb_table_t* t);
int odc_table_column_type(const struct odb_table_t* t, int col);
int odc_table_column_data_size(const struct odb_table_t* t, int col);
const char* odc_table_column_name(const struct odb_table_t* t, int col);
int odc_table_column_bitfield_count(const struct odb_table_t* t, int col);
const char* odc_table_column_bitfield_field_name(const struct odb_table_t* t, int col, int n);
int odc_table_column_bitfield_field_size(const struct odb_table_t* t, int col, int n);
int odc_table_column_bitfield_field_offset(const struct odb_table_t* t, int col, int n);

/* Decoding data */

//const struct odb_decoded_t* odc_table_decode_all(const struct odb_table_t* t);

/*
 * odb_table_decode - do the actual decode of the table
 *
 * t - the table opaque pointer
 * ncolumns - the number of columns described for decoding
 * nrows - the maximum number of rows to decode
 * columnData - describes the data layout per column
 * nthreads - number of threads to use for decoding (if multiple frames)
 *
 * RETURNS: the number of rows decoded
 */

long odc_table_decode(const struct odb_table_t* t, int ncolumns, long nrows,
                      struct odb_strided_column_t* columnData, int nthreads);

/*
 * odb_table_encode - encode data
 */

void odc_table_encode(void* buffer, long bufferSize,
                      int ncolumns, long nrows,
                      struct odb_strided_column_t* columnData,
                      long maxRowsPerFrame);

void odc_table_encode_to_file(int fd, int ncolumns, long nrows,
                              struct odb_strided_column_t* columnData,
                              long maxRowsPerFrame);

//void odc_free_odb_decoded(const struct odb_decoded_t* dt);

/* Encoding data */

//void* odc_encode(const struct odb_decoded_t* dt, void* buffer, long* size);

/// @note if buffer = NULL, library will allocate one that is to be freed with free

void* odc_import_encode_text(const char* data, const char* delimiter, void* buffer, long* size);
void* odc_import_encode_file(int fd, const char* delimiter, void* buffer, long* size);
void odc_import_encode_file_to_file(int input_fd, int output_fd, const char* delimiter);

/*--------------------------------------------------------------------------------------------------------------------*/
