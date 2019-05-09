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

#include <stdbool.h>

/* Types forward declared, but not defined (only accessible through function API). */

struct odc_reader_t;

struct odc_frame_t;

struct odc_decoder_t;

struct odc_encoder_t;

typedef struct odc_reader_t odc_reader_t;
typedef struct odc_frame_t odc_frame_t;
typedef struct odc_decoder_t odc_decoder_t;
typedef struct odc_encoder_t odc_encoder_t;

/*
 * Initialise API
 * @note This is only required if being used from a context where eckit::Main()
 *       is not otherwise initialised
*/

const int ODC_INTEGERS_AS_DOUBLES = 1;
const int ODC_INTEGERS_AS_LONGS = 2;

int odc_initialise_api();
int odc_integer_behaviour(int integerBehaviour);
int odc_halt_on_failure(bool halt);

int odc_version(const char** version);
int odc_git_sha1(const char** sha1);

/* Constants and lookups */

enum OdcColumnType {
    ODC_IGNORE   = 0,
    ODC_INTEGER  = 1,
    ODC_REAL     = 2,
    ODC_STRING   = 3,
    ODC_BITFIELD = 4,
    ODC_DOUBLE   = 5
};

/* Error handling */

enum OdcErrorValues {
    ODC_SUCCESS = 0,
    ODC_ERROR_ECKIT_EXCEPTION,
    ODC_ERROR_GENERAL_EXCEPTION,
    ODC_ERROR_UNKNOWN_EXCEPTION,
    ODC_ITERATION_COMPLETE
};

const char* odc_error_string(int err);

/* General value checks */

int odc_type_name(int type, const char** type_name);
int odc_set_missing_integer(long missing_integer);
int odc_set_missing_double(double missing_double);

int odc_type_count(int* count);
int odc_missing_integer(long* missing_value);
int odc_missing_double(double* missing_value);

/* Basic READ object API */

typedef long (*stream_read_t)(void* context, void* buffer, long length);
int odc_open_path(odc_reader_t** reader, const char* filename);
int odc_open_file_descriptor(odc_reader_t** reader, int fd);
int odc_open_buffer(odc_reader_t** reader, const void* data, long length);
int odc_open_stream(odc_reader_t** reader, void* context, stream_read_t stream_proc);

int odc_close(const odc_reader_t* o);

/* Table handling */

int odc_new_frame(odc_frame_t** frame, odc_reader_t* reader);
int odc_free_frame(const odc_frame_t* frame);
int odc_next_frame(odc_frame_t* frame);
int odc_next_frame_aggregated(odc_frame_t* frame, long maximum_rows);
int odc_copy_frame(odc_frame_t* source_frame, odc_frame_t** copy);

int odc_frame_row_count(const odc_frame_t* frame, long* count);
int odc_frame_column_count(const odc_frame_t* frame, int* count);
int odc_frame_column_attrs(const odc_frame_t* frame, int col, const char** name, int* type, int* element_size, int* bitfield_count);
int odc_frame_bitfield_attrs(const odc_frame_t* frame, int col, int field, const char** name, int* offset, int* size);

/*
 * Describe how decoding should occur
 */

/* A decode target may allocate its own buffer to decode into, or use one that is
 * externally supplied */

int odc_new_decoder(odc_decoder_t** decoder);
int odc_free_decoder(const odc_decoder_t* decoder);

int odc_decoder_defaults_from_frame(odc_decoder_t* decoder, const odc_frame_t* frame);
int odc_decoder_set_row_count(odc_decoder_t* decoder, long nrows);
int odc_decoder_row_count(const odc_decoder_t* decoder, long* nrows);
int odc_decoder_set_data_array(odc_decoder_t* decoder, void* data, long width, long height, bool columnMajor);
int odc_decoder_data_array(const odc_decoder_t* decoder, const void** data, long* width, long* height, bool* columnMajor);

int odc_decoder_add_column(odc_decoder_t* decoder, const char* name);
int odc_decoder_column_count(const odc_decoder_t* decoder, int* count);

int odc_decoder_set_column_attrs(odc_decoder_t* decoder, int col, int element_size, int stride, void* data);
int odc_decoder_column_attrs(const odc_decoder_t* decoder, int col, int* element_size, int* stride, const void** data);

/*
 * Do the actual decoding
 */

int odc_decode(odc_decoder_t* decoder, const odc_frame_t* frame, long* rows_decoded);
int odc_decode_threaded(odc_decoder_t* decoder, const odc_frame_t* frame, long* rows_decoded, int nthreads);

/*
 * odb_frame_encode - encode data
 */

int odc_new_encoder(odc_encoder_t** encoder);
int odc_free_encoder(const odc_encoder_t* encoder);

int odc_encoder_set_row_count(odc_encoder_t* encoder, long nrows);
int odc_encoder_set_rows_per_frame(odc_encoder_t* encoder, long rows_per_frame);
int odc_encoder_set_data_array(odc_encoder_t* encoder, const void* data, long width, long height, bool columnMajor);

int odc_encoder_add_column(odc_encoder_t* encoder, const char* name, int type);
int odc_encoder_column_set_attrs(odc_encoder_t* encoder, int col, int element_size, int stride, const void* data);
int odc_encoder_column_add_bitfield(odc_encoder_t* encoder, int col, const char* name, int nbits);

typedef long (*stream_write_t)(void* context, const void* buffer, long length);
int odc_encode_to_stream(odc_encoder_t* encoder, void* context, stream_write_t write_fn, long* bytes_encoded);
int odc_encode_to_file_descriptor(odc_encoder_t* encoder, int fd, long* bytes_encoded);
int odc_encode_to_buffer(odc_encoder_t* encoder, void* buffer, long length, long* bytes_encoded);

/*--------------------------------------------------------------------------------------------------------------------*/
