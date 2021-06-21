.. index:: Decoding Data

Decoding Data
=============

.. figure:: /_static/odb-2-frame-decoder.svg
   :alt: A Diagram Showing an Overview of the Data Decoding Process

   A Diagram Showing an Overview of the Data Decoding Process


   A **Frame** provides a sliding viewport into a stream of ODB-2 data managed by a **Reader**. The **Decoder** maps the data to be decoded onto a memory layout.


.. index:: Decoding Data; Reader

Reader
------

Depending on where the data is located (on the network, in the memory, in a file), we can define its source and pass it to the **Reader** object.

.. tabs::

   .. group-tab:: C

      .. code-block:: c

         odc_reader_t* reader = NULL;

         int rc = odc_open_path(&reader, "imaginary/path.odb");

         if (rc != ODC_SUCCESS) {
             fprintf(stderr, "Failed to open imaginary/path.odb: %s\n", odc_error_string(rc));
         }
         else {
             /*
              * Do work involving the reader here
              */
             odc_close(reader);
         }


   .. group-tab:: C++

      .. code-block:: cpp

         #include "eckit/io/FileHandle.h"

         FileHandle fh("imaginary/path.odb");
         fh.openForRead();
         AutoClose closer(fh);

         bool aggregated = true;
         Reader reader(fh, aggregated);

         // Do work involving the reader here


   .. group-tab:: Fortran

      .. code-block:: fortran

         type(odc_reader) :: reader
         integer :: rc

         rc = reader%open_path("imaginary/path.odb")

         if (rc /= ODC_SUCCESS) then
             print *, 'Failed to open imaginary/path.odb: ', odc_error_string(rc)
             stop 1
         else
             ! Do work involving the reader here

             rc = reader%close()
         end if


The **Reader** controls the file resources, and gives access to the underlying frames. The instance provides control of the aggregated or disaggregated access to frames, depending on whether the data is compatible or incompatible.

.. tabs::

   .. group-tab:: C

      .. code-block:: c

         odc_frame_t* frame = NULL;

         int rc = odc_new_frame(&frame, reader);

         if (rc != ODC_SUCCESS) {
             fprintf(stderr, "Failed to construct frame: %s\n", odc_error_string(rc));
         }
         else {
             long max_aggregated_rows = 1000000;

             while ((rc = odc_next_frame_aggregated(frame, max_aggregated_rows)) == ODC_SUCCESS) {
                 /*
                  * Do work involving the frame here
                  */
             }

             if (rc != ODC_ITERATION_COMPLETE) {
                 fprintf(stderr, "An error occurred reading the frames: %s\n", odc_error_string(rc));
             }
         }

         rc = odc_free_frame(frame);


   .. group-tab:: C++

      .. code-block:: cpp

         Frame frame;

         while ((frame = reader.next())) {
             // Do work involving the frame here
         }


   .. group-tab:: Fortran

      .. code-block:: fortran

         type(odc_frame) :: frame
         logical, parameter :: aggregated = .true.
         integer, parameter :: max_aggregated_rows = 1000000

         rc = frame%initialise(reader)

         if (rc /= ODC_SUCCESS) then
             print *, "Failed to construct frame: ", odc_error_string(rc)
         else
             rc = frame%next(aggregated, max_aggregated_rows)

             do while (rc == ODC_SUCCESS)
                 ! Do work involving the frame here

                 rc = frame%next(aggregated, max_aggregated_rows)
             end do

             if (rc /= ODC_ITERATION_COMPLETE) then
                 print *, "An error occurred reading the frames: ", odc_error_string(rc)
             end if
         end if


.. index:: Decoding Data; Frame

Frame
-----

The **Frame** is a viewport into a chunk of contiguous data within the ODB-2 stream. This data all has the same columnar structure (i.e. number, names of columns, and associated data types).

The **Frame** provides metadata about each chunk of data, including row counts, column information, etc. It can be used only for interrogating underlying data, without actually decoding it.


