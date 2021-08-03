.. index:: Reference; Fortran API
   :name: f90-reference

Fortran Reference
=================

Types
-----

.. f:type:: odc_reader

   Controls the ODB-2 data stream and associated resources, and gives access to the underlying frames

   :f open_path(path): :f:func:`🔗 <reader_open_path>`
   :f close(): :f:func:`🔗 <reader_close>`


.. f:type:: odc_frame

   Provides a viewport into a chunk of contiguous data within the ODB-2 stream

   :f initialise(reader): :f:func:`🔗 <frame_initialise>`
   :f free(): :f:func:`🔗 <frame_free>`
   :f copy(new_frame): :f:func:`🔗 <frame_copy>`
   :f next([aggregated, maximum_rows]): :f:func:`🔗 <frame_next>`
   :f row_count(nrows): :f:func:`🔗 <frame_row_count>`
   :f column_count(ncols): :f:func:`🔗 <frame_column_count>`
   :f column_attributes(col[, name, type, element_size, element_size_doubles, bitfield_count]): :f:func:`🔗 <frame_column_attributes>`
   :f bitfield_attributes(col, field[, name, offset, size]): :f:func:`🔗 <frame_bitfield_attributes>`
   :f properties_count(nproperties): :f:func:`🔗 <frame_properties_count>`
   :f property_idx(idx, key, val): :f:func:`🔗 <frame_property_idx>`
   :f property(key[, val, exists]): :f:func:`🔗 <frame_property>`


.. f:type:: odc_decoder

   Specifies which ODB-2 columns should be decoded and the memory that the decoded data should be put into

   :f initialise([column_major]): :f:func:`🔗 <decoder_initialise>`
   :f free(): :f:func:`🔗 <decoder_free>`
   :f defaults_from_frame(frame): :f:func:`🔗 <decoder_defaults_from_frame>`
   :f set_row_count(count): :f:func:`🔗 <decoder_set_row_count>`
   :f row_count(count): :f:func:`🔗 <decoder_row_count>`
   :f set_data(data[, column_major]): :f:func:`🔗 <decoder_set_data_array>`
   :f data([data, column_major]): :f:func:`🔗 <decoder_data_array>`
   :f add_column(name): :f:func:`🔗 <decoder_add_column>`
   :f column_count(count): :f:func:`🔗 <decoder_column_count>`
   :f column_set_data_size(col, element_size): :f:func:`🔗 <decoder_column_set_data_size>`
   :f column_set_data_array(col[, element_size, stride, data]): :f:func:`🔗 <decoder_column_set_data_array>`
   :f column_data_array(col[, element_size, element_size_doubles, stride, data]): :f:func:`🔗 <decoder_column_data_array>`
   :f decode(frame, rows_decoded[, nthreads]): :f:func:`🔗 <decoder_decode>`


.. f:type:: odc_encoder

   Describes data in memory and encodes it into ODB-2 frames

   :f logical column_major: Whether the column-major memory layout is used
   :f initialise(): :f:func:`🔗 <encoder_initialise>`
   :f free(): :f:func:`🔗 <encoder_free>`
   :f set_row_count(row_count): :f:func:`🔗 <encoder_set_row_count>`
   :f set_rows_per_frame(rows_per_frame): :f:func:`🔗 <encoder_set_rows_per_frame>`
   :f set_data(data[, column_major]): :f:func:`🔗 <encoder_set_data_array>`
   :f add_column(name, type): :f:func:`🔗 <encoder_add_column>`
   :f add_property(key, val): :f:func:`🔗 <encoder_add_property>`
   :f column_set_data_size(col[, element_size, element_size_doubles]): :f:func:`🔗 <encoder_column_set_data_size>`
   :f column_set_data_array(col[, element_size, element_size_doubles, stride, data]): :f:func:`🔗 <encoder_column_set_data_array>`
   :f column_add_bitfield(col, name, nbits): :f:func:`🔗 <encoder_column_add_bitfield>`
   :f encode(outunit, bytes_written): :f:func:`🔗 <encoder_encode>`


