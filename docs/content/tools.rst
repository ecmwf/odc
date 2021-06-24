Tools
=====

**odc** provides several tools for manipulating and inspecting ODB-2 data on the command line.

.. note::

   All example commands on this page can be run against the following files:

      * :download:`data-1.csv </_static/data-1.csv>`
      * :download:`data-1.odb </_static/data-1.odb>`
      * :download:`data-2.odb </_static/data-2.odb>`
      * :download:`data-3.odb </_static/data-3.odb>`

   The examples assume the binary directory of **odc** installation is in your ``$PATH``.


``odc``
-------

All commands and their help pages are available under this umbrella command.

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

Reads the ODB-2 data found in one file and re-encodes it into an output file. If the data in the first file is made of many small frames, this can result in significantly reduced file size.

Usage
   .. code-block:: shell

      odc compact <input.odb> <output.odb>

Options
   ``<input.odb>``
      Input ODB-2 file path.

   ``<output.odb>``
      Output ODB-2 file path.

Example
   .. code-block:: shell

      odc compact data.odb data-compacted.odb

      000 2021-05-11 14:17:13 (I) Verifying.
      000 2021-05-11 14:17:13 (I) Comparator::compare: (1) data.odb to (2) data-compacted.odb


``compare``
-----------

Compares two ODB files. Fails with a non-zero return code if the two files differ.

Usage
   .. code-block:: shell

      odc compare [-excludeColumns <list-of-columns>] [-excludeColumnsTypes <list-of-column-types>] [-dontCheckMissing] <file1.odb> <file2.odb>

Options
   ``-excludeColumns <list-of-columns>``
      Exclude specified columns from the comparison. Columns are specified in a comma-separated list.

   ``-excludeColumnsTypes <list-of-column-types>``
      Exclude data of specified type(s) from the comparison. Types are specified in a comma-separated list.

   ``-dontCheckMissing``
      Do not compare missing values.

   ``<file1.odb>``
      Path to first ODB-2 file.

   ``<file2.odb>``
      Path to second ODB-2 file.

Example
   .. code-block:: shell

      odc compare -excludeColumns "wigos@hdr,integer_missing,double_missing,bitfield_column" data-1.odb data-3.odb

      000 2021-06-24 15:21:16 (I) excludedColumns:[wigos@hdr,integer_missing,double_missing,bitfield_column,]
      000 2021-06-24 15:21:16 (I) Comparator::compare: (1) data-1.odb to (2) data-3.odb
      000 2021-06-24 15:21:16 (E) Exception: Values different in column expver: 1 is not equal 2
      000 2021-06-24 15:21:16 (E)
      000 2021-06-24 15:21:16 (I) While comparing rows number 1, columns 0 found different.
      000 2021-06-24 15:21:16 (I)  Values different in column expver: 1 is not equal 2

      000 2021-06-24 15:21:16 (I)  data1[0] = 1.000000e+00
      000 2021-06-24 15:21:16 (I)  data2[0] = 2.000000e+00
      000 2021-06-24 15:21:16 (I)  md1[0] = name: expver, type: INTEGER, codec: constant, value=1.000000, hasMissing=false
      000 2021-06-24 15:21:16 (I)  md2[0] = name: expver, type: INTEGER, codec: constant, value=2.000000, hasMissing=false
      000 2021-06-24 15:21:16 (E) Exception: Files differ.
      000 2021-06-24 15:21:16 (I) Comparing files data-1.odb and data-3.odb: 0.001502 second elapsed, 0.001261 second cpu
      000 2021-06-24 15:21:16 (E) ** Files differ.  Caught in  (eckit/runtime/Tool.cc +31 start)
      000 2021-06-24 15:21:16 (E) ** Exception terminates odc


``count``
---------

Determine the number of rows of data in ODB-2 file(s).

Usage
   .. code-block:: shell

      odc count <file1.odb> <file2.odb> ...

Options
   ``<input1.odb> <input2.odb> ...``
      Path to the input ODB-2 file(s). In case of multiple files, the result will be sum of all rows.

Example
   .. code-block:: shell

      odc count data-1.odb data-2.odb

      20


``fixrowsize``
--------------

Converts file to a format with fixed size rows.

