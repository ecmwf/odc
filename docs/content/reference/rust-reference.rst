.. index:: Reference; Rust API
   :name: rust-reference

Rust Reference
==============

Types
-----

.. describe:: struct Reader

   Owns an ODB-2 data stream and yields its frames.

   .. describe:: fn from_path(path: impl AsRef<Path>) -> Result<Reader>

      Open an ODB-2 file with default options.

      :Parameters:
         * **path** – file path to open
      :Errors: Fails if the file cannot be opened or is not valid ODB-2.

   .. describe:: fn from_path_with(path: impl AsRef<Path>, options: &ReaderOptions) -> Result<Reader>

      Open an ODB-2 file.

      :Parameters:
         * **path** – file path to open
         * **options** – aggregation behaviour
      :Errors: Fails if the file cannot be opened or is not valid ODB-2.

   .. describe:: fn from_handle(handle: DataHandle<Closed>, options: &ReaderOptions) -> Result<Reader>

      Read ODB-2 data from an eckit ``DataHandle`` (file, buffer, multi-file, byte range, …). The handle must not be open — the reader opens it for reading and owns it for its whole lifetime.

      :Parameters:
         * **handle** – closed eckit data handle to read from
         * **options** – aggregation behaviour
      :Errors: Fails if the handle cannot be opened or is not valid ODB-2.

   .. describe:: fn frames(&self) -> Frames

      Iterator over the frames of the stream. The iterator advances the underlying stream: each frame is yielded once, and a second call continues where the first stopped.


.. describe:: struct ReaderOptions

   Options for opening a :ref:`Reader <rust-reference>`.

   :Fields:
      * **aggregated** (``bool``) – aggregate consecutive compatible physical frames into logical frames
      * **row_limit** (``Option<i64>``) – maximum number of rows to aggregate into one logical frame


.. describe:: struct Frames

   Iterator over the frames of a reader, yielding ``Result<Frame>`` items.


.. describe:: struct Frame

   A viewport onto a chunk of contiguous, compatible data within an ODB-2 stream — possibly a logical frame aggregating several physical frames. Column metadata and properties are available without decoding.

   .. describe:: fn row_count(&self) -> usize

      Number of rows.

   .. describe:: fn column_count(&self) -> usize

      Number of columns.

   .. describe:: fn columns(&self) -> &[ColumnInfo]

      Column metadata, in frame order.

   .. describe:: fn column(&self, name: &str) -> Option<&ColumnInfo>

      Metadata of the named column.

   .. describe:: fn has_column(&self, name: &str) -> bool

      Whether the frame has a column with this name.

   .. describe:: fn properties(&self) -> &BTreeMap<String, String>

      Key/value properties encoded in the frame.

   .. describe:: fn span(&self, columns: &[&str], only_constant: bool) -> Result<Span>

      The sets of values of the named columns, and the frame's byte range in the stream, determined without decoding the frame.

      :Parameters:
         * **columns** – names of the columns to span
         * **only_constant** – require every named column to hold a single constant value across the frame
      :Errors: Fails if a named column does not exist, or the ``only_constant`` constraint is violated.

   .. describe:: fn dataframe(&self) -> Result<DataFrame>

      Decode all columns into a Polars ``DataFrame``. Missing values become nulls.

      :Errors: Fails if the underlying stream cannot be read or decoded.

   .. describe:: fn dataframe_with(&self, options: &DecodeOptions) -> Result<DataFrame>

      Decode selected columns into a Polars ``DataFrame``.

      :Parameters:
         * **options** – column selection and number of decode threads
      :Errors: Fails if a requested column does not exist or the underlying stream cannot be read or decoded.

   .. describe:: fn decode_into(&self, columns: &mut [(&str, DecodeTarget)], threads: usize) -> Result<usize>

      Decode the named columns into caller-allocated buffers, returning the number of rows decoded. Raw output: missing values keep their ODB sentinels and strings stay fixed-width NUL-padded cells.

      :Parameters:
         * **columns** – column names with their destination buffers
         * **threads** – number of decode threads
      :Errors: Fails if a column does not exist, a buffer does not fit its column, or the underlying stream cannot be read or decoded.


.. describe:: struct Span

   The sets of values of chosen columns within one frame, and that frame's byte range in the stream — determined without decoding the frame. Two spans compare equal when they cover the same columns with the same value sets.

   .. describe:: fn offset(&self) -> u64

      Byte offset of the frame within the data stream.

   .. describe:: fn length(&self) -> u64

      Length in bytes of the frame's encoded data.

   .. describe:: fn integer_values(&self, column: &str) -> Result<Vec<i64>>

      Integer values present in the named column, in ascending order.

      :Errors: Fails if the column is not part of the span or holds another type.

   .. describe:: fn real_values(&self, column: &str) -> Result<Vec<f64>>

      Floating-point values present in the named column, in ascending order.

      :Errors: Fails if the column is not part of the span or holds another type.

   .. describe:: fn string_values(&self, column: &str) -> Result<Vec<String>>

      String values present in the named column, in ascending order.

      :Errors: Fails if the column is not part of the span or holds another type.