Constants
---------

.. _`f-column-data-types`:

Column Data Types
~~~~~~~~~~~~~~~~~

.. f:variable:: ODC_IGNORE
   :type: integer(c_int)
   :attrs: parameter=0

   Specifies that the column is ignored (invalid for real data)


.. f:variable:: ODC_INTEGER
   :type: integer(c_int)
   :attrs: parameter=1

   Specifies the column contains integer data


.. f:variable:: ODC_REAL
   :type: integer(c_int)
   :attrs: parameter=2

   Specifies the column contains 32-bit floating point values


.. f:variable:: ODC_STRING
   :type: integer(c_int)
   :attrs: parameter=3

   Specifies the column contains character (string) data


.. f:variable:: ODC_BITFIELD
   :type: integer(c_int)
   :attrs: parameter=4

   Specifies the column contains bitfield data


.. f:variable:: ODC_DOUBLE
   :type: integer(c_int)
   :attrs: parameter=5

   Specifies the column contains 64-bit floating point values


.. _`f-return-codes`:

Return Codes
~~~~~~~~~~~~

.. f:variable:: ODC_SUCCESS
   :type: integer
   :attrs: parameter=0

   The function completed successfully


.. f:variable:: ODC_ITERATION_COMPLETE
   :type: integer
   :attrs: parameter=1

   All frames have been returned, and the loop can be terminated successfully.


.. f:variable:: ODC_ERROR_GENERAL_EXCEPTION
   :type: integer
   :attrs: parameter=2

   A known error was encountered. Call ``odc_error_string()`` with the returned code for details.

.. f:variable:: ODC_ERROR_UNKNOWN_EXCEPTION
   :type: integer
   :attrs: parameter=3

   An unexpected and unknown error was encountered. Call ``odc_error_string()`` with the returned code for details.


.. _`f-integer-behaviour`:

Integer Behaviour
~~~~~~~~~~~~~~~~~

.. f:variable:: ODC_INTEGERS_AS_DOUBLES
   :type: integer
   :attrs: parameter=1

   Represent integers as doubles in the API (default)


.. f:variable:: ODC_INTEGERS_AS_LONGS
   :type: integer
   :attrs: parameter=2

   Represent integers as 64-bit integers in the API


Module Functions
----------------

.. f:function:: odc_initialise_api

   Initialises API, must be called before any other function

   .. note::

      This is only required if being used from a context where **eckit::Main()** is not otherwise initialised.

   :r integer err: Return code :ref:`🔗 <f-return-codes>`


.. f:function:: odc_version(version_str)

   Retrieves the release version of the library in human-readable format, e.g. ``1.3.0``

   :p character(:) version_str [out,allocatable]: Return variable for version number
   :r integer err: Return code :ref:`🔗 <f-return-codes>`


.. f:function:: odc_vcs_version(git_sha1)

   Retrieves version control checksum of the latest change, e.g. ``a88011c007a0db48a5d16e296934a197eac2050a``

   :p character(:) git_sha1 [out,allocatable]: Return variable for version control checksum
   :r integer err: Return code :ref:`🔗 <f-return-codes>`


.. f:function:: odc_column_type_name(type, type_name)

   Retrieves a human-readable name of a column data type

   :p integer(c_int) type [in]: Column data type :ref:`🔗 <f-column-data-types>`
   :p character(:) type_name [out,allocatable]: Return variable for column data type name
   :r integer err: Return code :ref:`🔗 <f-return-codes>`


.. f:function:: odc_column_type_count(ntypes)

   Retrieves number of supported column data types

   :p integer(c_int) ntypes [out]: Return variable for number of data types
   :r integer err: Return code :ref:`🔗 <f-return-codes>`


.. f:function:: odc_error_string(err)

   Returns a human-readable error message for an error code

   :p integer err [in]: Error code :ref:`🔗 <f-return-codes>`
   :r character(:) error_string [target,allocatable]: Error message


.. f:function:: odc_missing_integer(missing_integer)

   Retrieves the value that identifies a missing integer in the API

   :p integer(c_long) missing_integer [out]: Return variable for missing integer value
   :r integer err: Return code :ref:`🔗 <f-return-codes>`