Usage
   .. code-block:: shell

      odc fixrowsize <input.odb> <output.odb>

Options
   ``<input.odb>``
      Input ODB-2 file path.

   ``<output.odb>``
      Output ODB-2 file path.

Example
   .. code-block:: shell

      odc fixrowsize data.odb data-fixed.odb

      000 2021-06-17 13:59:45 (I) Verifying.
      000 2021-06-17 13:59:45 (I) Comparator::compare: (1) data.odb to (2) data-fixed.odb


.. _`odc-header`:

``header``
----------

For each frame in a specified ODB-2 file display metadata associated with the frames and the columnar structure of the data.

Usage
   .. code-block:: shell

      odc header [-offsets] [-ddl] [-table <table-name-in-the-generated-ddl>] <file.odb>

Options
   ``-offsets``
      Print only data offsets and sizes within the file. This produces a tabular output with one row per frame in the source file. The columns specify the offset of the frame in the file (in bytes), the size of the frame (in bytes), the size of the frame (in rows) and the number of columns in the frame.

   ``-ddl``
      Print Data Definition Language (DDL) query that describes the data frame.

   ``-table <table-name-in-the-generated-ddl>``
      Specify the name of the table in the DDL query.

   ``<file.odb>``
      Input ODB-2 file path.

Examples
   .. code-block:: shell

      odc header data-1.odb

      Header 1. Begin offset: 0, end offset: 1241, number of rows in block: 10, byteOrder: same
      0. name: expver, type: INTEGER, codec: constant, value=1.000000, hasMissing=false
      1. name: date@hdr, type: INTEGER, codec: constant, value=20210420.000000, hasMissing=false
      2. name: statid@hdr, type: STRING, codec: int8_string, width=8, #words=10
      3. name: wigos@hdr, type: STRING, codec: int8_string, width=16, #words=10
      4. name: obsvalue@body, type: REAL, codec: short_real2, range=<0.000000,111.110397>, hasMissing=false
      5. name: integer_missing, type: INTEGER, codec: int16, range=<0.000000,4321.000000>, hasMissing=false
      6. name: double_missing, type: REAL, codec: short_real2, range=<12.340000,43.209999>, hasMissing=false
      7. name: bitfield_column, type: BITFIELD [flag_a:1;flag_b:2;flag_c:5] , codec: int16, range=<0.000000,999.000000>, hasMissing=false


   .. code-block:: shell

      odc header -offsets data-1.odb

      0 1241 10 8


   .. code-block:: shell

      odc header -ddl -table observations data-1.odb

      CREATE TYPE bitfield_column_at_observations_t AS (flag_a bit1, flag_b bit2, flag_c bit5);
      CREATE TABLE observations AS (
        expver INTEGER,
        date@hdr INTEGER,
        statid@hdr STRING,
        wigos@hdr STRING,
        obsvalue@body REAL,
        integer_missing INTEGER,
        double_missing REAL,
        bitfield_column bitfield_column_at_observations_t,
      ) ON 'data-1.odb';


``import``
----------

Imports data from a text file into ODB-2 format. This typically imports data from a CSV format.

Data column headers must be in the following format:

.. code-block:: none

   NAME:TYPE

For example (using ``,`` as the delimiter):

.. code-block:: none

   col1:INTEGER,col2:REAL,col3:STRING

Usage
   .. code-block:: shell

      odc import [-d delimiter] <file.txt> <file.odb>

Options
   ``-d delimiter``
      Data delimiter, can be a single character (e.g.: ``,``) or ``TAB``. The default delimiter is ``,``.

   ``<file.txt>``
      Path to source text file.

   ``<file.odb>``
      Path of the ODB-2 file to create.

