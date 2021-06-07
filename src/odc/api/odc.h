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

/** Defines integer behaviour as double data type (default) */
const int ODC_INTEGERS_AS_DOUBLES = 1; // this is the default
/** Defines integer behaviour as long data type */
const int ODC_INTEGERS_AS_LONGS = 2;

/** Initialises API, must be called before any other function
 * \note This is only required if being used from a context where **eckit::Main()** is not otherwise initialised.
 * \returns Return code (#OdcErrorValues)
 */
int odc_initialise_api();
/** Sets treatment of integers in ODB-2 data
 * \param integerBehaviour Desired integer behaviour (#ODC_INTEGERS_AS_DOUBLES #ODC_INTEGERS_AS_LONGS)
 * \returns Return code (#OdcErrorValues)
 */
int odc_integer_behaviour(int integerBehaviour);

/** @} */


/** \defgroup Version Accessors */
/** @{ */

/** Retrieves release version of the library in human-readable format, e.g. ``1.3.0``
 * \param version Return variable for release version
 * \returns Return code (#OdcErrorValues)
 */
int odc_version(const char** version);
/** Retrieves version control checksum of the latest change, e.g. ``a88011c007a0db48a5d16e296934a197eac2050a``
 * \param version Return variable for version control checksum
 * \returns Return code (#OdcErrorValues)
 */
int odc_vcs_version(const char** version);

/** @} */


/** \defgroup Error Handling */
/** @{ */

/** Return codes */
enum OdcErrorValues {
    /** Defines return code for success */
    ODC_SUCCESS                  = 0,
    /** Defines return code for successful end of iteration */
    ODC_ITERATION_COMPLETE       = 1,
    /** Defines return code for a general error */
    ODC_ERROR_GENERAL_EXCEPTION  = 2,
    /** Defines return code for an unknown error */
    ODC_ERROR_UNKNOWN_EXCEPTION  = 3
};

/** Returns a human-readable error message for an error code
 * \param err Error code (#OdcErrorValues)
 * \returns Error message
 */
const char* odc_error_string(int err);

// int odc_abort_on_failure(bool abort); ///< @todo to remove

/** Error handler function signature
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
    /** Defines the encoded data type for an ignored column */
    ODC_IGNORE   = 0,
    /** Defines the encoded data type for an integer column */
    ODC_INTEGER  = 1,
    /** Defines the encoded data type for a real column */
    ODC_REAL     = 2,
    /** Defines the encoded data type for a string column */
    ODC_STRING   = 3,
    /** Defines the encoded data type for a bitfield column */
    ODC_BITFIELD = 4,
    /** Defines the encoded data type for a double column */
    ODC_DOUBLE   = 5
};

/** Retrieves number of supported column data types
 * \param count Return variable for number of data types
 * \returns Return code (#OdcErrorValues)
 */
int odc_column_type_count(int* count);

/** Retrieves a human-readable name of a column data type
 * \param type Column data type (#OdcColumnType)
 * \param type_name Return variable for column data type name
 * \returns Return code (#OdcErrorValues)
 */
int odc_column_type_name(int type, const char** type_name);

/// @todo In the top CMakelists.txt assert that in this system C long is 64 bit
/// @todo In the top CMakelists.txt assert that in this system C double is 64 bit


/** Sets the value that identifies a missing integer
 * \param missing_integer Missing integer value
 * \returns Return code (#OdcErrorValues)
 */
int odc_set_missing_integer(long missing_integer);

/** Sets the value that identifies a missing double
 * \param missing_double Missing double value
 * \returns Return code (#OdcErrorValues)
 */
int odc_set_missing_double(double missing_double);

/** Retrieves the value that identifies a missing integer
 * \param missing_value Return variable for missing integer value
 * \returns Return code (#OdcErrorValues)
 */
int odc_missing_integer(long* missing_value);
/** Retrieves the value that identifies a missing double
 * \param missing_value Return variable for missing double value
 * \returns Return code (#OdcErrorValues)
 */
int odc_missing_double(double* missing_value);

/** @} */


/** \defgroup Reader */
/** @{ */

struct odc_reader_t;
/** Controls the ODB-2 file resources, and gives access to the underlying frames */
typedef struct odc_reader_t odc_reader_t;
/** Creates a reader and opens the specified file path
 * \param reader Reader instance
 * \param filename File path to open
 * \returns Return code (#OdcErrorValues)
 */
int odc_open_path(odc_reader_t** reader, const char* filename);

/** Creates a reader from an already open file descriptor.
 *
 * It will duplicate the file descriptor so the calling code is safe to close the file descriptor.
 *
 * \param reader Reader instance
 * \param fd File descriptor
 * \returns Return code (#OdcErrorValues)
 */
int odc_open_file_descriptor(odc_reader_t** reader, int fd);

/** Creates a reader from a memory buffer
 * \param reader Reader instance
 * \param data Memory buffer
 * \param length Length of memory buffer
 * \returns Return code (#OdcErrorValues)
 */
int odc_open_buffer(odc_reader_t** reader, const void* data, long length);

/** Reader stream handler function signature
 * \param context Stream handler context
 * \param buffer Memory buffer to handle
 * \param buffer Size of the memory buffer
 */
typedef long (*odc_stream_read_t)(void* context, void* buffer, long length);

/** Creates a reader associated to a stream handler
 * \param reader Reader instance
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
/** Provides a viewport into a chunk of contiguous data within the ODB-2 stream */
typedef struct odc_frame_t odc_frame_t;