.. note::

   For the sake of clarity, many code snippets below omit necessary error checking when calling **odc** functions. Please see :doc:`/content/usage-examples` for full, runnable code examples with built-in error control.


.. tabs::

   .. group-tab:: C

      .. code-block:: c

         long row_count;
         int column_count;

         odc_frame_row_count(frame, &row_count);
         odc_frame_column_count(frame, &column_count);

         printf("Row count: %ld\nColumn count: %d\n\n", row_count, column_count);

         for (int col = 0; col < column_count; ++col) {
             const char* name;
             int type;
             int element_size;
             int bitfield_count;

             odc_frame_column_attributes(frame, col, &name, &type, &element_size, &bitfield_count);

             const char* type_name;

             odc_column_type_name(type, &type_name);

             printf("Column %d\n", col);
             printf("  name: %s\n", name);
             printf("  type: %s\n", type_name);
             printf("  size: %d\n", element_size);

             if (type == ODC_BITFIELD) {
                 for (int bf = 0; bf < bitfield_count; ++bf) {
                     const char* bf_name;
                     int bf_offset;
                     int bf_size;

                     odc_frame_bitfield_attributes(frame, col, bf, &bf_name, &bf_offset, &bf_size);

                     printf("  bitfield %d\n", bf);
                     printf("      name: %s\n", bf_name);
                     printf("    offset: %d\n", bf_offset);
                     printf("     nbits: %d\n", bf_size);
                 }
             }
         }


   .. group-tab:: C++

      .. code-block:: cpp

         std::cout << "Row count: " << frame.rowCount() << std::endl;
         std::cout << "Column count: " << frame.columnCount() << std::endl << std::endl;

         int i = 0;
         for (auto const& column : frame.columnInfo()) {
             std::cout << "Column " << i++ << std::endl;
             std::cout << "  name: " << column.name << std::endl;
             std::cout << "  type: " << columnTypeName(column.type) << std::endl;
             std::cout << "  size: " << column.decodedSize << std::endl;

             int j = 0;
             if (column.type == BITFIELD) {
                 for (auto const& bf : column.bitfield) {
                     std::cout << "  bitfield " << j++ << std::endl;
                     std::cout << "      name: " << bf.name << std::endl;
                     std::cout << "      offset: " << bf.offset << std::endl;
                     std::cout << "      nbits: " << bf.size << std::endl;
                 }
             }
         }


   .. group-tab:: Fortran

      .. code-block:: fortran

         integer(8), target :: row_count
         integer, target :: column_count
         integer, target :: col, type, element_size, bitfield_count
         integer, target :: bf, bf_offset, bf_size
         character(:), allocatable, target :: name, type_name, bf_name

         rc = frame%row_count(row_count)
         rc = frame%column_count(column_count)

         print *, "Row count: ", row_count
         print *, "Column count: ", column_count

         do col = 1, column_count
             rc = frame%column_attributes(col, name, type, element_size, bitfield_count=bitfield_count)
             rc = odc_column_type_name(type, type_name)

             print *, "Column ", col
             print *, "  name: ", name
             print *, "  type: ", type_name
             print *, "  size: ", element_size

             if (type == ODC_BITFIELD) then
                 do bf = 1, bitfield_count
                     rc = frame%bitfield_attributes(col, bf, bf_name, bf_offset, bf_size)

                     print *, "  bitfield ", bf
                     print *, "      name: ", bf_name
                     print *, "    offset: ", bf_offset
                     print *, "     nbits: ", bf_size
                 end do
             end if
         end do


The **Frame** may map exactly to one frame within the ODB-2 format (as described earlier), or may be an *aggregated frame* containing multiple compatible frames.


.. index:: Decoding Data; Span

Span
^^^^

The C++ API also provides the **Span** interface, which can be used to glimpse the values of specific columns within a **Frame**. This is useful when indexing data, where only certain columns might be used for the index.