.. describe:: struct ReadOptions

   Options for :ref:`read_odb and read_odb_single <rust-reference>`.

   :Fields:
      * **columns** (``Option<Vec<String>>``) – columns to decode, in the requested order; ``None`` decodes all
      * **aggregated** (``bool``) – aggregate consecutive compatible physical frames into logical frames
      * **threads** (``usize``) – number of decode threads per frame


.. describe:: struct DecodeOptions

   Options for decoding a frame into a ``DataFrame``.

   :Fields:
      * **columns** (``Option<Vec<String>>``) – columns to decode, in the requested order; ``None`` decodes all
      * **threads** (``usize``) – number of decode threads


.. describe:: enum DecodeTarget<'a>

   Caller-owned destination buffers for ``Frame::decode_into``. All slots are 8 bytes, matching the decoded ODB layout.

   :Variants:
      * **I64(&mut [i64])** – for INTEGER and BITFIELD columns
      * **F64(&mut [f64])** – for REAL and DOUBLE columns
      * **Str { data: &mut [u64], width: usize }** – for STRING columns: fixed-width NUL-padded cells of ``width`` bytes held in native-endian 8-byte slots


.. describe:: struct WriteOptions

   Options for encoding.

   :Fields:
      * **rows_per_frame** (``usize``) – maximum number of rows per physical output frame
      * **types** (``HashMap<String, ColumnType>``) – per-column overrides of the dtype-derived ODB column type
      * **properties** (``BTreeMap<String, String>``) – key/value properties attached to every output frame
      * **bitfields** (``HashMap<String, Vec<Bit>>``) – bit group layout for columns encoded as BITFIELD


.. describe:: struct RawColumn<'a>

   One column of ``write_odb_raw``.

   :Fields:
      * **name** (``&str``) – column name
      * **column_type** (``ColumnType``) – ``Integer`` or ``Bitfield`` for ``I64`` data, ``Double`` or ``Real`` for ``F64``, ``String`` for ``Str``
      * **data** (``EncodeSource``) – the column's values


.. describe:: enum EncodeSource<'a>

   Caller-owned source data for one column of ``write_odb_raw``. Missing values are represented by the sentinels ``integer_missing_value()`` in ``I64`` data and ``double_missing_value()`` in ``F64`` data.

   :Variants:
      * **I64(&[i64])** – integer or bitfield values
      * **F64(&[f64])** – floating-point values
      * **Str { data: &[u8], width: usize }** – fixed-width NUL-padded cells of ``width`` bytes


.. describe:: struct RowMajorColumn<'a>

   One column of ``write_odb_row_major``.

   :Fields:
      * **name** (``&str``) – column name
      * **column_type** (``ColumnType``) – any type except ``Ignore``; ``Bitfield`` requires a ``WriteOptions::bitfields`` entry
      * **size** (``usize``) – cell size in bytes: 8, except for string columns, which may span several 8-byte cells


.. describe:: struct ColumnInfo

   Metadata for one column of a frame.

   :Fields:
      * **name** (``String``) – column name
      * **column_type** (``ColumnType``) – column data type
      * **decoded_size** (``usize``) – size of a single decoded value in bytes (always a multiple of 8)
      * **bitfield** (``Vec<Bit>``) – bit groups; non-empty only for bitfield columns


.. describe:: struct Bit

   One named bit group of a bitfield column.

   :Fields:
      * **name** (``String``) – bit group name
      * **size** (``i32``) – bit group size in bits
      * **offset** (``i32``) – bit group offset in bits


.. describe:: struct Property

   A key/value property encoded in a frame.

   :Fields:
      * **key** (``String``) – property key
      * **value** (``String``) – property value


.. _`rust-column-data-types`:

Column Data Types
-----------------

.. describe:: ColumnType::Ignore

   Specifies that the column is ignored (invalid for real data)

.. describe:: ColumnType::Integer

   Specifies the column contains integer data — decodes as ``i64``

.. describe:: ColumnType::Real

   Specifies the column contains 32-bit floating point values

.. describe:: ColumnType::String

   Specifies the column contains character (string) data

.. describe:: ColumnType::Bitfield

   Specifies the column contains bitfield data — decodes as ``i64``

.. describe:: ColumnType::Double

   Specifies the column contains 64-bit floating point values


Functions
---------

.. describe:: fn read_odb(path: impl AsRef<Path>, options: &ReadOptions) -> Result<Vec<DataFrame>>

   Decode an ODB-2 file into one ``DataFrame`` per logical frame.

   :Parameters:
      * **path** – file path to open
      * **options** – column selection, aggregation and threading
   :Errors: Fails if the file cannot be opened, is not valid ODB-2, or a requested column does not exist.

