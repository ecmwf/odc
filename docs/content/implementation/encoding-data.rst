.. index:: Encoding Data

Encoding Data
=============

The encoding process works in the other way. Data in memory is described by the **Encoder** and, given an appropriate data stream, data will be encoded into frames.

Each time a new data structure is encountered, it will be described by a different encoder instance, but the encoded data will result in a subsequent frame in the stream.


.. index:: Encoding Data; Encoder

Encoder
-------

Just like the **Decoder**, the **Encoder** provides several options for handling memory layouts.

Row-major layout
   In :ref:`row-major layout <decoder-row-major-layout>`, the consecutive elements of a single data row reside one next to each other.

   .. tabs::

      .. group-tab:: C

         .. code-block:: c

            long nrows = 1000;
            int ncols = 6;
            double data[nrows][ncols];
            // set up the data here...

            odc_encoder_t* encoder = NULL;
            odc_new_encoder(&encoder);

            odc_encoder_add_column(encoder, "column0", ODC_INTEGER);
            odc_encoder_add_column(encoder, "column1", ODC_INTEGER);
            odc_encoder_add_column(encoder, "column2", ODC_REAL);
            odc_encoder_add_column(encoder, "column3", ODC_STRING);
            odc_encoder_add_column(encoder, "column4", ODC_REAL);

            // column3 is a 16-byte string column (hence takes 2 cols in the array --> ncols=6)
            odc_encoder_column_set_data_size(encoder, 3, 16);

            odc_encoder_set_data_array(encoder, data, ncols*sizeof(double), nrows, 0);

            // encode the data here...

            odc_free_encoder(encoder);


      .. group-tab:: C++

         .. note::

            C++ interface does not support data encoding from a row-major layout. In this case, recommended API is C. Alternatively, you can construct a :ref:`custom memory layout <encoder-custom-layout>` encoder instead.


      .. group-tab:: Fortran

         .. code-block:: fortran

            integer(8), parameter :: nrows = 1000
            integer, parameter :: ncols = 6
            real(8), target :: data(ncols, nrows)
            ! set up the data here...

            type(odc_encoder) :: encoder
            logical, parameter :: column_major = .false.
            integer, target :: outunit
            integer(8), target :: bytes_written

            rc = encoder%initialise()

            rc = encoder%add_column("column1", ODC_INTEGER)
            rc = encoder%add_column("column2", ODC_INTEGER)
            rc = encoder%add_column("column3", ODC_REAL)
            rc = encoder%add_column("column4", ODC_STRING)
            rc = encoder%add_column("column5", ODC_REAL)

            ! column4 is a 16-byte string column (hence takes 2 cols in the array --> ncols=6)
            rc = encoder%column_set_data_size(4, 16)

            rc = encoder%set_data(data, column_major)

            ! encode the data here...

            rc = encoder%free()