.. f:function:: odc_missing_double(missing_double)

   Retrieves the value that identifies a missing double in the API

   :p real(c_double) missing_double [out]: Return variable for missing double value
   :r integer err: Return code :ref:`🔗 <f-return-codes>`


.. f:function:: odc_set_missing_integer(missing_integer)

   Sets the value that identifies a missing integer in the API

   :p integer(c_long) missing_integer [in,value]: Missing integer value
   :r integer err: Return code :ref:`🔗 <f-return-codes>`


.. f:function:: odc_missing_double(missing_double)

   Sets the value that identifies a missing double in the API

   :p real(c_double) missing_double [in,value]: Missing double value
   :r integer err: Return code :ref:`🔗 <f-return-codes>`


.. f:function:: odc_set_failure_handler(handler, context)

   Sets an error handler which will be called on error with the supplied context and an error code

   :p procedure(failure_handler_t) handler [pointer]: Error handler function
   :p integer(c_long) context: Error handler context
   :r integer err: Return code :ref:`🔗 <f-return-codes>`


.. f:function:: odc_integer_behaviour(integer_behaviour)

   Sets treatment of integers in the API

   :p integer(c_int) integer_behaviour [in,value]: Desired integer behaviour :ref:`🔗 <f-integer-behaviour>`
   :r integer err: Return code :ref:`🔗 <f-return-codes>`


Type Methods
------------

.. f:function:: reader_open_path(path)

   Initialise the reader to read the ODB-2 data stream in the specified path.

   :p character(:) path [in]: File path to open
   :r integer err: Return code :ref:`🔗 <f-return-codes>`


.. f:function:: reader_close

   Closes opened reader

   :r integer err: Return code :ref:`🔗 <f-return-codes>`


.. f:function:: frame_initialise(reader)

   Initialises current frame associated with the specified reader

   :p odc_reader reader [inout]: Reader instance
   :r integer err: Return code :ref:`🔗 <f-return-codes>`


.. f:function:: frame_free()

   Deallocates memory used up by the current frame

   :r integer err: Return code :ref:`🔗 <f-return-codes>`


.. f:function:: frame_copy(new_frame)

   Copies current frame to another frame

   :p odc_frame new_frame: Target frame instance to copy to
   :r integer err: Return code :ref:`🔗 <f-return-codes>`


.. f:function:: frame_next([aggregated, maximum_rows])

   Advances to the next frame in the stream

   :o logical aggregated [in,default=.false.]: Whether to aggregate compatible data into a logical frame
   :o integer(c_long) maximum_rows [in]: Maximum number of aggregated rows, will turn on aggregation if supplied
   :r integer err: Return code :ref:`🔗 <f-return-codes>`


.. f:function:: frame_row_count(nrows)

   Retrieves number of rows in current frame

   :p integer(c_long) nrows [out]: Return variable for number of rows
   :r integer err: Return code :ref:`🔗 <f-return-codes>`


.. f:function:: frame_column_count(ncols)

   Retrieves number of columns in current frame

   :p integer(c_int) ncols [out]: Return variable for number of columns
   :r integer err: Return code :ref:`🔗 <f-return-codes>`


.. f:function:: frame_column_attributes(col[, name, type, element_size, element_size_doubles, bitfield_count])

   Retrieves column attributes from current frame

   :p integer col [in]: Target column index
   :o character(:) name [out,allocatable]: Return variable for column name
   :o integer type [out]: Return variable for column data type :ref:`🔗 <f-column-data-types>`
   :o integer element_size [out]: Return variable for column size in bytes
   :o integer element_size_doubles [out]: Return variable for column size in number of doubles
   :o integer bitfield_count [out]: Return variable for number of column bitfields
   :r integer err: Return code :ref:`🔗 <f-return-codes>`