.. describe:: fn read_odb_single(path: impl AsRef<Path>, options: &ReadOptions) -> Result<DataFrame>

   Decode an ODB-2 file into a single ``DataFrame``, concatenating all frames. Returns an empty ``DataFrame`` for an empty source.

   :Parameters:
      * **path** – file path to open
      * **options** – column selection, aggregation and threading
   :Errors: Fails like ``read_odb``, or if frames have incompatible schemas.

.. describe:: fn write_odb(df: &DataFrame, path: impl AsRef<Path>, options: &WriteOptions) -> Result<()>

   Encode a ``DataFrame`` into an ODB-2 file. Column types derive from dtypes: ``Int64`` (and smaller integers / ``Boolean``, widened) → INTEGER, ``Float64`` → DOUBLE, ``Float32`` → REAL, ``String`` → STRING; nulls become ODB missing values.

   :Parameters:
      * **df** – the data to encode
      * **path** – output file path
      * **options** – frame size, type overrides, properties and bitfields
   :Errors: Fails on an empty ``DataFrame``, unsupported dtypes, invalid type overrides or bitfield specifications, or if the file cannot be written.

.. describe:: fn write_odb_to(df: &DataFrame, handle: &mut DataHandle<Writing>, options: &WriteOptions) -> Result<()>

   Encode a ``DataFrame`` into an open eckit ``DataHandle`` (file, buffer, tee, …).

.. describe:: fn write_odb_raw(columns: &[RawColumn], path: impl AsRef<Path>, options: &WriteOptions) -> Result<()>

   Encode raw column slices into an ODB-2 file, without a ``DataFrame``. ``WriteOptions::types`` is ignored — each column's type is explicit.

   :Parameters:
      * **columns** – the columns to encode
      * **path** – output file path
      * **options** – frame size, properties and bitfields
   :Errors: Fails if a buffer does not match its column type, on an empty input, or if the file cannot be written.

.. describe:: fn write_odb_raw_to(columns: &[RawColumn], handle: &mut DataHandle<Writing>, options: &WriteOptions) -> Result<()>

   Encode raw column slices into an open eckit ``DataHandle``.

.. describe:: fn write_odb_row_major(cells: &[u64], columns: &[RowMajorColumn], path: impl AsRef<Path>, options: &WriteOptions) -> Result<()>

   Encode rows of 8-byte cells into an ODB-2 file. ``cells`` holds consecutive rows, each as wide as the summed column sizes. Within a row, an integer or bitfield cell holds an ``i64`` bit pattern, a real or double cell holds an ``f64`` bit pattern, and a string column's cells hold NUL-padded bytes.

   :Parameters:
      * **cells** – the row-major cell buffer
      * **columns** – name, type and cell size of each column
      * **options** – frame size, properties and bitfields
   :Errors: Fails on an invalid column size, a ``cells`` length that is not a whole number of rows, an empty buffer, an invalid bitfield specification, or if the file cannot be written.

.. describe:: fn write_odb_row_major_to(cells: &[u64], columns: &[RowMajorColumn], handle: &mut DataHandle<Writing>, options: &WriteOptions) -> Result<()>

   Encode rows of 8-byte cells into an open eckit ``DataHandle``.

.. describe:: fn version() -> String

   Release version of the odc C++ library, e.g. ``1.6.3``.

.. describe:: fn vcs_version() -> String

   Version control checksum of the odc C++ library.

.. describe:: fn integer_missing_value() -> i64

   The sentinel value marking a missing integer in ODB-2 data.

.. describe:: fn double_missing_value() -> f64

   The sentinel value marking a missing double in ODB-2 data.


Error Handling
--------------

All fallible functions return ``Result``; see :ref:`the API design guide <rust-interface>` for the error handling conventions.

.. describe:: type Result<T> = std::result::Result<T, Error>

   Result alias used throughout the crate.

.. describe:: enum Error

   Errors returned by this crate.

   :Variants:
      * **Odc** – typed odc C++ exception
      * **Eckit** – typed eckit C++ exception (e.g. from ``DataHandle`` operations)
      * **Polars** – Polars error while building or consuming a ``DataFrame``
      * **UnsupportedDtype** – a ``DataFrame`` column has a dtype that cannot be encoded to ODB-2
      * **UnsupportedColumnType** – a requested ODB column cannot be decoded (e.g. type ``Ignore``)
      * **InvalidTypeOverride** – a type override in ``WriteOptions::types`` is not compatible with the column's dtype
      * **ColumnNotFound** – column not found in the frame
      * **InvalidBuffer** – a caller-provided buffer does not fit its column
      * **InvalidRowMajorData** – a row-major cell buffer does not match its declared columns
      * **InvalidBitfield** – an invalid bitfield specification
      * **EmptyDataFrame** – nothing to encode
