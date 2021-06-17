Tools
=====

**odc** provides several tools for manipulating and inspecting ODB-2 data from the command line.

``odc``
-------

All commands and their help pages are available under this joint command.

Usage
   .. code-block:: shell

      odc <command> [<command-parameters>]
      odc help <command>

Options
   ``<command>``
      One of available commands:

      - `compact`_
      - `compare`_
      - `count`_
      - `fixrowsize`_
      - `header`_
      - `import`_
      - `ls`_
      - `mdset`_
      - `merge`_
      - `set`_
      - `split`_
      - `sql`_
      - `xyv`_

      .. - `index`_


``compact``
-----------

Tries to compress a file.

Usage
   .. code-block:: shell

      odc compact <input.odb> <output.odb>

Options
   ``<input.odb>``
      Name of the input ODB-2 file.

   ``<output.odb>``
      Name of the output ODB-2 file.

Example
   .. code-block:: shell

      odc compact data.odb data-compacted.odb

      000 2021-05-11 14:17:13 (I) Verifying.
      000 2021-05-11 14:17:13 (I) Comparator::compare: (1) data.odb to (2) data-compacted.odb


``compare``
-----------

Compares two ODB files.

Usage
   .. code-block:: shell

      odc compare [-excludeColumns <list-of-columns>] [-excludeColumnsTypes <list-of-column-types>] [-dontCheckMissing] <file1.odb> <file2.odb>

Options
   ``-excludeColumns <list-of-columns>``
      Exclude columns from the comma-separated list.

   ``-excludeColumnsTypes <list-of-column-types>``
      Exclude columns of listed data types (comma-separated list).

   ``-dontCheckMissing``
      Do not compare missing values.

   ``<file1.odb>``
      File to compare.

   ``<file2.odb>``
      File to compare with.

Example
   .. code-block:: shell

      odc compare -excludeColumns "expver,wigos@hdr,integer_missing,double_missing" data-1.odb data-3.odb

      000 2021-05-11 14:19:53 (I) excludedColumns:[wigos@hdr,integer_missing,double_missing,]
      000 2021-05-11 14:19:53 (I) Comparator::compare: (1) data-1.odb to (2) data-3.odb
      000 2021-05-11 14:19:53 (E) Exception: Values different in column expver: 1 is not equal 2
      000 2021-05-11 14:19:53 (E)
      000 2021-05-11 14:19:53 (I) While comparing rows number 1, columns 0 found different.
      000 2021-05-11 14:19:53 (I)  Values different in column expver: 1 is not equal 2

      000 2021-05-11 14:19:53 (I)  data1[0] = 1.000000e+00
      000 2021-05-11 14:19:53 (I)  data2[0] = 2.000000e+00
      000 2021-05-11 14:19:53 (I)  md1[0] = name: expver, type: INTEGER, codec: constant, value=1.000000, hasMissing=false
      000 2021-05-11 14:19:53 (I)  md2[0] = name: expver, type: INTEGER, codec: constant, value=2.000000, hasMissing=false
      000 2021-05-11 14:19:53 (E) Exception: Files differ.
      000 2021-05-11 14:19:53 (I) Comparing files data-1.odb and data-3.odb: 0.001221 second elapsed, 0.001068 second cpu
      000 2021-05-11 14:19:53 (E) ** Files differ.  Caught in  (../src/eckit/runtime/Tool.cc +31 start)
      000 2021-05-11 14:19:53 (E) ** Exception terminates odc


``count``
---------

Counts number of rows in files.

Usage
   .. code-block:: shell

      odc count <file.odb>

Options
   ``<file.odb>``
      Name of the ODB-2 file.

Example
   .. code-block:: shell

      odc count data.odb

      161811


``fixrowsize``
---------

Converts file to a format with fixed size rows.

Usage
   .. code-block:: shell

      odc fixrowsize <input.odb> <output.odb>

Options
   ``<input.odb>``
      Name of the input ODB-2 file.

   ``<output.odb>``
      Name of the output ODB-2 file.

Example
   .. code-block:: shell

      odc fixrowsize data.odb data-fixed.odb

      000 2021-06-17 13:59:45 (I) Verifying.
      000 2021-06-17 13:59:45 (I) Comparator::compare: (1) data.odb to (2) data-fixed.odb


``header``
----------

Shows header(s) and metadata(s) of file.

Usage
   .. code-block:: shell

      odc header [-offsets] [-ddl] [-table <table-name-in-the-generated-ddl>] <file.odb>