In case there is a need for it, a **Span** also supports introducing constraints that a **Frame** must have constant values in a column. This offers a much quicker processing, as the data does not have to be decoded first.

.. code-block:: cpp

   class ExampleVisitor : public SpanVisitor {
       template <typename T>

       void dumpValues(const std::string& colName, const std::set<T>& vals) {
           std::cout << "name: " << colName << std::endl;
           for (const T& val : vals) {
               std::cout << val << std::endl;
           }
       }

       void operator()(const std::string& colName, const std::set<long>& vals) {
           std::cout << "Column with integer values" << std::endl;
           dumpValues(colName, vals);
       }

       void operator()(const std::string& colName, const std::set<double>& vals) {
           std::cout << "Column with real values" << std::endl;
           dumpValues(colName, vals);
       }

       void operator()(const std::string& colName, const std::set<std::string>& vals) {
           std::cout << "Column with string values" << std::endl;
           dumpValues(colName, vals);
       }
   };

   std::vector<std::string> columns = {
       "column0",
       "column2",
       "column3",
   };

   bool onlyConstantValues = false;

   Span span = frame.span(columns, onlyConstantValues);
   ExampleVisitor v;

   span.visit(v);


.. index:: Decoding Data; Properties

Properties
^^^^^^^^^^

Additional properties may be read from the **Frame**, in the form of key/value pairs. These are normally used for storing metadata.

.. tabs::

   .. group-tab:: C

      .. code-block:: c

         int nproperties;

         // Get number of properties encoded in the frame
         odc_frame_properties_count(frame, &nproperties);

         const char* key;
         const char* value;

         int i;
         for (i = 0; i < nproperties; i++) {

             // Get property key and value by its index
             odc_frame_property_idx(frame, i, &key, &value);

             printf("  Property: %s => %s\n", key, value);
         }

         // Or, get property value by its key
         odc_frame_property(frame, "my_key", &value);

         printf("  Property: my_key => %s\n", value ? value : "(undefined)");


   .. group-tab:: C++

      .. code-block:: cpp

         // Go through all properties
         for (const auto& property : frame.properties()) {
             std::cout << "  Property: " << property.first << " => " << property.second << std::endl;
         }

         // Or, get property value by its key
         auto it = frame.properties().find("my_key");
         std::cout << "  Property: my_key => "
                   << (it != frame.properties().end() ? it->second : "(undefined)") << std::endl;


   .. group-tab:: Fortran

      .. code-block:: fortran

         integer :: nproperties, idx
         character(:), allocatable, target :: key, val
         logical :: exists

         ! Get number of properties encoded in the frame
         rc = frame%properties_count(nproperties)

         do idx = 1, nproperties

            ! Get property key and value by its index
            frame%property_idx(idx, key, val)

            print *, "  Property: ", key, " => ", val
         end do

         ! Or, get property value by its key
         rc = frame%property('my_key', val, exists)

         if (exists) print *, "  Property: my_key => ", val


.. index:: Decoding Data; Decoder

.. _decoder:

Decoder
-------

The **Decoder** specifies how a decoding operation should be carried out: the columns that should be decoded, and the memory that the decoded data should be put into.

The decoding details can be filled in automatically from the **Frame**, in which case the **Decoder** will decode everything.