/** Creates a frame instance for interrogating ODB-2 data
 * \param frame Frame instance
 * \param reader Reader instance
 * \returns Return code (#OdcErrorValues)
 */
int odc_new_frame(odc_frame_t** frame, odc_reader_t* reader);

/** Deallocates memory used up by a frame
 * \param frame Frame instance
 * \returns Return code (#OdcErrorValues)
 */
int odc_free_frame(const odc_frame_t* frame);

/** Advances to the next frame in the stream
 * \param frame Frame instance
 * \returns Return code (#OdcErrorValues)
 */
int odc_next_frame(odc_frame_t* frame);

/** Advances to the next logical frame in the stream
 * \param frame Frame instance
 * \param maximum_rows Maximum number of aggregated rows
 * \returns Return code (#OdcErrorValues)
 */
int odc_next_frame_aggregated(odc_frame_t* frame, long maximum_rows);

/** Copies the frame to another frame
 * \param source_frame Source frame instance to copy from
 * \param copy Target frame instance to copy to
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

/** Retrieves column attributes in current frame
 * \param frame Frame instance
 * \param col Target column index
 * \param name Return variable for column name
 * \param type Return variable for column type
 * \param element_size Return variable for column size in bytes
 * \param bitfield_count Return variable for number of column bitfields
 * \returns Return code (#OdcErrorValues)
 */
int odc_frame_column_attributes(const odc_frame_t* frame, int col, const char** name, int* type, int* element_size, int* bitfield_count);

/** Retrieves bitfield attributes of a column
 * \param frame Frame instance
 * \param col Target column index
 * \param entry Target bitfield index
 * \param name Return variable for bitfield name
 * \param offset Return variable for bitfield offset
 * \param size Return variable for bitfield size in bits
 * \returns Return code (#OdcErrorValues)
 */
int odc_frame_bitfield_attributes(const odc_frame_t* frame, int col, int entry, const char** name, int* offset, int* size);

/** Retrieves the number of properties encoded in the frame
 * \param frame Frame instance
 * \param nproperties Return variable for number of properties
 * \returns Return code (#OdcErrorValues)
 */
int odc_frame_properties_count(const odc_frame_t* frame, int* nproperties);

/** Retrieves the property key and value by its index
 * \param frame Frame instance
 * \param idx Property index
 * \param key Return variable for property key
 * \param value Return variable for property value
 * \returns Return code (#OdcErrorValues)
 */
int odc_frame_property_idx(const odc_frame_t* frame, int idx, const char** key, const char** value);

/** Retrieves the property value by its key
 * \param frame Frame instance
 * \param key Property key
 * \param value Return variable for property value
 * \returns Return code (#OdcErrorValues)
 */
int odc_frame_property(const odc_frame_t* frame, const char* key, const char** value);

/** @} */



/** \defgroup Decoder */
/** @{ */

struct odc_decoder_t;
/** Specifies which ODB-2 columns should be decoded and the memory that the decoded data should be put into */
typedef struct odc_decoder_t odc_decoder_t;

/* A decode target may allocate its own buffer to decode into, or use one that is
 * externally supplied */

/** Creates a decoder instance for decoding ODB-2 format
 * \param decoder Decoder instance
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
 * \param data Data array to decode into
 * \param width Width of data array in bytes
 * \param height Height of data array in rows
 * \param columnMajor Whether the column-major memory layout is used
 * \returns Return code (#OdcErrorValues)
 */
int odc_decoder_data_array(const odc_decoder_t* decoder, const void** data, long* width, long* height, bool* columnMajor);

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
 * \param element_size Return variable for column data size in bytes
 * \param stride Return variable for column data width in bytes
 * \param data Return variable for column data array
 * \returns Return code (#OdcErrorValues)
 */
int odc_decoder_column_data_array(const odc_decoder_t* decoder, int col, int* element_size, int* stride, const void** data);

/**
 * Decodes the data described by the frame into the configured data array(s)
 * \param decoder Decoder instance
 * \param frame Frame instance
 * \param rows_decoded Return variable for number of decoded rows
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
 * \param rows_decoded Return variable for number of decoded rows
 * \param nthreads Number of threads
 * \returns Return code (#OdcErrorValues)
 */
int odc_decode_threaded(odc_decoder_t* decoder, const odc_frame_t* frame, long* rows_decoded, int nthreads);

/** @} */



/** \defgroup Encoder */
/** @{ */

struct odc_encoder_t;
/** Describes data in memory and encodes it into ODB-2 frames **/
typedef struct odc_encoder_t odc_encoder_t;

/** Creates an encoder instance for encoding into ODB-2 format
 * \param encoder Encoder instance
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
 * \param bytes_encoded Return variable for number of encoded bytes
 * \returns Return code (#OdcErrorValues)
 */
int odc_encode_to_file_descriptor(odc_encoder_t* encoder, int fd, long* bytes_encoded);

/** Encodes ODB-2 into a pre-allocated memory buffer
 * \param encoder Encoder instance
 * \param buffer Memory buffer
 * \param length Buffer size
 * \param bytes_encoded Return variable for number of encoded bytes
 * \returns Return code (#OdcErrorValues)
 */
int odc_encode_to_buffer(odc_encoder_t* encoder, void* buffer, long length, long* bytes_encoded);

/** @} */

/*--------------------------------------------------------------------------------------------------------------------*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* odc_api_odc_H */