Options
   ``-offsets``
      Print only data offsets within the file.

   ``-ddl``
      Print Data Definition Language (DDL) query that describes the data frame.

   ``-table <table-name-in-the-generated-ddl>``
      Define the name of the table in the DDL query.

   ``<file.odb>``
      Name of the ODB-2 file.

Examples
   .. code-block:: shell

      odc header data.odb

      Header 1. Begin offset: 0, end offset: 67698, number of rows in block: 10000, byteOrder: same
      0. name: date@hdr, type: INTEGER, codec: constant, value=20210401.000000, hasMissing=false
      1. name: lat@hdr, type: REAL, codec: short_real, range=<-54.840000,79.980003>, hasMissing=false
      2. name: lon@hdr, type: REAL, codec: short_real, range=<-172.009995,177.740005>, hasMissing=false
      3. name: obsvalue@body, type: REAL, codec: short_real, range=<-12.400000,102980.000000>, hasMissing=true, missingValue=-2147483647.000000

      Header 2. Begin offset: 67698, end offset: 135476, number of rows in block: 10000, byteOrder: same
      0. name: date@hdr, type: INTEGER, codec: constant, value=20210401.000000, hasMissing=false
      1. name: lat@hdr, type: REAL, codec: short_real, range=<-62.189999,70.680000>, hasMissing=false
      2. name: lon@hdr, type: REAL, codec: short_real, range=<-175.149994,179.199997>, hasMissing=false
      3. name: obsvalue@body, type: REAL, codec: short_real, range=<-15.756924,103320.000000>, hasMissing=true, missingValue=-2147483647.000000

      ...

   .. code-block:: shell

      odc header -offsets data.odb

      0 67698 10000 4
      67698 67778 10000 4
      135476 67831 10000 4
      203307 67738 10000 4
      271045 67953 10000 4
      338998 68023 10000 4
      407021 68054 10000 4
      475075 67664 10000 4
      542739 67844 10000 4
      610583 67964 10000 4
      678547 67826 10000 4
      746373 68043 10000 4
      814416 68016 10000 4
      882432 67781 10000 4
      950213 67774 10000 4
      1017987 67575 10000 4
      1085562 12660 1811 4

   .. code-block:: shell

      odc header -ddl -table observations data.odb

      CREATE TABLE observations AS (
         date@hdr INTEGER,
         lat@hdr REAL,
         lon@hdr REAL,
         obsvalue@body REAL,
      ) ON 'data.odb';


``import``
----------

Imports data from a text file.

Data column headers must be in the following format:

.. code-block:: none

   NAME:TYPE

For example:

.. code-block:: none

   col1:INTEGER,col2:REAL,col3:STRING

Usage
   .. code-block:: shell

      odc import [-d delimiter] <file.txt> <file.odb>

Options
   ``-d delimiter``
      Data delimiter, can be a single character (e.g.: ``,``) or ``TAB``.

   ``<file.txt>``
      Name of the text file.

   ``<file.odb>``
      Name of the ODB-2 file.

Example
   .. code-block:: shell

      odc import -d , data-1.csv data-1.odb

      000 2021-05-11 14:09:36 (I) ImportTool::run: inFile: data-1.csv, outFile: data-1.odb
      000 2021-05-11 14:09:36 (I) TextReaderIterator::parseHeader: columns: [expver:INTEGER,date@hdr:INTEGER,statid@hdr:STRING,wigos@hdr:STRING,obsvalue@body:REAL,integer_missing:INTEGER,double_missing:REAL]
      000 2021-05-11 14:09:36 (I) TextReaderIterator::parseHeader: delimiter: ','
      000 2021-05-11 14:09:36 (I) TextReaderIterator::parseHeader: header: 'expver:INTEGER,date@hdr:INTEGER,statid@hdr:STRING,wigos@hdr:STRING,obsvalue@body:REAL,integer_missing:INTEGER,double_missing:REAL'
      000 2021-05-11 14:09:36 (I) ImportTool::odbFromCSV: Copied 10 rows.