.. f:function:: frame_bitfield_attributes(col, field[, name, offset, size])

   Retrieves bitfield attributes of a column

   :p integer col [in]: Target column index
   :p integer field [in]: Target bitfield index
   :o character(:) name [out,allocatable]: Return variable for bitfield name
   :o integer offset [out]: Return variable for bitfield offset
   :o integer size [out]: Return variable for bitfield size in bits
   :r integer err: Return code :ref:`🔗 <f-return-codes>`


.. f:function:: frame_properties_count(nproperties)

   Retrieves number of the properties associated with the logical frame

   :p integer nproperties [out]: Return variable for number of properties
   :r integer err: Return code :ref:`🔗 <f-return-codes>`


.. f:function:: frame_property_idx(idx, key, val)

   Retrieves the property key and value by its index

   :p integer idx [in]: Property index
   :p character(:) key [out,allocatable]: Return variable for property key
   :p character(:) val [out,allocatable]: Return variable for property value
   :r integer err: Return code :ref:`🔗 <f-return-codes>`


.. f:function:: frame_property(key[, val, exists])

   Retrieves the property value by its key

   :p character(:) key [in]: Property key
   :o character(:) val [out,allocatable]: Return variable for property value
   :o logical exists [out]: Return variable for property existence
   :r integer err: Return code :ref:`🔗 <f-return-codes>`


.. f:function:: decoder_initialise([column_major])

   Initialises current decoder

   :o logical column_major [in,default=.true.]: Whether to use the column-major memory layout
   :r integer err: Return code :ref:`🔗 <f-return-codes>`


.. f:function:: decoder_free()

   Deallocates memory used up by the current decoder

   :r integer err: Return code :ref:`🔗 <f-return-codes>`


.. f:function:: decoder_defaults_from_frame(frame)

   Configures current decoder to decode all data contained in the supplied frame

   :p odc_frame frame [in]: Frame instance
   :r integer err: Return code :ref:`🔗 <f-return-codes>`


.. f:function:: decoder_set_row_count(count)

   Sets number of rows to allocate in current decoder

   :p integer(c_long) count [in]: Number of rows to allocate
   :r integer err: Return code :ref:`🔗 <f-return-codes>`


.. f:function:: decoder_row_count(count)

   Retrieves number of rows that are allocated in current decoder

   :p integer(c_long) count [out]: Return variable for number of rows
   :r integer err: Return code :ref:`🔗 <f-return-codes>`


.. f:function:: decoder_set_data_array(data[, column_major])

   Sets an output data array into which the data may be decoded

   :p real(dp) data(:,:) [inout,target]: Data array to decode into
   :o logical column_major [in,default=.true.]: Whether the column-major memory layout is used
   :r integer err: Return code :ref:`🔗 <f-return-codes>`


.. f:function:: decoder_data_array([data, column_major])

   Retrieves the output data array into which the data may be decoded

   :o real(dp) data(:,:) [inout]: Data array for decoded data
   :o logical column_major [out]: Return variable for the used column-major memory layout
   :r integer err: Return code :ref:`🔗 <f-return-codes>`


.. f:function:: decoder_add_column(name)

   Adds a data column to current decoder

   :p character(:) name [in]: Data column name
   :r integer err: Return code :ref:`🔗 <f-return-codes>`


.. f:function:: decoder_column_count(count)

   Retrieves number of columns that are allocated in current decoder

   :p integer count [out]: Return variable for number of columns
   :r integer err: Return code :ref:`🔗 <f-return-codes>`


.. f:function:: decoder_column_set_data_size(col, element_size)

   Sets the decoded data size for a column in bytes

   :p integer col [in]: Column index
   :p integer(c_int) element_size [in]: Column data size in bytes
   :r integer err: Return code :ref:`🔗 <f-return-codes>`


.. f:function:: decoder_column_set_data_array(col[, element_size, stride, data])

   Sets an output data array into which the data associated with the column can be decoded

   :p integer col [in]: Column index
   :o integer element_size [in]: Column data size in bytes
   :o integer stride [in]: Column data width in bytes
   :o type(c_ptr) data [in]: Column data array
   :r integer err: Return code :ref:`🔗 <f-return-codes>`


