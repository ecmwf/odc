/*
 * (C) Copyright 2019- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


/** @author Simon Smart */
/** @date January 2019 */

#ifndef odc_api_odc_H
#define odc_api_odc_H

#ifdef __cplusplus
extern "C" {
#endif

/*--------------------------------------------------------------------------------------------------------------------*/

#include <stdbool.h>

/**
 * Initialise API
 * @note This is only required if being used from a context where eckit::Main() is not otherwise initialised
 */

///@{

const int ODC_INTEGERS_AS_DOUBLES = 1; // this is the default
const int ODC_INTEGERS_AS_LONGS = 2;

int odc_initialise_api();
int odc_integer_behaviour(int integerBehaviour);

///@}


/** Version accessors */

///@{

/** Human readable release version e.g. 1.2.3 */
int odc_version(const char** version);
/** Version under VCS system, typically a git sha1. Not useful for computing software dependencies. */
int odc_vcs_version(const char** version);

///@}


/** Error handling */

///@{

enum OdcErrorValues {
    ODC_SUCCESS                  = 0,
    ODC_ITERATION_COMPLETE       = 1,
    ODC_ERROR_GENERAL_EXCEPTION  = 2,
    ODC_ERROR_UNKNOWN_EXCEPTION  = 3
};

const char* odc_error_string(int err);

// int odc_abort_on_failure(bool abort); ///< @todo to remove

typedef void (*odc_failure_handler_t)(void* context, int error_code);

/** Set a function to be called on error in addition to returning an error code.
 *  The handler can can access odc_error_string()
 *  The handler can also access std::current_exception() if needed (and even rethrow if required).
 */
int odc_set_failure_handler(odc_failure_handler_t handler, void* context);

///@}


/** Values and Types */

///@{

enum OdcColumnType {
    ODC_IGNORE   = 0,
    ODC_INTEGER  = 1,
    ODC_REAL     = 2,
    ODC_STRING   = 3,
    ODC_BITFIELD = 4,
    ODC_DOUBLE   = 5
};

/** Get the number of column types. */
int odc_column_type_count(int* count);

/** Get the name of the column type identified by the integer */
int odc_column_type_name(int type, const char** type_name);

/// @todo In the top CMakelists.txt assert that in this system C long is 64 bit
/// @todo In the top CMakelists.txt assert that in this system C double is 64 bit


/** Set the value that identifies a missing integer (64 bit long) when encoding and decoding via this interface.
    This differs from the marker used to identify missing values in the encoded ODB message,
    which depends on the codec used. */
int odc_set_missing_integer(long missing_integer);

/** Set the value that identifies a missing double (64 bit double) when encoding and decoding via this interface.
    This differs from the marker used to identify missing values in the encoded ODB message,
    which depends on the codec used. */
int odc_set_missing_double(double missing_double);

/** The value that identifies a missing integer (64 bit long) when encoding and decoding via this interface. */
int odc_missing_integer(long* missing_value);
/** The value that identifies a missing double (64 bit double) when encoding and decoding via this interface. */
int odc_missing_double(double* missing_value);

///@}


/** Basic Read API */

///@{

struct odc_reader_t;
typedef struct odc_reader_t odc_reader_t;
/** Creates a reader and opens the speficied file. */
int odc_open_path(odc_reader_t** reader, const char* filename);

/** Creates a reader from an already open file descriptor.
 *  It will duplicate the file descriptor so the calling code is safe to close the file descriptor.
 */
int odc_open_file_descriptor(odc_reader_t** reader, int fd);

/** Creates a reader from a memory buffer. */
int odc_open_buffer(odc_reader_t** reader, const void* data, long length);

typedef long (*odc_stream_read_t)(void* context, void* buffer, long length);

/** Creates a reader associated to a stream handler. */
int odc_open_stream(odc_reader_t** reader, void* context, odc_stream_read_t stream_proc);

/** Closes and destroys the reader.
 *  Must be called for every reader created.
 */
int odc_close(const odc_reader_t* reader);

///@}


/** Frame handling */

///@{

struct odc_frame_t;
typedef struct odc_frame_t odc_frame_t;

/** Creates a frame object, with client responsible to free it. */
int odc_new_frame(odc_frame_t** frame, odc_reader_t* reader);

/** Frees the frame object. */
int odc_free_frame(const odc_frame_t* frame);

/** Advance to the next frame.
 *  Must be called to access the first frame.
 *  Once iteration is complete (no more valid frames), it returns ODC_ITERATION_COMPLETE
 */
int odc_next_frame(odc_frame_t* frame);

/** Advance to the next logical frame.
    ODB2 files contain multiple encoded frames.
    This function merges multiple sequential frames that have the same structure.
*/
int odc_next_frame_aggregated(odc_frame_t* frame, long maximum_rows);

/** Copies a frame to another newly allocated frame. */
int odc_copy_frame(odc_frame_t* source_frame, odc_frame_t** copy);

/** The number of rows in the frame. */
int odc_frame_row_count(const odc_frame_t* frame, long* count);

/** The number of columns in the frame. */
int odc_frame_column_count(const odc_frame_t* frame, int* count);

/** The properties of a columns in the frame.
    NULL pointers will be understood as a parameter that is not required to be filled in.
    @param frame the frame
    @param col   the column number (0-based)
    @param name  return the name of the column
    @param type  return the type of the column
    @param element_size  return the element size in bytes (guaranteed to be a multiple of 8)
    @param bitfield_count  return the number of entries associated with the bitfield column (0 if type is not ODC_BITFIELD)
 */
int odc_frame_column_attributes(const odc_frame_t* frame, int col, const char** name, int* type, int* element_size, int* bitfield_count);

/** The properties of an entry in a bitfield column.
    NULL pointers will be understood as a parameter that is not required to be filled in.
    @param frame  the frame
    @param col    the column number (0-based)
    @param entry  the entry number in the bitfield (0-based)
    @param name   return the name of the column
    @param offset return which bit at which the entry in the bitfield begins
    @param size   return the entry size in bits
 */
int odc_frame_bitfield_attributes(const odc_frame_t* frame, int col, int entry, const char** name, int* offset, int* size);

///@}



/**
 * Decoding data
 */

///@{

struct odc_decoder_t;
typedef struct odc_decoder_t odc_decoder_t;

/* A decode target may allocate its own buffer to decode into, or use one that is
 * externally supplied */

/** Creates a decoder for decoding ODB2 format */
int odc_new_decoder(odc_decoder_t** decoder);

/** Deallocates the decoder */
int odc_free_decoder(const odc_decoder_t* decoder);

/** Configures a decoder to decode all the data contained in the frame.
 *  This is the most typical way to decode data, and is equivalent to adding all columns to the decoder.
 */
int odc_decoder_defaults_from_frame(odc_decoder_t* decoder, const odc_frame_t* frame);

/** Instruct the decoder to output in column major form */
int odc_decoder_set_column_major(odc_decoder_t* decoder, bool columnMajor);

/** If the decoder is to allocate its buffer internally, specify the number of rows to allocate
    if this is to be different from the number of rows in the frame decoded.
    This is only really useful if we are re-using a decoder. */
int odc_decoder_set_row_count(odc_decoder_t* decoder, long nrows);

/** Get the number of rows the decoder is configured to decode data into */
int odc_decoder_row_count(const odc_decoder_t* decoder, long* nrows);

/**
 * Define the output data array into which the data may be decoded.
 * This is a shortcut to calling odc_decoder_column_set_data_array for every column.
 */
int odc_decoder_set_data_array(odc_decoder_t* decoder, void* data, long width, long height, bool columnMajor);

/** Gets the output data array into which the data may be decoded */
int odc_decoder_data_array(const odc_decoder_t* decoder, const void** data, long* width, long* height, bool* columnMajor);

/**
 *  Adds a column to the set for decoding.
 *  This is an alternative to odc_decoder_defaults_from_frame
 */
int odc_decoder_add_column(odc_decoder_t* decoder, const char* name);

/** The number of columns the decoder is configured to decode */
int odc_decoder_column_count(const odc_decoder_t* decoder, int* count);

/**
 * Set the decoded data size for a column in bytes.
 */
int odc_decoder_column_set_data_size(odc_decoder_t* decoder, int col, int element_size);

/**
 * Sets a specific data array into which the data associated with the column can be decoded
 * This is an alternative to the odc_decoder_set_data_array.
 */
int odc_decoder_column_set_data_array(odc_decoder_t* decoder, int col, int element_size, int stride, void* data);

/**
 * Gets the buffer and data layout into which the data has been decoded.
 * Only valid after calling odc_decode
 */
int odc_decoder_column_data_array(const odc_decoder_t* decoder, int col, int* element_size, int* stride, const void** data);

/**
 * Decode the data described by the frame into the data array(s) configured in the decoder.
 * If no data array has been set above, an array to decode into will be allocated. This can be
 * obtained by using the odc_decoder_data_array function.
 */
int odc_decode(odc_decoder_t* decoder, const odc_frame_t* frame, long* rows_decoded);

/**
 * Decode the data described by the frame into the data array(s) configured in the decoder.
 * If the frame is logical aggregated over multiple frames in the message,
 * then parallelise the decoding over multiple threads.
 */
int odc_decode_threaded(odc_decoder_t* decoder, const odc_frame_t* frame, long* rows_decoded, int nthreads);

///@}



/**
 * Encoding data
 */

///@{

struct odc_encoder_t;
typedef struct odc_encoder_t odc_encoder_t;

/** Creates an encoder for encoding into ODB2 format */
int odc_new_encoder(odc_encoder_t** encoder);

/** Deallocates the encoder */
int odc_free_encoder(const odc_encoder_t* encoder);

/** Declare the number of rows you are going to encode */
int odc_encoder_set_row_count(odc_encoder_t* encoder, long nrows);

/** Overrides the default number of rows which will be grouped together into frames */
int odc_encoder_set_rows_per_frame(odc_encoder_t* encoder, long rows_per_frame);

/** Associate a 2D data array to an encoder and describe its shape.
 *  Note this does not yet encode the data and the data isn't copied.
 *  The client code is responsible for keeping the data in memory until encoded.
 *  There can be only one associated data array at a time per encoder,
 *  but an encoder can be used to encode a sequence of data arrays with interleaved calls to odc_encode_to_* functions.
 *  @param encoder the encoder
 *  @param data the memory buffer
 *  @param width the width of the 2D array in bytes. NOTE it is in BYTES.
 *  @param height the height of the 2D array in rows, being greater or equal than the row count.
 *  @param columnMajorWidth if zero then 2D array is interpreted as row-major, otherwise its the size in bytes of the
 *         column entries, typically 8 bytes.
 */
int odc_encoder_set_data_array(odc_encoder_t* encoder, const void* data, long width, long height, int columnMajorWidth);

/// @todo implment the int columnMajorWidth  in the above function

/** Describes a new column in the encoder */
int odc_encoder_add_column(odc_encoder_t* encoder, const char* name, int type);

/** Describes the column element size overriding the default for the type
 *  Typically used for string types where the length is greater than 8 bytes
 */
int odc_encoder_column_set_element_size(odc_encoder_t* encoder, int col, int element_size);

/**
 * Set the source data size for a given column in bytes
 *  @param encoder the encoder
 *  @param col the column (zero indexed)
 *  @param element_size the element size in bytes (must be a multiple of 8) [a value of 0 uses default]
 */
int odc_encoder_column_set_data_size(odc_encoder_t* encoder, int col, int element_size);

/** Associates a custom data layout and data array with the column.
 *  This function is used as an alternative to odc_encoder_set_data_array
 *  @param encoder the encoder
 *  @param col the column (zero indexed)
 *  @param element_size the element size in bytes (must be a multiple of 8) [a value of 0 uses default]
 *  @param stride the separation in memory between consecutive elements in bytes [0 uses default, i.e. contiguous]
 *  @param data a pointer to the first element
 */
int odc_encoder_column_set_data_array(odc_encoder_t* encoder, int col, int element_size, int stride, const void* data);

/** Adds a bitfield to a column */
int odc_encoder_column_add_bitfield(odc_encoder_t* encoder, int col, const char* name, int nbits);


typedef long (*odc_stream_write_t)(void* context, const void* buffer, long length);

/** Encodes ODB2 in a stream handler */
int odc_encode_to_stream(odc_encoder_t* encoder, void* context, odc_stream_write_t write_fn, long* bytes_encoded);

/** Encodes ODB2 into an already open file descriptor */
int odc_encode_to_file_descriptor(odc_encoder_t* encoder, int fd, long* bytes_encoded);

/** Encodes ODB2 into a pre-allocated memory buffer.
    Returns an error if an insufficiently large buffer is supplied */
int odc_encode_to_buffer(odc_encoder_t* encoder, void* buffer, long length, long* bytes_encoded);

///@}

/*--------------------------------------------------------------------------------------------------------------------*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* odc_api_odc_H */