.. .. todo::
   Check why this command does not work:

   .. code-block:: shell

      odc index data.odb
      000 2021-05-12 08:38:13 (E) Exception: Assertion failed: !s->second.opened_ in open, line 104 of /tmp/metabuilds/ecflow-metab_5062/leap42/GNU.73/eckit/eckit/src/eckit/io/PooledHandle.cc
      Assertion failed: !s->second.opened_ in open, line 104 of /tmp/metabuilds/ecflow-metab_5062/leap42/GNU.73/eckit/eckit/src/eckit/io/PooledHandle.cc
      backtrace [2] stack has 13 addresses
      (/usr/local/apps/eckit/1.16.0/GNU/7.3.0/lib/libeckit.so+eckit::BackTrace::dump[abi:cxx11]())0x18b
      (/usr/local/apps/eckit/1.16.0/GNU/7.3.0/lib/libeckit.so+eckit::AssertionFailed::AssertionFailed(std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> > const&, eckit::CodeLocation const&))0x58a
      (/usr/local/apps/eckit/1.16.0/GNU/7.3.0/lib/libeckit.so+eckit::PooledHandle::openForRead())0x6a3
      (/usr/local/apps/eckit/1.16.0/GNU/7.3.0/lib/libeckit.so+eckit::PartFileHandle::openForRead())0x26
      (/usr/local/apps/odc/1.3.0/GNU/7.3.0/bin/../lib/libodccore.so+odc::Select::Select(std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> > const&, eckit::DataHandle&, bool))0x30
      (/usr/local/apps/odc/1.3.0/GNU/7.3.0/bin/../lib/libodccore.so+odc::Indexer::createIndex(eckit::PathName const&, eckit::PathName const&))0x519
      (/usr/local/apps/odc/1.3.0/GNU/7.3.0/bin/../lib/libodctools.so+odc::tool::IndexTool::run())0x4b7
      (/usr/local/apps/odc/1.3.0/GNU/7.3.0/bin/../lib/libodctools.so+odc::tool::ToolRunnerApplication::run())0x21
      (/usr/local/apps/eckit/1.16.0/GNU/7.3.0/lib/libeckit.so+eckit::Tool::start())0x11
      (odc)
      (odc)
      (/lib64/libc.so.6+__libc_start_main)0xf5
      (odc)

      end of backtrace dump ...
      000 2021-05-12 08:38:13 (E) ** Assertion failed: !s->second.opened_ in open, line 104 of /tmp/metabuilds/ecflow-metab_5062/leap42/GNU.73/eckit/eckit/src/eckit/io/PooledHandle.cc Caught in  (/tmp/metabuilds/ecflow-metab_5062/leap42/GNU.73/eckit/eckit/src/eckit/runtime/Tool.cc +31 start)
      000 2021-05-12 08:38:13 (E) ** Exception terminates odc

   ``index``
   ---------

   Creates index of reports for a given file.

   The index file is an ODB-2 file with following integer columns:

   - ``block_begin``
   - ``block_length``
   - ``seqno``
   - ``n_rows``

   One entry is made for each unique ``seqno``, a block pair within the source ODB-2 file.

   Usage
      .. code-block:: shell

         odc index <file.odb> [<file.odb.idx>]

   Options
      ``<file.odb>``
         Name of the ODB-2 file.

      ``<file.odb.idx>``
         Name of the index file.

   Example
      .. code-block:: shell

         odc index data.odb data.odb.idx


``ls``
------

Shows file’s contents.

Usage
   .. code-block:: shell

      odc ls [-o <file.txt>] <file.odb>

Options
   ``-o <file.txt>``
      Name of the output file. If omitted, contents will be printed on standard output.

   ``<file.odb>``
      Name of the ODB-2 file.

Example
   .. code-block:: shell

      odc ls -o data.txt data.odb

      000 2021-05-11 14:33:01 (I) Selected 161811 row(s).


``mdset``
---------

Creates a new file resetting types or values (constants only) of columns.

Usage
   .. code-block:: shell

      odc mdset <update-list> <input.odb> <output.odb>

Options
   ``<update-list>``
      A comma separated list of expressions of the form:

      .. code-block:: shell

         <column-name> : <type> = <value>

      ``<type>`` can be one of:

      - ``integer``
      - ``real``
      - ``double``
      - ``string``

      If omitted, the existing type of the column will not be changed.

      Both type and value are optional, but at least one of the two should be present.

   ``<input.odb>``
      Name of the input ODB-2 file.

   ``<output.odb>``
      Name of the output ODB-2 file.

Example
   .. code-block:: shell

      odc mdset "expver:INTEGER=0008" data-1.odb data-1-new.odb

      000 2021-05-11 14:40:22 (I) MDSetTool::parseUpdateList: expver : INTEGER = '0008'
      000 2021-05-11 14:40:22 (I) expver: name: expver, type: INTEGER, codec: constant, value=1.000000, hasMissing=false
      000 2021-05-11 14:40:22 (I) MDSetTool::run: SAME ORDER 140


``merge``
---------

Merges rows from ODB-2 files.

Each of the ODB-2 files being merged must have unique columns.

Usage
   .. code-block:: shell

      odc merge -o <output-file.odb> <input1.odb> <input2.odb> ...

   Or:

   .. code-block:: shell

      odc merge -S -o <output-file.odb> <input1.odb> <sql-select1> <input2.odb> <sql-select2> ...