Example
   .. code-block:: shell

      odc import -d , data-1.csv data-1.odb

      000 2021-06-24 15:11:37 (I) ImportTool::run: inFile: data-1.csv, outFile: data-1.odb
      000 2021-06-24 15:11:37 (I) TextReaderIterator::parseHeader: columns: [expver:INTEGER,date@hdr:INTEGER,statid@hdr:STRING,wigos@hdr:STRING,obsvalue@body:REAL,integer_missing:INTEGER,double_missing:REAL,bitfield_column:BITFIELD[flag_a:1;flag_b:2;flag_c:5]]
      000 2021-06-24 15:11:37 (I) TextReaderIterator::parseHeader: delimiter: ','
      000 2021-06-24 15:11:37 (I) TextReaderIterator::parseHeader: header: 'expver:INTEGER,date@hdr:INTEGER,statid@hdr:STRING,wigos@hdr:STRING,obsvalue@body:REAL,integer_missing:INTEGER,double_missing:REAL,bitfield_column:BITFIELD[flag_a:1;flag_b:2;flag_c:5]'
      000 2021-06-24 15:11:37 (I) ImportTool::odbFromCSV: Copied 10 rows.


``ls``
------

Decode and print the contents of an ODB-2 file to stdout or a specified text output file.

Usage
   .. code-block:: shell

      odc ls [-o <file.txt>] <file.odb>

Options
   ``-o <file.txt>``
      Output file path. If omitted, contents will be printed on standard output.

   ``<file.odb>``
      Path to the input ODB-2 file.

Example
   .. code-block:: shell

      odc ls -o data-1.txt data-1.odb

      000 2021-06-18 12:05:22 (I) Selected 10 row(s).


``mdset``
---------

Create a copy of an ODB-2 file with metadata-only values modified, including modifications to the value of constant columns.

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
      Path to the input ODB-2 file.

   ``<output.odb>``
      Path to the output ODB-2 file.

Example
   .. code-block:: shell

      odc mdset "expver:INTEGER=0008" data-1.odb data-1-new.odb

      000 2021-05-11 14:40:22 (I) MDSetTool::parseUpdateList: expver : INTEGER = '0008'
      000 2021-05-11 14:40:22 (I) expver: name: expver, type: INTEGER, codec: constant, value=1.000000, hasMissing=false
      000 2021-05-11 14:40:22 (I) MDSetTool::run: SAME ORDER 140


``merge``
---------

Merges rows from ODB-2 files to form a single ODB-2 file with the union of the columns of the source files.

.. warning::

   Each of the ODB-2 files being merged must have unique columns. In case the same column names occur in multiple files, the command will fail with a non-zero return code.

.. warning::

   Each of the ODB-2 files being merged should have the same number of rows. In case the number of rows differ between the input files, the output file will be truncated to the lowest number of rows.

Usage
   .. code-block:: shell

      odc merge -o <output-file.odb> <input1.odb> <input2.odb> ...

   Or:

   .. code-block:: shell

      odc merge -S -o <output-file.odb> <input1.odb> <sql-select1> <input2.odb> <sql-select2> ...

Options
   ``-S``
      Apply SQL-like query before merging (see :doc:`/content/reference/sql-reference`).

   ``-o <output-file.odb>``
      Path to the ODB-2 output file.

   ``<input1.odb> <input2.odb> ...``
      Path to the input ODB-2 files.

Example
   .. code-block:: shell

      odc merge -o data-merged.odb data-1.odb data-2.odb

      000 2021-06-24 15:17:01 (I) MergeTool::merge: output metadata: 0. name: expver, type: INTEGER, codec: int32, range=<2147483647.000000,2147483647.000000>, hasMissing=false
      000 2021-06-24 15:17:01 (I) 1. name: date@hdr, type: INTEGER, codec: int32, range=<2147483647.000000,2147483647.000000>, hasMissing=false
      000 2021-06-24 15:17:01 (I) 2. name: statid@hdr, type: STRING, codec: chars, width=8, #words=0
      000 2021-06-24 15:17:01 (I) 3. name: wigos@hdr, type: STRING, codec: chars, width=16, #words=0
      000 2021-06-24 15:17:01 (I) 4. name: obsvalue@body, type: REAL, codec: long_real, range=<-2147483647.000000,-2147483647.000000>, hasMissing=false
      000 2021-06-24 15:17:01 (I) 5. name: integer_missing, type: INTEGER, codec: int32, range=<2147483647.000000,2147483647.000000>, hasMissing=false
      000 2021-06-24 15:17:01 (I) 6. name: double_missing, type: REAL, codec: long_real, range=<-2147483647.000000,-2147483647.000000>, hasMissing=false
      000 2021-06-24 15:17:01 (I) 7. name: bitfield_column, type: BITFIELD [flag_a:1;flag_b:2;flag_c:5] , codec: int32, range=<2147483647.000000,2147483647.000000>, hasMissing=false
      000 2021-06-24 15:17:01 (I) 8. name: obsvalue@duplicate, type: REAL, codec: long_real, range=<-2147483647.000000,-2147483647.000000>, hasMissing=false

      000 2021-06-24 15:17:01 (I) Input file number 0 ended.
      000 2021-06-24 15:17:01 (I) Merging files 'data-1.odb,data-2.odb,' into 'data-merged.odb': 0.001114 second elapsed, 0.000868 second cpu