Column-major layout
   In :ref:`column-major layout <decoder-column-major-layout>`, the consecutive elements of a single data column reside one below each other.

   .. tabs::

      .. group-tab:: C

         .. code-block:: c

            long nrows = 1000;
            int ncols = 6;
            double data[ncols][nrows];
            // set up the data here...

            odc_encoder_t* encoder = NULL;
            odc_new_encoder(&encoder);

            odc_encoder_add_column(encoder, "column0", ODC_INTEGER);
            odc_encoder_add_column(encoder, "column1", ODC_INTEGER);
            odc_encoder_add_column(encoder, "column2", ODC_REAL);
            odc_encoder_add_column(encoder, "column3", ODC_STRING);
            odc_encoder_add_column(encoder, "column4", ODC_REAL);

            // column3 is a 16-byte string column (hence takes 2 cols in the array --> ncols=6)
            odc_encoder_column_set_data_size(encoder, 3, 16);

            odc_encoder_set_data_array(encoder, data, ncols*sizeof(double), nrows, sizeof(double));

            // encode the data here...

            odc_free_encoder(encoder);


      .. group-tab:: C++

         .. note::

            C++ interface does not support data encoding from a column-major layout. In this case, recommended API is C. Alternatively, you can construct a :ref:`custom memory layout <encoder-custom-layout>` encoder instead.


      .. group-tab:: Fortran

         .. code-block:: fortran

            integer(8), parameter :: nrows = 1000
            integer, parameter :: ncols = 6
            real(8), target :: data(nrows, ncols)
            ! set up the data here...

            type(odc_encoder) :: encoder
            logical, parameter :: column_major = .true.
            integer, target :: outunit
            integer(8), target :: bytes_written

            rc = encoder%initialise()

            rc = encoder%add_column("column1", ODC_INTEGER)
            rc = encoder%add_column("column2", ODC_INTEGER)
            rc = encoder%add_column("column3", ODC_REAL)
            rc = encoder%add_column("column4", ODC_STRING)
            rc = encoder%add_column("column5", ODC_REAL)

            ! column4 is a 16-byte string column (hence takes 2 cols in the array --> ncols=6)
            rc = encoder%column_set_data_size(4, 16);

            ! column major is the default in Fortran, so the column_major argument can be omitted
            rc = encoder%set_data(data)

            ! encode the data here...

            rc = encoder%free()


.. _`encoder-custom-layout`:

Custom layout
   For a :ref:`custom layout <decoder-custom-layout>`, the stride size can be specified, leading to a more complex layout that better fits the data.

   .. tabs::

      .. group-tab:: C

         .. code-block:: c

            long nrows = 1000;

            uint64_t data0[nrows];
            uint64_t data1[nrows];
            double data2[nrows];
            char data3[nrows][16];
            double data4[nrows];
            // set up the data here...

            odc_encoder_t* encoder = NULL;
            odc_new_encoder(&encoder);

            odc_encoder_set_row_count(encoder, nrows);

            odc_encoder_add_column(encoder, "column0", ODC_INTEGER);
            odc_encoder_add_column(encoder, "column1", ODC_INTEGER);
            odc_encoder_add_column(encoder, "column2", ODC_REAL);
            odc_encoder_add_column(encoder, "column3", ODC_STRING);
            odc_encoder_add_column(encoder, "column4", ODC_REAL);

            // column3 is a 16-byte string column
            odc_encoder_column_set_data_size(encoder, 3, 16);

            odc_encoder_column_set_data_array(encoder, 0, sizeof(uint64_t), sizeof(uint64_t), data0);
            odc_encoder_column_set_data_array(encoder, 1, sizeof(uint64_t), sizeof(uint64_t), data1);
            odc_encoder_column_set_data_array(encoder, 2, sizeof(double), sizeof(double), data2);
            odc_encoder_column_set_data_array(encoder, 3, 16, 16, data3);
            odc_encoder_column_set_data_array(encoder, 4, sizeof(double), sizeof(double), data4);

            // encode the data here...

            odc_free_encoder(encoder);


      .. group-tab:: C++

         .. code-block:: cpp

            size_t nrows = 1000;

            uint64_t data0[nrows];
            uint64_t data1[nrows];
            double data2[nrows];
            char data3[nrows][16];
            double data4[nrows];
            // set up the data here...

            std::vector<ColumnInfo> columns = {
                ColumnInfo{std::string("column0"), ColumnType(INTEGER), sizeof(uint64_t)},
                ColumnInfo{std::string("column1"), ColumnType(INTEGER), sizeof(uint64_t)},
                ColumnInfo{std::string("column2"), ColumnType(REAL), sizeof(double)},
                ColumnInfo{std::string("column3"), ColumnType(STRING), 16},
                ColumnInfo{std::string("column4"), ColumnType(REAL), sizeof(double)},
            };

            std::vector<ConstStridedData> strides {
                // ptr, nrows, element_size, stride
                {data0, nrows, sizeof(uint64_t), sizeof(uint64_t)},
                {data1, nrows, sizeof(uint64_t), sizeof(uint64_t)},
                {data2, nrows, sizeof(double), sizeof(double)},
                {data3, nrows, 16, 16},
                {data4, nrows, sizeof(double), sizeof(double)},
            };

            // encode the data here...


      .. group-tab:: Fortran

         .. code-block:: fortran

            use, intrinsic :: iso_c_binding

            integer(8), parameter :: nrows = 1000
            integer(8), target :: data1(nrows)
            integer(8), target :: data2(nrows)
            real(8), target :: data3(nrows)
            character(16), target :: data4(nrows)
            real(8), target :: data5(nrows)
            ! set up the data here...

            type(odc_encoder) :: encoder
            integer, target :: outunit
            integer(8), target :: bytes_written

            rc = encoder%initialise()

            rc = encoder%set_row_count(nrows)

            rc = encoder%add_column("column1", ODC_INTEGER)
            rc = encoder%add_column("column2", ODC_INTEGER)
            rc = encoder%add_column("column3", ODC_REAL)
            rc = encoder%add_column("column4", ODC_STRING)
            rc = encoder%add_column("column5", ODC_REAL)

            ! column4 is a 16-byte string column
            rc = encoder%column_set_data_size(4, 16)

            rc = encoder%column_set_data_array(1, 8, stride=8, data=c_loc(data1))
            rc = encoder%column_set_data_array(2, 8, stride=8, data=c_loc(data2))
            rc = encoder%column_set_data_array(3, 8, stride=8, data=c_loc(data3))
            rc = encoder%column_set_data_array(4, 16, stride=16, data=c_loc(data4))
            rc = encoder%column_set_data_array(5, 8, stride=8, data=c_loc(data5))

            ! encode the data here...

            rc = encoder%free()