.. tabs::

   .. group-tab:: C

      .. code-block:: c

         odc_decoder_t* decoder = NULL;

         odc_new_decoder(&decoder);
         odc_decoder_defaults_from_frame(decoder, frame);

         long rows_decoded;
         odc_decode(decoder, frame, &rows_decoded);
         printf("Decoded %ld rows\n", rows_decoded);

         const void* data;
         long width;
         long height;
         bool columnMajor;
         odc_decoder_data_array(decoder, &data, &width, &height, &columnMajor);

         /* Note that these values describe the _array_ not the frame.
          * The array in memory is allowed to be bigger than strictly required
          * to store the data */

         printf("Decoded into a 2D array:\n");
         printf("  First element location: %p\n", data);
         printf("  Table width (bytes): %ld\n", width);
         printf("  Table height (rows): %ld\n", height);
         printf("  Column major: %s\n", (columnMajor ? "true" : "false"));


   .. group-tab:: C++

      .. note::

         C++ interface does not support automatic decoding of frame data. In this case, recommended API is C. Alternatively, you can construct a :ref:`custom memory layout <decoder-custom-layout>` decoder instead.


   .. group-tab:: Fortran

      .. code-block:: fortran

         type(odc_decoder) :: decoder
         integer(8), target :: rows_decoded
         real(8), pointer :: data(:,:)
         logical :: column_major

         rc = decoder%initialise()
         rc = decoder%defaults_from_frame(frame)

         rc = decoder%decode(frame, rows_decoded)
         print *, "Decoded ", rows_decoded, " rows"

         rc = decoder%data(data, column_major)

         print *, "Decoded into a 2D array:"
         print *, "  First element location: ", loc(data(1,1))
         print *, "  Table width (columns): ", size(data, 2)
         print *, "  Table height (rows): ", size(data, 1)
         print *, "  Column major: ", merge(" true", "false", column_major)

         rc = decoder%free()


The **Decoder** instance can be reused, in case memory mapping is the same across different frames. However, if the encoded data is sufficiently incompatible, a new **Decoder** instance will have to be created.

.. note::

   The **Decoder** does not have to be filled in from the information in the **Frame**, and certainly not from the current one. A decoder can be reused, for example in case a sequence of incompatible frames, that all have just two columns in common. It‘s possible to specify to extract just those two columns.


The **Decoder** provides several options for handling memory layouts.


.. _`decoder-row-major-layout`:

Row-major layout
   In row-major layout, the consecutive elements of a single data row reside one next to each other. The stride is the width of each row, representing a contiguous block in memory. In row-major mode, the width of each row is the combined size of all cells.

   .. figure:: /_static/odb-2-row-major.svg
      :alt: A Diagram Showing a Row-major Layout

      A Diagram Showing a Row-major Layout


   Row-major is the default method of storing multidimensional arrays in C and C++.

   .. tabs::

      .. group-tab:: C

         .. code-block:: c

            /*
             * Construct a decoder that will decode 5 named columns into a row-major
             * data layout
             */

            odc_decoder_t* decoder;
            odc_new_decoder(&decoder);

            odc_decoder_add_column(decoder, "column0");
            odc_decoder_add_column(decoder, "column1");
            odc_decoder_add_column(decoder, "column2");
            odc_decoder_add_column(decoder, "column3");
            odc_decoder_add_column(decoder, "column4");

            /* column3 is a 16-byte string column (hence takes 2 cols in the array --> ncols=6) */
            odc_decoder_column_set_data_size("column3", 3, 16);

            int nrows = 1000;
            int ncols = 6;
            double data[nrows][ncols];

            odc_decoder_set_data_array(decoder, data, ncols*sizeof(double), nrows, /* columnMajor */false);

            long rows_decoded;
            odc_decode(decoder, frame, &rows_decoded);

            /* And use the data ... */


      .. group-tab:: C++

         .. note::

            C++ interface does not support decoding of frame data into row-major layout. In this case, recommended API is C. Alternatively, you can construct a :ref:`custom memory layout <decoder-custom-layout>` decoder instead.


      .. group-tab:: Fortran

         .. code-block:: fortran

            ! Construct a decoder that will decode 5 named columns into a row-major
            ! data layout

            integer(8), parameter :: nrows = 1000
            integer, parameter :: ncols = 6
            real(8), target :: data(ncols, nrows)
            logical, parameter :: column_major = .false.

            rc = decoder%initialise(column_major)

            rc = decoder%add_column("column1")
            rc = decoder%add_column("column2")
            rc = decoder%add_column("column3")
            rc = decoder%add_column("column4")
            rc = decoder%add_column("column5")

            ! column4 is a 16-byte string column (hence takes 2 cols in the array --> ncols=6)
            rc = decoder%column_set_data_size("column4", 4, 16);

            rc = decoder%set_data(data, column_major)

            rc = decoder%decode(frame, rows_decoded)
            print *, "Decoded ", rows_decoded, " rows"

            ! And use the data ...