Options
   ``-S``
      Apply SQL-like query before merging.

   ``-o <output-file.odb>``
      Name of the ODB-2 output file.

   ``<input1.odb> <input2.odb> ...``
      Names of the input ODB-2 files.

Example
   .. code-block:: shell

      odc merge -o data-merged.odb data-1.odb data-2.odb data-3.odb

      000 2021-05-11 14:47:45 (I) Merging files 'data-1.odb,data-2.odb,data-3.odb,' into 'data-merged.odb': 0.000996 second elapsed, 0.000887 second cpu


``set``
-------

Creates a new file setting columns to given values.

Usage
   .. code-block:: shell

      odc set <update-list> <input.odb> <output.odb>

Options
   ``<update-list>``
      A comma separated list of expressions of the form:

      .. code-block:: shell

         <column-name> = <value>

   ``<input.odb>``
      Name of the input ODB-2 file.

   ``<output.odb>``
      Name of the output ODB-2 file.

Example
   .. code-block:: shell

      odc set "expver=0008" data-1.odb data-1-new.odb

      000 2021-05-11 14:52:06 (I) SetTool::parseUpdateList: expver='0008'


``split``
---------

Splits file according to given template.

Input file will be split along the same values of the column names which appear in the `output template`_ option.

Usage
   .. code-block:: shell

      odc split [-no_verification] [-maxopenfiles <N>] <input.odb> <output_template.odb>

Options
   ``-no_verification``
      Skip the verification of split files for data consistency.

   ``-maxopenfiles <N>``
      Maximum number of open files at one time.

   ``<input.odb>``
      Name of the input ODB-2 file.

   .. _`output template`:

   ``<output_template.odb>``
      Output template for the split command. Fields can be referenced by the following format:

      .. code-block:: shell

         {<column-name>}

      Multiple fields can be referenced, and the input file will be split along all of their values. Any string outside of curly braces will be used as-is in the final file name.

Example
   .. code-block:: shell

      odc split data-1.odb data-1.{expver}.{statid}.odb

      000 2021-05-12 08:25:54 (I) Verifying split...
      000 2021-05-12 08:25:54 (I) Number of rows: 10. Total number of differences: 0
      000 2021-05-12 08:25:54 (I) Split verification: 0.006566 second elapsed, 0.001783 second cpu


``sql``
-------

Executes SQL statement.

Usage
   .. code-block:: shell

      odc sql [-T] [-offset <offset>] [-length <length>] [-N] [-i <inputfile>] [-o <outputfile>] [-f default|wide|odb] [-delimiter <delim>] [--binary|--bin] [--no_alignment] [--full_precision] <select-statement> | <script-filename>

Options
   ``-T``
      Disables printing of column names.

   ``-offset <offset>``
      Start processing file at a given offset.

   ``-length <length>``
      Process only given bytes of data.

   ``-N``
      Do not write NULLs, but proper missing data values.

   ``-i <inputfile>``
      ODB-2 input file.

   ``-o <outputfile>``
      ODB-2 output file.

   ``-f default|wide|odb``
      ODB-2 output format:

      - ``default`` is ASCII
      - ``wide`` is ASCII with bitfields definitions in header
      - ``odb`` is binary ODB-2

   ``-delimiter <delim>``
      Changes the default values’ delimiter (``TAB`` by default). ``delim`` can be any character or string.

   ``--binary|--bin``
      Print bitfields in binary notation.

   ``--no_alignment``
      Do not align columns.

   ``--full_precision``
      Print with full precision.

   ``<select-statement>``
      SQL select statement to execute.

   ``<script-filename>``
      File that contains the SQL select statement.

Example
   .. code-block:: shell

      odc sql -i data-1.odb --no_alignment --full_precision "SELECT obsvalue@body;"

      obsvalue@body
      0.00000000000000000
      12.34560012817382812
      24.69120025634765625
      37.03680038452148438
      49.38240051269531250
      61.72800064086914062
      74.07360076904296875
      86.41919708251953125
      98.76480102539062500
      111.11039733886718750


``xyv``
-------

Creates XYV representation of file for displaying in a graphics program.

Usage
   .. code-block:: shell

      odc xyv <input.odb> <value-column> <output.odb>

Options
   ``<input.odb>``
      Name of the input ODB-2 file.

   ``<value-column>``
      Name of the value column.

   ``<output.odb>``
      Name of the output ODB-2 file.

Example
   .. code-block:: shell

      odc xyv data.odb "obsvalue@body" data-xyv.odb

      000 2021-05-12 08:29:54 (I) select lat, lon, obsvalue@body from "data.odb";