Once an **Encoder** has been set up properly, data can be encoded with it.

.. tabs::

   .. group-tab:: C

      C supports data encoding in three ways.

      File descriptor
         Data can be encoded into an already open file descriptor using ``odc_encode_to_file_descriptor()`` function.

         .. code-block:: c

            #include <fcntl.h>
            #include <unistd.h>

            int file_descriptor = open("imaginary/path.odb", O_CREAT|O_TRUNC|O_WRONLY, 0666);
            long bytes_encoded;

            odc_encode_to_file_descriptor(encoder, file_descriptor, &bytes_encoded);

            close(file_descriptor);


      Memory buffer
         Data can be encoded into a pre-allocated memory buffer using ``odc_encode_to_buffer()`` function.

         .. code-block:: c

            char buffer[4096];
            long bytes_encoded;

            odc_encode_to_buffer(encoder, buffer, sizeof(buffer), &bytes_encoded);


         .. note::

            In case an insufficiently large buffer is supplied, an error will be returned.


      Stream handler
         Data can be encoded via a stream handler using ``odc_encode_to_stream()`` function. A callback of signature ``odc_stream_write_t`` is passed together with ``context``, which will be sequentially called with appropriate ``buffer`` and ``length`` pairs.

         .. code-block:: c

            long write_fn(void* context, const void* buffer, long length) {
                // user defined action
                return length; // return handled length
            }

            // user defined context, passed unchanged to callback
            void* context;
            long bytes_encoded;

            odc_encode_to_stream(encoder, context, write_fn, &bytes_encoded);


   .. group-tab:: C++

      C++ supports data encoding into `eckit`_ ``DataHandle`` objects and their derivatives. These are very general objects, that can refer to any data source, hence their utility.

      ``FileHandle`` (eckit)
         .. code-block:: cpp

            #include "eckit/io/FileHandle.h"

            const Length length;

            FileHandle fh("imaginary/path.odb");
            fh.openForWrite(length);
            AutoClose closer(fh);

            encode(fh, columns, strides);


   .. group-tab:: Fortran

      Fortran supports data encoding to standard I/O.

      .. code-block:: fortran

         integer :: outunit
         integer(8), target :: bytes_written

         open(newunit=outunit, file="imaginary/path.odb", access="stream", form="unformatted")
         rc = encoder%encode(outunit, bytes_written)
         close(outunit)