.. _`decoder-column-major-layout`:

Column-major layout
   In column-major layout, the consecutive elements of a single data column reside one below each other. The stride is the height of each column, representing a contiguous block in memory. In column-major mode, when the table is built, the cells are always 8-bytes wide.

   .. figure:: /_static/odb-2-column-major.svg
      :alt: A Diagram Showing a Column-major Layout

      A Diagram Showing a Column-major Layout


   Column-major is the default method of storing multidimensional arrays in Fortran.

   .. tabs::

      .. group-tab:: C

         .. code-block:: c

            /*
             * Construct a decoder that will decode 5 named columns into a column-major
             * data layout
             */

            odc_decoder_t* decoder;
            odc_new_decoder(&decoder);

            odc_decoder_add_column(decoder, "column0");
            odc_decoder_add_column(decoder, "column1");
            odc_decoder_add_column(decoder, "column2");
            odc_decoder_add_column(decoder, "column3");
            odc_decoder_add_column(decoder, "column4");

            /* column3 is a 16-byte string column (hence takes 2 cols in the array --> ncols=6) */
            odc_decoder_column_set_data_size("column3", 3, 16);

            int nrows = 1000;
            int ncols = 6;
            double data[ncols][nrows];

            odc_decoder_set_data_array(decoder, data, ncols*sizeof(double), nrows, /* columnMajor */true);

            long rows_decoded;
            odc_decode(decoder, frame, &rows_decoded);

            /* And use the data ... */


      .. group-tab:: C++

         .. note::

            C++ interface does not support decoding of frame data into column-major layout. In this case, recommended API is C. Alternatively, you can construct a :ref:`custom memory layout <decoder-custom-layout>` decoder instead.


      .. group-tab:: Fortran

         .. code-block:: fortran

            ! Construct a decoder that will decode 5 named columns into a column-major
            ! data layout

            integer(8), parameter :: nrows = 1000
            integer, parameter :: ncols = 6
            real(8), target :: data(nrows, ncols)
            logical, parameter :: column_major = .true.

            rc = decoder%initialise(column_major)

            rc = decoder%add_column("column1")
            rc = decoder%add_column("column2")
            rc = decoder%add_column("column3")
            rc = decoder%add_column("column4")
            rc = decoder%add_column("column5")

            ! column4 is a 16-byte string column (hence takes 2 cols in the array --> ncols=6)
            rc = decoder%column_set_data_size("column4", 4, 16);

            ! column major is the default in Fortran, so the column_major argument can be omitted
            rc = decoder%set_data(data)

            rc = decoder%decode(frame, rows_decoded)
            print *, "Decoded ", rows_decoded, " rows"

            ! And use the data ...


.. _`decoder-custom-layout`:

Custom layout
   For a custom layout, the stride size can be specified, leading to a more complex layout that better fits the data.

   For example, this is useful in Python workflows since *pandas/numpy* data frames are constructed with more optimized layouts. In this case all of the strings in data might be grouped by length, followed by all integers, and finally by real numbers.

   .. tabs::

      .. group-tab:: C

         .. code-block:: c

            /*
             * Construct a decoder that will decode 5 named columns into a custom
             * data layout
             */

            odc_decoder_t* decoder;
            odc_new_decoder(&decoder);

            odc_decoder_add_column(decoder, "column0");
            odc_decoder_add_column(decoder, "column1");
            odc_decoder_add_column(decoder, "column2");
            odc_decoder_add_column(decoder, "column3");
            odc_decoder_add_column(decoder, "column4");

            /* column3 is a 16-byte string column */
            odc_decoder_column_set_data_size("column3", 3, 16);

            int nrows = 1000;

            odc_decoder_set_row_count(decoder, nrows);

            uint64_t data0[nrows];
            uint64_t data1[nrows];
            double   data2[nrows];
            char     data3[nrows][16];
            double   data4[nrows];

            odc_decoder_column_set_data_array(decoder, 0, sizeof(uint64_t), sizeof(uint64_t), data0);
            odc_decoder_column_set_data_array(decoder, 1, sizeof(uint64_t), sizeof(uint64_t), data1);
            odc_decoder_column_set_data_array(decoder, 2, sizeof(double), sizeof(double), data2);
            odc_decoder_column_set_data_array(decoder, 3, 16, 16, data3);
            odc_decoder_column_set_data_array(decoder, 4, sizeof(double), sizeof(double), data4);

            long rows_decoded;
            odc_decode(decoder, frame, &rows_decoded);

            /* And use the data ... */


      .. group-tab:: C++

         .. code-block:: cpp

            // Construct a decoder that will decode 5 named columns into a custom
            // data layout

            size_t nrows = frame.rowCount();

            uint64_t data0[nrows];
            uint64_t data1[nrows];
            double data2[nrows];
            char data3[nrows][16];
            double data4[nrows];

            std::vector<std::string> columns {
                "column0",
                "column1",
                "column2",
                "column3",
                "column4",
            };

            std::vector<StridedData> strides {
                // ptr, nrows, element_size, stride
                {data0, nrows, sizeof(uint64_t), sizeof(uint64_t)},
                {data1, nrows, sizeof(uint64_t), sizeof(uint64_t)},
                {data2, nrows, sizeof(double), sizeof(double)},
                {data3, nrows, 16, 16}, // column3 is a 16-byte string column
                {data4, nrows, sizeof(double), sizeof(double)},
            };

            Decoder decoder(columns, strides);
            decoder.decode(frame);

            // And use the data ...


      .. group-tab:: Fortran

         .. code-block:: fortran

            ! Construct a decoder that will decode 5 named columns into a custom
            ! data layout

            use, intrinsic :: iso_c_binding

            integer(8), parameter :: nrows = 1000
            integer(8), target :: data1(nrows)
            integer(8), target :: data2(nrows)
            real(8), target :: data3(nrows)
            character(16), target :: data4(nrows)
            real(8), target :: data5(nrows)

            rc = decoder%initialise()

            rc = decoder%add_column("column1")
            rc = decoder%add_column("column2")
            rc = decoder%add_column("column3")
            rc = decoder%add_column("column4")
            rc = decoder%add_column("column5")

            ! column4 is a 16-byte string column (hence takes 2 cols in the array --> ncols=6)
            rc = decoder%column_set_data_size("column4", 4, 16);

            rc = decoder%set_row_count(nrows)

            rc = decoder%column_set_data_array(1, 8, 8, c_loc(data1))
            rc = decoder%column_set_data_array(2, 8, 8, c_loc(data2))
            rc = decoder%column_set_data_array(3, 8, 8, c_loc(data3))
            rc = decoder%column_set_data_array(4, 16, 16, c_loc(data4))
            rc = decoder%column_set_data_array(5, 8, 8, c_loc(data5))

            rc = decoder%decode(frame, rows_decoded)
            print *, "Decoded ", rows_decoded, " rows"

            ! And use the data ...


.. note::

   The decoded string data is highly likely to not be null terminated, and the storage space will be in multiples of 8 bytes (e.g. 8, 16, ...). If a decoded string is equal to the maximum length, there will be no null terminator in it. The user *must* account for this, by specifying a maximum length while using decoded strings.
