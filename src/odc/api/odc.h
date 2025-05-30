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

/** @note The API reference of this code is generated automatically, however only existing groups are included in the
 * resultant documentation. In case any new groups that provide the public API are added, they must be manually
 * inserted into an appropriate section. Same goes for removal or renaming of existing groups, otherwise non-propagated
 * changes can fail the documentation build. Please see <c-reference.rst> in this repository for more information.
 */

#ifndef odc_api_odc_H
#define odc_api_odc_H

#ifdef __cplusplus
extern "C" {
#endif

/*--------------------------------------------------------------------------------------------------------------------*/

#include <stdbool.h>

/** \defgroup Initialisation */
/** @{ */

/** Represent integers as doubles in the API (default) */
const int ODC_INTEGERS_AS_DOUBLES = 1;  // this is the default

/** Represent integers as 64-bit integers in the API */
const int ODC_INTEGERS_AS_LONGS = 2;

/** Initialises API, must be called before any other function
 * \note This is only required if being used from a context where **eckit::Main()** is not otherwise initialised.
 * \returns Return code (#OdcErrorValues)
 */
int odc_initialise_api();
/** Sets treatment of integers in the odc API
 * \param integerBehaviour Desired integer behaviour (#ODC_INTEGERS_AS_DOUBLES #ODC_INTEGERS_AS_LONGS)
 * \returns Return code (#OdcErrorValues)
 */
int odc_integer_behaviour(int integerBehaviour);

/** @} */


/** \defgroup Version Accessors */
/** @{ */

/** Retrieves the release version of the library in human-readable format, e.g. ``1.3.0``
 * \param version Return variable for release version. Returned pointer valid throughout program lifetime.
 * \returns Return code (#OdcErrorValues)
 */
int odc_version(const char** version);
/** Retrieves version control checksum of the latest change, e.g. ``a88011c007a0db48a5d16e296934a197eac2050a``
 * \param version Return variable for version control checksum. Returned pointer valid throughout program lifetime.
 * \returns Return code (#OdcErrorValues)
 */
int odc_vcs_version(const char** version);

/** @} */


/** \defgroup Error Handling */
/** @{ */

/** Return codes */
enum OdcErrorValues {
    /** The function completed successfully */
    ODC_SUCCESS = 0,
    /** All frames have been returned, and the loop can be terminated successfully. */
    ODC_ITERATION_COMPLETE = 1,
    /** A known error was encountered. Call ``odc_error_string()`` with the returned code for details. */
    ODC_ERROR_GENERAL_EXCEPTION = 2,
    /** An unexpected and unknown error was encountered. Call ``odc_error_string()`` with the returned code for details.
     */
    ODC_ERROR_UNKNOWN_EXCEPTION = 3
};

/** Returns a human-readable error message for the last error given an error code
 * \param err Error code (#OdcErrorValues)
 * \returns Error message
 */
const char* odc_error_string(int err);

// int odc_abort_on_failure(bool abort); ///< @todo to remove

/** Error handler callback function signature
 * \param context Error handler context
 * \param error_code Error code (#OdcErrorValues)
 */
typedef void (*odc_failure_handler_t)(void* context, int error_code);

/** Sets an error handler which will be called on error with the supplied context and an error code
 * \param handler Error handler function
 * \param context Error handler context
 */
int odc_set_failure_handler(odc_failure_handler_t handler, void* context);

/** @} */


/** \defgroup Values and Types */
/** @{ */

/** Column data types */
enum OdcColumnType {
    /** Specifies that the column is ignored (invalid for real data) */
    ODC_IGNORE = 0,
    /** Specifies the column contains integer data */
    ODC_INTEGER = 1,
    /** Specifies the column contains 32-bit floating point values */
    ODC_REAL = 2,
    /** Specifies the column contains character (string) data */
    ODC_STRING = 3,
    /** Specifies the column contains bitfield data */
    ODC_BITFIELD = 4,
    /** Specifies the column contains 64-bit floating point values */
    ODC_DOUBLE = 5
};

/** Retrieves number of supported column data types
 * \param count Return variable for number of data types
 * \returns Return code (#OdcErrorValues)
 */
int odc_column_type_count(int* count);

/** Retrieves a human-readable name of a column data type
 * \param type Column data type (#OdcColumnType)
 * \param type_name Return variable for column data type name. Returned pointer valid throughout program lifetime.
 * \returns Return code (#OdcErrorValues)
 */
int odc_column_type_name(int type, const char** type_name);

/// @todo In the top CMakelists.txt assert that in this system C long is 64 bit
/// @todo In the top CMakelists.txt assert that in this system C double is 64 bit


/** Sets the value that identifies a missing integer in the API
 * \param missing_integer Missing integer value
 * \returns Return code (#OdcErrorValues)
 */
int odc_set_missing_integer(long missing_integer);

/** Sets the value that identifies a missing double in the API
 * \param missing_double Missing double value
 * \returns Return code (#OdcErrorValues)
 */
int odc_set_missing_double(double missing_double);

/** Retrieves the value that identifies a missing integer in the API
 * \param missing_value Return variable for missing integer value
 * \returns Return code (#OdcErrorValues)
 */
int odc_missing_integer(long* missing_value);
/** Retrieves the value that identifies a missing double in the API
 * \param missing_value Return variable for missing double value
 * \returns Return code (#OdcErrorValues)
 */
int odc_missing_double(double* missing_value);

/** @} */


/** \defgroup Reader */
/** @{ */

struct odc_reader_t;
/** Opaque type for the Reader object. Controls the ODB-2 data stream and associated resources, and gives access to the
 * underlying frames. */
typedef struct odc_reader_t odc_reader_t;
/** Creates a reader and opens the specified file path
 * \param reader Reader instance. Returned instance must be freed using #odc_close.
 * \param filename File path to open
 * \returns Return code (#OdcErrorValues)
 */
int odc_open_path(odc_reader_t** reader, const char* filename);

/** Creates a reader from an already open file descriptor.
 *
 * The file descriptor will be duplicated so the calling code is safe to close the file descriptor.
 *
 * \param reader Reader instance. Returned instance must be freed using #odc_close.
 * \param fd File descriptor
 * \returns Return code (#OdcErrorValues)
 */
int odc_open_file_descriptor(odc_reader_t** reader, int fd);

/** Creates a reader from a memory buffer
 * \param reader Reader instance. Returned instance must be freed using #odc_close.
 * \param data Memory buffer
 * \param length Length of memory buffer
 * \returns Return code (#OdcErrorValues)
 */
int odc_open_buffer(odc_reader_t** reader, const void* data, long length);

/** Reader stream handler callback function signature. Functions analogously to the POSIX read() function.
 * \param context Stream handler context
 * \param buffer Memory buffer to handle
 * \param buffer Size of the memory buffer
 */
typedef long (*odc_stream_read_t)(void* context, void* buffer, long length);

/** Creates a reader associated to a custom stream handler. The callback specified will be called in the same way as the
 * POSIX read() function to obtain data from the custom stream.
 * \param reader Reader instance. Returned instance must be freed using #odc_close.
 * \param context Stream handler context
 * \param stream_proc Stream handler function
 * \returns Return code (#OdcErrorValues)
 */
int odc_open_stream(odc_reader_t** reader, void* context, odc_stream_read_t stream_proc);

/** Closes opened resource and destroys the reader
 * \param reader Reader instance
 * \returns Return code (#OdcErrorValues)
 */
int odc_close(const odc_reader_t* reader);

/** @} */


/** \defgroup Frame */
/** @{ */

struct odc_frame_t;
/** Opaque type for the Frame object. Provides a viewport onto a chunk of contiguous data within the ODB-2 stream. */
typedef struct odc_frame_t odc_frame_t;

/** Creates a frame instance associated with a specific reader instance, for interrogating ODB-2 data
 * \param frame Frame instance. Returned instance must be freed using #odc_free_frame.
 * \param reader Reader instance
 * \returns Return code (#OdcErrorValues)
 */
int odc_new_frame(odc_frame_t** frame, odc_reader_t* reader);

/** Deallocates frame object and associated resources.
 * \param frame Frame instance
 * \returns Return code (#OdcErrorValues)
 */
int odc_free_frame(const odc_frame_t* frame);

/** Advances the viewport to the next frame in the stream
 * \param frame Frame instance
 * \returns Return code (#OdcErrorValues)
 */
int odc_next_frame(odc_frame_t* frame);

/** Advances the viewport to the next logical frame in the stream
 * \param frame Frame instance
 * \param maximum_rows Maximum number of rows to aggregate into one logical frame
 * \returns Return code (#OdcErrorValues)
 */
int odc_next_frame_aggregated(odc_frame_t* frame, long maximum_rows);

/** Creates an independent copy of the frame object, resulting in an independent viewport on the ODB data stream with
 * its own associated resources. To use the new copied frame independently from the first requires the ODB-2 data stream
 * to be seekable.
 * \param source_frame Source frame instance to copy from
 * \param copy Target frame instance to copy to. Returned instance must be freed using #odc_free_frame.
 * \returns Return code (#OdcErrorValues)
 */
int odc_copy_frame(odc_frame_t* source_frame, odc_frame_t** copy);

/** Retrieves number of rows in the frame
 * \param frame Frame instance
 * \param count Return variable for number of rows
 * \returns Return code (#OdcErrorValues)
 */
int odc_frame_row_count(const odc_frame_t* frame, long* count);

/** Retrieves number of columns in the frame
 * \param frame Frame instance
 * \param count Return variable for number of columns
 * \returns Return code (#OdcErrorValues)
 */
int odc_frame_column_count(const odc_frame_t* frame, int* count);

/** Retrieves column attributes from current frame
 * \param frame Frame instance
 * \param col Target column index
 * \param name (*optional*) Return variable for column name. Returned pointer valid until frame object destroyed or
 * advanced to the next frame.
 * \param type (*optional*) Return variable for column type
 * \param element_size (*optional*) Return variable for column size in bytes
 * \param bitfield_count (*optional*) Return variable for number of column bitfields
 * \returns Return code (#OdcErrorValues)
 */
int odc_frame_column_attributes(const odc_frame_t* frame, int col, const char** name, int* type, int* element_size,
                                int* bitfield_count);

/** Retrieves bitfield attributes of a column
 * \param frame Frame instance
 * \param col Target column index
 * \param entry Target bitfield index
 * \param name (*optional*) Return variable for bitfield name. Returned pointer valid until frame object destroyed or
 * advanced to the next frame.
 * \param offset (*optional*) Return variable for bitfield offset
 * \param size (*optional*) Return variable for bitfield size in bits
 * \returns Return code (#OdcErrorValues)
 */
int odc_frame_bitfield_attributes(const odc_frame_t* frame, int col, int entry, const char** name, int* offset,
                                  int* size);

/** Retrieves the number of properties encoded in the frame
 * \param frame Frame instance
 * \param nproperties Return variable for number of properties
 * \returns Return code (#OdcErrorValues)
 */
int odc_frame_properties_count(const odc_frame_t* frame, int* nproperties);

/** Retrieves the property key and value by its index
 * \param frame Frame instance
 * \param idx Property index
 * \param key Return variable for property key. Returned pointer valid until frame object destroyed or advanced to the
 * next frame.
 * \param value Return variable for property value. Returned pointer valid until frame object destroyed or advanced to
 * the next frame.
 * \returns Return code (#OdcErrorValues)
 */
int odc_frame_property_idx(const odc_frame_t* frame, int idx, const char** key, const char** value);

/** Retrieves the property value by its key
 * \param frame Frame instance
 * \param key Property key
 * \param value Return variable for property value. Returned pointer valid until frame object destroyed or advanced to
 * the next frame.
 * \returns Return code (#OdcErrorValues)
 */
int odc_frame_property(const odc_frame_t* frame, const char* key, const char** value);

/** @} */


/** \defgroup Decoder */
/** @{ */

struct odc_decoder_t;
/** Opaque type for the Decoder object. Specifies which ODB-2 columns should be decoded and the memory that should be
 * used for the decoded data. */
typedef struct odc_decoder_t odc_decoder_t;

/* A decode target may allocate its own buffer to decode into, or use one that is
 * externally supplied */

/** Creates a decoder instance for decoding ODB-2 format
 * \param decoder Decoder instance. Returned instance must be freed using #odc_free_decoder.
 * \returns Return code (#OdcErrorValues)
 */
int odc_new_decoder(odc_decoder_t** decoder);

/** Deallocates memory used up by a decoder
 * \param decoder Decoder instance
 * \returns Return code (#OdcErrorValues)
 */
int odc_free_decoder(const odc_decoder_t* decoder);

/** Configures a decoder to decode all data contained in the supplied frame
 * \param decoder Decoder instance
 * \param frame Frame instance
 * \returns Return code (#OdcErrorValues)
 */
int odc_decoder_defaults_from_frame(odc_decoder_t* decoder, const odc_frame_t* frame);

/** Instructs the decoder whether to output in column-major form
 * \param decoder Decoder instance
 * \param columnMajor Whether to output in column-major form
 * \returns Return code (#OdcErrorValues)
 */
int odc_decoder_set_column_major(odc_decoder_t* decoder, bool columnMajor);

/** Sets number of rows to allocate in a decoder
 * \param decoder Decoder instance
 * \param nrows Number of rows to allocate
 * \returns Return code (#OdcErrorValues)
 */
int odc_decoder_set_row_count(odc_decoder_t* decoder, long nrows);

/** Retrieves number of rows that are allocated in a decoder
 * \param decoder Decoder instance
 * \param nrows Return variable for number of rows
 * \returns Return code (#OdcErrorValues)
 */
int odc_decoder_row_count(const odc_decoder_t* decoder, long* nrows);

/**
 * Sets an output data array into which the data may be decoded
 * \param decoder Decoder instance
 * \param data Data array to decode into
 * \param width Width of data array in bytes
 * \param height Height of data array in rows
 * \param columnMajor Whether the column-major memory layout is used
 * \returns Return code (#OdcErrorValues)
 */
int odc_decoder_set_data_array(odc_decoder_t* decoder, void* data, long width, long height, bool columnMajor);

/** Retrieves the output data array into which the data may be decoded
 * \param decoder Decoder instance
 * \param data (*optional*) Data array to decode into. Returned pointer valid until decoder object destroyed.
 * \param width (*optional*) Width of data array in bytes
 * \param height (*optional*) Height of data array in rows
 * \param columnMajor (*optional*) Whether the column-major memory layout is used
 * \returns Return code (#OdcErrorValues)
 */
int odc_decoder_data_array(const odc_decoder_t* decoder, const void** data, long* width, long* height,
                           bool* columnMajor);

/** Adds a data column to a decoder
 * \param decoder Decoder instance
 * \param name Data column name
 * \returns Return code (#OdcErrorValues)
 */
int odc_decoder_add_column(odc_decoder_t* decoder, const char* name);

/** Retrieves number of columns that are allocated in a decoder
 * \param decoder Decoder instance
 * \param count Return variable for number of columns
 * \returns Return code (#OdcErrorValues)
 */
int odc_decoder_column_count(const odc_decoder_t* decoder, int* count);

/** Sets the decoded data size for a column in bytes
 * \param decoder Decoder instance
 * \param col Column index
 * \param element_size Column data size in bytes
 * \returns Return code (#OdcErrorValues)
 */
int odc_decoder_column_set_data_size(odc_decoder_t* decoder, int col, int element_size);

/**
 * Sets an output data array into which the data associated with the column can be decoded
 * \param decoder Decoder instance
 * \param col Column index
 * \param element_size Column data size in bytes
 * \param stride Column data width in bytes
 * \param data Column data array
 * \returns Return code (#OdcErrorValues)
 */
int odc_decoder_column_set_data_array(odc_decoder_t* decoder, int col, int element_size, int stride, void* data);

/**
 * Retrieves the buffer and data layout into which the data has been decoded
 * \param decoder Decoder instance
 * \param col Column index
 * \param element_size (*optional*) Return variable for column data size in bytes
 * \param stride (*optional*) Return variable for column data width in bytes
 * \param data (*optional*) Return variable for column data array. Returned pointer valid until decoder object
 * destroyed.
 * \returns Return code (#OdcErrorValues)
 */
int odc_decoder_column_data_array(const odc_decoder_t* decoder, int col, int* element_size, int* stride,
                                  const void** data);

/**
 * Decodes the data described by the frame into the configured data array(s)
 * \param decoder Decoder instance
 * \param frame Frame instance
 * \param rows_decoded (*optional*) Return variable for number of decoded rows
 * \returns Return code (#OdcErrorValues)
 */
int odc_decode(odc_decoder_t* decoder, const odc_frame_t* frame, long* rows_decoded);

/**
 * Decodes the data described by the frame into the configured data array(s).
 *
 * If the frame is a logical one, parallelise the decoding over multiple threads.
 *
 * \param decoder Decoder instance
 * \param frame Frame instance
 * \param rows_decoded (*optional*) Return variable for number of decoded rows
 * \param nthreads Number of threads
 * \returns Return code (#OdcErrorValues)
 */
int odc_decode_threaded(odc_decoder_t* decoder, const odc_frame_t* frame, long* rows_decoded, int nthreads);

/** @} */


/** \defgroup Encoder */
/** @{ */

struct odc_encoder_t;
/** Opaque type for the Encoder object. Describes the column properties and the data layout of the source data for
 * encoding into ODB-2 frames. **/
typedef struct odc_encoder_t odc_encoder_t;

/** Creates an encoder instance for encoding into ODB-2 format
 * \param encoder Encoder instance. Returned instance must be freed using #odc_free_encoder.
 * \returns Return code (#OdcErrorValues)
 */
int odc_new_encoder(odc_encoder_t** encoder);

/** Deallocates memory used up by an encoder
 * \param encoder Encoder instance
 * \returns Return code (#OdcErrorValues)
 */
int odc_free_encoder(const odc_encoder_t* encoder);

/** Adds a key/value property to encode as part of the frame
 * \param encoder Encoder instance
 * \param key Property key
 * \param value Property value
 * \returns Return code (#OdcErrorValues)
 */
int odc_encoder_add_property(odc_encoder_t* encoder, const char* key, const char* value);

/** Sets number of rows to allocate in an encoder
 * \param encoder Encoder instance
 * \param nrows Number of rows
 * \returns Return code (#OdcErrorValues)
 */
int odc_encoder_set_row_count(odc_encoder_t* encoder, long nrows);

/** Sets number of rows to encode per frame
 * \param encoder Encoder instance
 * \param rows_per_frame Number of rows per frame
 * \returns Return code (#OdcErrorValues)
 */
int odc_encoder_set_rows_per_frame(odc_encoder_t* encoder, long rows_per_frame);

/** Sets input data array from which data may be encoded
 * \param encoder Encoder instance
 * \param data Data array to encode
 * \param width Width of the data array in bytes
 * \param height Height of the data array in rows
 * \param columnMajorWidth Column size in bytes for column-major layout, if zero interpret as row-major layout
 * \returns Return code (#OdcErrorValues)
 */
int odc_encoder_set_data_array(odc_encoder_t* encoder, const void* data, long width, long height, int columnMajorWidth);

/// @todo implement the int columnMajorWidth in the above function

/** Adds a data column to current encoder
 * \param encoder Encoder instance
 * \param name Column name
 * \param type Column data type (#OdcColumnType)
 * \returns Return code (#OdcErrorValues)
 */
int odc_encoder_add_column(odc_encoder_t* encoder, const char* name, int type);

/**
 * Sets the source data size for a column
 * \param encoder Encoder instance
 * \param col Column index
 * \param element_size Column data size in bytes
 * \returns Return code (#OdcErrorValues)
 */
int odc_encoder_column_set_data_size(odc_encoder_t* encoder, int col, int element_size);

/** Sets a custom data layout and data array for a column
 * \param encoder Encoder instance
 * \param col Column index
 * \param element_size Column size in bytes
 * \param stride Column width in bytes
 * \param data Column data array
 * \returns Return code (#OdcErrorValues)
 */
int odc_encoder_column_set_data_array(odc_encoder_t* encoder, int col, int element_size, int stride, const void* data);

/** Adds a bitfield to a column
 * \param encoder Encoder instance
 * \param col Column index
 * \param name Bitfield name
 * \param nbits Bitfield size in bits
 * \returns Return code (#OdcErrorValues)
 */
int odc_encoder_column_add_bitfield(odc_encoder_t* encoder, int col, const char* name, int nbits);

/** Encoder stream handler function signature
 * \param context Stream handler context
 * \param buffer Memory buffer to handle
 * \param length Size of memory buffer in bytes
 */
typedef long (*odc_stream_write_t)(void* context, const void* buffer, long length);

/** Encodes ODB-2 into a stream handler
 * \param encoder Encoder instance
 * \param context Stream handler context
 * \param write_fn Stream handler function
 * \param bytes_encoded Return variable for number of encoded bytes
 * \returns Return code (#OdcErrorValues)
 */
int odc_encode_to_stream(odc_encoder_t* encoder, void* context, odc_stream_write_t write_fn, long* bytes_encoded);

/** Encodes ODB-2 into an already open file descriptor
 * \param encoder Encoder instance
 * \param fd File descriptor
 * \param bytes_encoded (*optional*) Return variable for number of encoded bytes
 * \returns Return code (#OdcErrorValues)
 */
int odc_encode_to_file_descriptor(odc_encoder_t* encoder, int fd, long* bytes_encoded);

/** Encodes ODB-2 into a pre-allocated memory buffer
 * \param encoder Encoder instance
 * \param buffer Memory buffer
 * \param length Buffer size
 * \param bytes_encoded (*optional*) Return variable for number of encoded bytes
 * \returns Return code (#OdcErrorValues)
 */
int odc_encode_to_buffer(odc_encoder_t* encoder, void* buffer, long length, long* bytes_encoded);

/** @} */

/*--------------------------------------------------------------------------------------------------------------------*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* odc_api_odc_H */
