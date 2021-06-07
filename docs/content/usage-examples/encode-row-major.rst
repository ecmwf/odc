Row-major Encode
================

Shown below is a simple program that encodes sample data from a row-major memory layout and outputs an ODB-2 file.


.. note::

   C++ interface does not support data encoding from a row-major layout. In this case, recommended API is C. Alternatively, you can encode from a :ref:`custom memory layout <odc-encode-custom>` instead.


.. tabs::

   .. group-tab:: C

      .. literalinclude:: ../../../tests/c_api/odc_encode_row_major.c
         :language: c
         :class: copybutton


      To use this sample program, invoke it from the command line with a path to an ODB-2 output file:

      .. code-block:: none

         ./odc-c-encode-row-major example.odb

         Written 20 rows to example.odb


      .. code-block:: none

         odc ls example.odb

         expver	date@hdr	statid@hdr	wigos@hdr		obsvalue@body	integer_missing	double_missing	bitfield_column
         'xxxx'	20210524	'stat00'	'0-12345-0-67800'	0.000000	1234		12.340000	1
         'xxxx'	20210524	'stat01'	'0-12345-0-67801'	12.345600	4321		43.209999	11
         'xxxx'	20210524	'stat02'	'0-12345-0-67802'	24.691200	.		.		107
         'xxxx'	20210524	'stat03'	'0-12345-0-67803'	37.036800	1234		12.340000	1
         'xxxx'	20210524	'stat04'	'0-12345-0-67804'	49.382401	4321		43.209999	11
         'xxxx'	20210524	'stat05'	'0-12345-0-67805'	61.728001	.		.		107
         'xxxx'	20210524	'stat06'	'0-12345-0-67806'	74.073601	1234		12.340000	1
         'xxxx'	20210524	'stat07'	'0-12345-0-67807'	86.419197	4321		43.209999	11
         'xxxx'	20210524	'stat08'	'0-12345-0-67808'	98.764801	.		.		107
         'xxxx'	20210524	'stat09'	'0-12345-0-67809'	111.110397	1234		12.340000	1
         'xxxx'	20210524	'stat10'	'0-12345-0-67810'	123.456001	4321		43.209999	11
         'xxxx'	20210524	'stat11'	'0-12345-0-67811'	135.801605	.		.		107
         'xxxx'	20210524	'stat12'	'0-12345-0-67812'	148.147202	1234		12.340000	1
         'xxxx'	20210524	'stat13'	'0-12345-0-67813'	160.492798	4321		43.209999	11
         'xxxx'	20210524	'stat14'	'0-12345-0-67814'	172.838394	.		.		107
         'xxxx'	20210524	'stat15'	'0-12345-0-67815'	185.184006	1234		12.340000	1
         'xxxx'	20210524	'stat16'	'0-12345-0-67816'	197.529602	4321		43.209999	11
         'xxxx'	20210524	'stat17'	'0-12345-0-67817'	209.875198	.		.		107
         'xxxx'	20210524	'stat18'	'0-12345-0-67818'	222.220795	1234		12.340000	1
         'xxxx'	20210524	'stat19'	'0-12345-0-67819'	234.566406	4321		43.209999	11
         000 2021-05-24 14:06:05 (I) Selected 20 row(s).


   .. group-tab:: Fortran

      .. literalinclude:: ../../../tests/f_api/odc_encode_row_major.f90
         :language: fortran
         :class: copybutton


      To use this sample program, invoke it from the command line with a path to an ODB-2 output file:

      .. code-block:: none

         ./odc-fortran-encode-row-major example.odb

         Written 20 rows to example.odb


      .. code-block:: none

         odc ls example.odb

         expver	date@hdr	statid@hdr	wigos@hdr		obsvalue@body	integer_missing	double_missing	bitfield_column
         'xxxx'	20210524	'stat00'	'0-12345-0-67800'	0.000000	1234		12.340000	1
         'xxxx'	20210524	'stat01'	'0-12345-0-67801'	12.345600	4321		43.209999	11
         'xxxx'	20210524	'stat02'	'0-12345-0-67802'	24.691200	.		.		107
         'xxxx'	20210524	'stat03'	'0-12345-0-67803'	37.036800	1234		12.340000	1
         'xxxx'	20210524	'stat04'	'0-12345-0-67804'	49.382401	4321		43.209999	11
         'xxxx'	20210524	'stat05'	'0-12345-0-67805'	61.728001	.		.		107
         'xxxx'	20210524	'stat06'	'0-12345-0-67806'	74.073601	1234		12.340000	1
         'xxxx'	20210524	'stat07'	'0-12345-0-67807'	86.419205	4321		43.209999	11
         'xxxx'	20210524	'stat08'	'0-12345-0-67808'	98.764801	.		.		107
         'xxxx'	20210524	'stat09'	'0-12345-0-67809'	111.110397	1234		12.340000	1
         'xxxx'	20210524	'stat10'	'0-12345-0-67810'	123.456001	4321		43.209999	11
         'xxxx'	20210524	'stat11'	'0-12345-0-67811'	135.801605	.		.		107
         'xxxx'	20210524	'stat12'	'0-12345-0-67812'	148.147202	1234		12.340000	1
         'xxxx'	20210524	'stat13'	'0-12345-0-67813'	160.492798	4321		43.209999	11
         'xxxx'	20210524	'stat14'	'0-12345-0-67814'	172.838409	.		.		107
         'xxxx'	20210524	'stat15'	'0-12345-0-67815'	185.184006	1234		12.340000	1
         'xxxx'	20210524	'stat16'	'0-12345-0-67816'	197.529602	4321		43.209999	11
         'xxxx'	20210524	'stat17'	'0-12345-0-67817'	209.875198	.		.		107
         'xxxx'	20210524	'stat18'	'0-12345-0-67818'	222.220795	1234		12.340000	1
         'xxxx'	20210524	'stat19'	'0-12345-0-67819'	234.566406	4321		43.209999	11
         000 2021-05-24 14:38:35 (I) Selected 20 row(s).