.. f:function:: decoder_column_data_array(col[, element_size, element_size_doubles, stride, data])

   Retrieves the buffer and data layout into which the data has been decoded

   :p integer col [in]: Column index
   :o integer element_size [out]: Return variable for column data size in bytes
   :o integer element_size_doubles [out]: Return variable for column data size in doubles
   :o integer stride [out]: Return variable for column data width in bytes
   :o type(c_ptr) data [out]: Return variable for column data array
   :r integer err: Return code :ref:`🔗 <f-return-codes>`


.. f:function:: decoder_decode(frame, rows_decoded[, nthreads])

   Decodes the data described by the frame into the configured data array(s)

   :p odc_frame frame [inout]: Frame instance
   :p integer(c_long) rows_decoded [out]: Return variable for number of decoded rows
   :o integer nthreads [in]: Number of threads
   :r integer err: Return code :ref:`🔗 <f-return-codes>`


.. f:function:: encoder_initialise()

   Initialises current encoder

   :r integer err: Return code :ref:`🔗 <f-return-codes>`


.. f:function:: encoder_free()

   Deallocates memory used up by the current encoder

   :r integer err: Return code :ref:`🔗 <f-return-codes>`


.. f:function:: encoder_set_row_count(row_count)

   Sets number of rows to allocate in current encoder

   :p integer(c_long) row_count [in]: Number of rows
   :r integer err: Return code :ref:`🔗 <f-return-codes>`


.. f:function:: encoder_set_rows_per_frame(rows_per_frame)

   Sets number of rows to encode per frame

   :p integer(c_long) rows_per_frame [in]: Number of rows per frame
   :r integer err: Return code :ref:`🔗 <f-return-codes>`


.. f:function:: encoder_set_data_array(data[, column_major])

   Sets input data array from which data may be encoded

   :p real(dp) data(:,:) [in,target]: Data array to encode
   :o logical column_major [in,default=.true.]: Whether the data is in column-major memory layout
   :r integer err: Return code :ref:`🔗 <f-return-codes>`


.. f:function:: encoder_add_column(name, type)

   Adds a data column to current encoder

   :p character(:) name [in]: Column name
   :p integer type [in]: Column data type :ref:`🔗 <f-column-data-types>`
   :r integer err: Return code :ref:`🔗 <f-return-codes>`


.. f:function:: encoder_add_property(key, val)

   Adds a key/value property to encode as part of the frame

   :p character(:) key [in]: Property key
   :p character(:) val [in]: Property value
   :r integer err: Return code :ref:`🔗 <f-return-codes>`


.. f:function:: encoder_column_set_data_size(col[, element_size, element_size_doubles])

   Sets the source data size for a column

   :p integer col [in]: Column index
   :o integer element_size [in]: Column data size in bytes
   :o integer element_size_doubles [in]: Column data size in doubles, will take precedence over **element_size** if provided
   :r integer err: Return code :ref:`🔗 <f-return-codes>`


.. f:function:: encoder_column_set_data_array(col[, element_size, element_size_doubles, stride, data])

   Sets a custom data layout and data array for a column

   :p integer col [in]: Column index
   :o integer element_size [in]: Column size in bytes
   :o integer element_size_doubles [in]: Column size in doubles, will take precedence over **element_size** if provided
   :o integer stride [in]: Column width in bytes
   :o type(c_ptr) data [in]: Column data array
   :r integer err: Return code :ref:`🔗 <f-return-codes>`


.. f:function:: encoder_column_add_bitfield(col, name, nbits)

   Adds a bitfield to a column

   :p integer col [in]: Column index
   :p character(:) name [in]: Bitfield name
   :p integer nbits [in]: Bitfield size in bits
   :r integer err: Return code :ref:`🔗 <f-return-codes>`


.. f:function:: encoder_encode(outunit, bytes_written)

   Encodes the data to Fortran I/O unit

   :p integer outunit [in]: Valid Fortran I/O unit
   :p integer(c_long) bytes_written [out]: Return variable for number of bytes written
   :r integer err: Return code :ref:`🔗 <f-return-codes>`
