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

struct odb_decode_target_t;

struct odb_encoder_t;

/*
 * TODO:
 * 1. Finish encoder/decoder
 * 2. Relabel Table --> Frame
 * 3. odb_t types --> odc_t types
 * 4. Add fortran interface
 * 5. Prettify the C header file (need post-processing for odyssey)
 * 5. Default to integer behaviour?
 */

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

enum OdcColumnType {
    ODC_IGNORE   = 0,
    ODC_INTEGER  = 1,
    ODC_REAL     = 2,
    ODC_STRING   = 3,
    ODC_BITFIELD = 4,
    ODC_DOUBLE   = 5
};

int odc_type_count();
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

struct odb_t* odc_open_path(const char* filename);
struct odb_t* odc_open_file_descriptor(int fd);
struct odb_t* odc_open_buffer(const void* data, long length);
struct odb_t* odc_open_stream(void* handle, long (*stream_proc)(void* handle, void* buffer, long length));

void odc_close(struct odb_t* o);

/* Table handling */

struct odb_table_t* odc_alloc_next_table(struct odb_t* o, bool aggregated);
void odc_free_table(struct odb_table_t* o);

int odc_table_row_count(const struct odb_table_t* t);
int odc_table_column_count(const struct odb_table_t* t);
int odc_table_column_type(const struct odb_table_t* t, int col);
int odc_table_column_data_size(const struct odb_table_t* t, int col);
const char* odc_table_column_name(const struct odb_table_t* t, int col);
int odc_table_column_bitfield_count(const struct odb_table_t* t, int col);
const char* odc_table_column_bitfield_field_name(const struct odb_table_t* t, int col, int n);
int odc_table_column_bitfield_field_size(const struct odb_table_t* t, int col, int n);
int odc_table_column_bitfield_field_offset(const struct odb_table_t* t, int col, int n);

/* Decoding data */

// TODO: Is this really the right API?

/* A decode target may allocate its own buffer to decode into, or use one that is
 * externally supplied */
struct odb_decode_target_t* odc_alloc_decode_target();
void odc_free_decode_target(struct odb_decode_target_t* dt);
void odc_decode_target_set_row_count(struct odb_decode_target_t* dt, long nrows);

/* Construct a decode target that will decode an entire table (including allocating buffer space) */
void odc_table_build_all_decode_target(const struct odb_table_t* t, struct odb_decode_target_t* dt);

int odc_decode_target_add_column(struct odb_decode_target_t* dt, const char* name);
void odc_decode_target_column_set_size(struct odb_decode_target_t* dt, int col, int elemSize);
void odc_decode_target_column_set_stride(struct odb_decode_target_t* dt, int col, int stride);
void odc_decode_target_column_set_data(struct odb_decode_target_t* dt, int col, void* data);

const void* odc_decode_target_array_data(struct odb_decode_target_t* dt);
int odc_decode_target_column_size(struct odb_decode_target_t* dt, int col);
int odc_decode_target_column_stride(struct odb_decode_target_t* dt, int col);
const void* odc_decode_target_column_data(struct odb_decode_target_t* dt, int col);


/*
 * odb_table_decode - do the actual decode of the table (allocates if necessary)
 *
 * t - the table opaque pointer
 * dt - the target decode details
 * nthreads - number of threads to use for decoding (if multiple frames)
 *
 * RETURNS: the number of rows decoded
 */

long odc_table_decode(const struct odb_table_t* t, struct odb_decode_target_t* dt, int nthreads);

//const struct odb_decoded_t* odc_table_decode_all(const struct odb_table_t* t);


/*
 * odb_table_encode - encode data
 * TODO: Bit field encoding
 */

struct odb_encoder_t* odc_alloc_encoder();
void odc_free_encoder(struct odb_encoder_t* en);

/* Add a dense column of data */

void odc_encoder_set_row_count(struct odb_encoder_t* en, long nrows);
void odc_encoder_set_rows_per_frame(struct odb_encoder_t* en, long rows_per_frame);
void odc_encoder_set_data_array(struct odb_encoder_t* en, void* data, bool columnMajor);

int odc_encoder_add_column(struct odb_encoder_t* en, const char* name, int type);
void odc_encoder_column_set_size(struct odb_encoder_t* en, int col, int elemSize);
void odc_encoder_column_set_stride(struct odb_encoder_t* en, int col, int stride);
void odc_encoder_column_set_data(struct odb_encoder_t* en, int col, const void*);
void odc_encoder_column_add_bitfield_field(struct odb_encoder_t* en, int col, const char* name, int nbits);

long odc_encode_to_stream(struct odb_encoder_t* en, void* handle, long (*write_fn)(void* handle, const void* buffer, long length));
long odc_encode_to_file_descriptor(struct odb_encoder_t* en, int fd);
long odc_encode_to_buffer(struct odb_encoder_t* en, void* buffer, long length);



//void odc_free_odb_decoded(const struct odb_decoded_t* dt);

/* Encoding data */

//void* odc_encode(const struct odb_decoded_t* dt, void* buffer, long* size);

/*--------------------------------------------------------------------------------------------------------------------*/