``set``
-------

Copy an ODB-2 file replacing the values associated with specified columns with the (constant) values specified.

Usage
   .. code-block:: shell

      odc set <update-list> <input.odb> <output.odb>

Options
   ``<update-list>``
      A comma separated list of expressions of the form:

      .. code-block:: shell

         <column-name> = <value>

   ``<input.odb>``
      Path to the input ODB-2 file.

   ``<output.odb>``
      Path to the output ODB-2 file.

Example
   .. code-block:: shell

      odc set "expver=0008" data-1.odb data-1-new.odb

      000 2021-05-11 14:52:06 (I) SetTool::parseUpdateList: expver='0008'


``split``
---------

Split the rows from a single ODB-2 file across multiple new files according to the values in specified columns. The naming of the new files and the columns used for the split are according to the filename template specified in the `output template`_ option.

Usage
   .. code-block:: shell

      odc split [-no_verification] [-maxopenfiles <N>] <input.odb> <output_template.odb>

Options
   ``-no_verification``
      Skip the verification of split files for data consistency.

   ``-maxopenfiles <N>``
      Maximum number of open files at one time.

   ``<input.odb>``
      Path to the input ODB-2 file.

   .. _`output template`:

   ``<output_template.odb>``
      Output template for the split command. Fields can be referenced by the following format:

      .. code-block:: shell

         {<column-name>}

      Multiple fields can be referenced, and the input file will be split along all of their values. Any string outside of curly braces will be used as-is in the final file name.

Example
   .. code-block:: shell

      odc split -no_verification data-1.odb data-1.{expver}.{statid}.odb


``sql``
-------

Interrogate ODB-2 data using SQL-like queries.

.. seealso::

   For additional reference and examples, see :doc:`/content/reference/sql-reference`.


Usage
   .. code-block:: shell

      odc sql [-T] [-offset <offset>] [-length <length>] [-N] [-i <inputfile>] [-o <outputfile>] [-f default|wide|ascii|odb] [-delimiter <delim>] [--binary|--bin] [--no_alignment] [--full_precision] <select-statement> | <script-filename>

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
      Path to the ODB-2 input file.

   ``-o <outputfile>``
      Path to the output file to create.

   ``-f default|wide|ascii|odb``
      ODB-2 output format:

      - ``default`` is ``ascii`` on stdout and ``odb`` to file
      - ``wide`` is ASCII formatted with column definitions in header
      - ``ascii`` is ASCII formatted
      - ``odb`` is binary ODB-2. This option is only supported with the ``-o`` argument.

   ``-delimiter <delim>``
      Changes the delimiter used when printing output in a human readable, ``ascii``, format (``TAB`` by default). ``delim`` can be any character or string.

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

      odc sql -i data-1.odb --no_alignment --full_precision "select obsvalue@body"

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

.. warning::

   The ``xyv`` command requires that the input file contains both ``lat`` and ``lon`` columns with appropriate coordinates. In case these columns cannot be found, the command will fail with a non-zero return code.

Usage
   .. code-block:: shell

      odc xyv <input.odb> <value-column> <output.odb>

Options
   ``<input.odb>``
      Path to the input ODB-2 file.

   ``<value-column>``
      Name of the value column.

   ``<output.odb>``
      Path to the output ODB-2 file to create.

Example
   .. code-block:: shell

      odc xyv data.odb "obsvalue@body" data-xyv.odb

      000 2021-05-12 08:29:54 (I) select lat, lon, obsvalue@body from "data.odb";