.. index:: Encoding Data; Bitfields

Bitfields
---------

Bitfield columns can be used to store data for *flags*, up to a maximum of 32-bits per column. Within an integer, the bits can be identified and named by their offset. Groups of bits are identified as well as individual bits, therefore each item has an offset and a size.

   .. tabs::

      .. group-tab:: C

         .. code-block:: c

            long nrows = 1000;
            int ncols = 1;
            uint64_t data[nrows];
            // set up the data here...

            odc_encoder_t* encoder = NULL;
            odc_new_encoder(&encoder);

            odc_encoder_set_row_count(encoder, nrows);

            odc_encoder_add_column(encoder, "flags", ODC_BITFIELD);

            odc_encoder_column_add_bitfield(encoder, 0, "flag_a", 1);
            odc_encoder_column_add_bitfield(encoder, 0, "flag_b", 2);
            odc_encoder_column_add_bitfield(encoder, 0, "flag_c", 3);
            odc_encoder_column_add_bitfield(encoder, 0, "flag_d", 1);

            odc_encoder_column_set_data_array(encoder, 0, sizeof(uint64_t), sizeof(uint64_t), data);

            // encode the data here...

            odc_free_encoder(encoder);


      .. group-tab:: C++

         .. code-block:: cpp

            size_t nrows = 1000;
            uint64_t data[nrows];
            // set up the data here...

            std::vector<ColumnInfo::Bit> bitfields = {
                // name, size, offset+=size(n-1)
                {"flag_a", 1, 0},
                {"flag_b", 2, 1},
                {"flag_c", 3, 3},
                {"flag_d", 1, 6},
            };

            std::vector<ColumnInfo> columns = {
                ColumnInfo{std::string("flags"), ColumnType(BITFIELD), sizeof(uint64_t), bitfields},
            };

            std::vector<ConstStridedData> strides {
                // ptr, nrows, element_size, stride
                {data, nrows, sizeof(uint64_t), sizeof(uint64_t)},
            };

            // encode the data here...


      .. group-tab:: Fortran

         .. code-block:: fortran

            integer(8), parameter :: nrows = 1000
            integer, parameter :: ncols = 1
            integer(8), target :: data(nrows)
            ! set up the data here...

            type(odc_encoder) :: encoder

            rc = encoder%initialise()

            rc = encoder%set_row_count(nrows)

            rc = encoder%add_column("flags", ODC_BITFIELD)

            rc = encoder%column_add_bitfield(1, "flag_a", 1)
            rc = encoder%column_add_bitfield(1, "flag_b", 2)
            rc = encoder%column_add_bitfield(1, "flag_c", 3)
            rc = encoder%column_add_bitfield(1, "flag_d", 1)

            rc = encoder%column_set_data_array(1, 8, stride=8, data=c_loc(data))

            ! encode the data here...

            rc = encoder%free()


.. index:: Encoding Data; Properties

Properties
----------

Additional properties may be encoded as part of frame’s data, in the form of key/value pairs. This can be useful for metadata that describes the encoded data itself.

.. tabs::

   .. group-tab:: C

      .. code-block:: c

         const char* property_key = "encoded_by";
         const char* property_value = "ECMWF";

         odc_encoder_add_property(encoder, property_key, property_value);


   .. group-tab:: C++

      .. code-block:: cpp

         std::map<std::string, std::string> properties = {
             { "encoded_by", "ECMWF" },
         };

         // pass properties to encode()


   .. group-tab:: Fortran

      .. code-block:: fortran

         rc = encoder%add_property("encoded_by", "ECMWF")


.. _`eckit`: https://github.com/ecmwf/eckit
