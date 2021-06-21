.. index:: Reference; File Format

File Format Reference
=====================

Data Stream
-----------

The file format associated with ODB is a compact, tabular streaming format. The format consists of a series of frames (or tables), each of which has a header and is followed by a specified number of rows of data. The data is organised by columns, which are encoded by the codec specified in the header. There is no requirement that multiple frames have the same number, or the same type, of columns.

The data is designed to be straightforward to stream. Two valid ODB files which are concatenated form a valid ODB file.

Each element (either header or row) is preceded by an ``uint16`` marker. This allows distinguishing between rows and headers, and should be consistent with the expected values.

Endianness
~~~~~~~~~~

Within this document, except where otherwise marked, all elements will be recorded in the specified order, but all of the elements within a table may be written either in big-endian or little-endian format. The header contains a flag indicating which endianness has been used, and if it does not match the endianness of the reading machine, then the byte order for each individual element (integer, double, ...) needs to be reversed during reading (except as noted).


Data Types
----------

The following data types are used throughout this document.

===========  =======  ==================================================================================================
Type         Bytes    Description
===========  =======  ==================================================================================================
``float``    ``4``    Floating point number (32-bit)
``double``   ``8``    Floating point number (64-bit)
``uint8``    ``1``    Unsigned integer (8-bit)
``int8``     ``1``    Signed integer (8-bit)
``uint16``   ``2``    Unsigned integer (16-bit)
``int16``    ``2``    Signed integer (16-bit)
``uint32``   ``4``    Unsigned integer (32-bit)
``int32``    ``4``    Signed integer (32-bit)
``uint64``   ``8``    Unsigned integer (64-bit)
``int64``    ``8``    Signed integer (64-bit)
``char[n]``  ``n``    A sequence of ``n`` 8-bit characters, endianness independent
``string``   ``> 4``  A compound type, comprising a ``uint32``, ``n``, which specifies the number of characters followed
                      by a ``char[n]``
===========  =======  ==================================================================================================


Missing Values
~~~~~~~~~~~~~~

The missing value used in the different contexts is different. Unless otherwise indicated, this value can be selected at runtime when encoding, and the value used as the missing value (and correspondingly returned by the codecs) is stored inside the resultant file.

========  ===============
Type      Value
========  ===============
Integral  ``2147483647``
Real      ``-2147483647``
Bitfield  ``0``
========  ===============


Frames
------

Frames are self-describing and know their own length. Each frame starts with an ``uint16`` marker.

Data stream can be split into frames without decoding them.


Fixed Size Header
~~~~~~~~~~~~~~~~~

Fixed size header contains various top-level information about the stream.

===========  =======================  ==================================================================================
Type         Value                    Description
===========  =======================  ==================================================================================
``uint16``   ``0xFFFF``               Header marker
``char[3]``  ``"ODA"``                Magic string, marking the data format, endianness independent
``uint32``   ``1`` or ``0x01000000``  Endianness signifier. Written as the value ``1`` in the endianness used for
                                      outputting the file. Read in in native endianness. If the value is ``1`` the same
                                      endianness is used to read the file as to write it. Otherwise, each element that
                                      is read should have its bytes reversed.
``int32``    ``versionMajor``         The major version number of the ODB API format (not the software), currently ``0``
``int32``    ``versionMinor``         The minor version number of the ODB API format (not the software), currently ``5``
``string``   ``md5``                  The MD5 hash of the data section of the table
``uint32``   ``headerLength``         The number of bytes occupied by the header
``uint64``   ``dataSize``             The number of bytes occupied by the payload (rows)
``uint64``   ``prevFrameOffset``      The offset of the previous table in the ODB file. Appears to be unused, and
                                      always equal to zero.
``uint64``   ``numberOfRows``         The number of rows of data encoded in the table (before EOF or the next header)
===========  =======================  ==================================================================================


Variable Header
~~~~~~~~~~~~~~~

Variable header describes column structure and contains codec-specific information. Flags and properties can be used to store additional meta data. String tables are part of codec-specific information.

Columns
^^^^^^^

The columns determine the types of the data following in the table, and the codecs that are used to encode it.

=============  =====================  ==================================================
Type           Value                  Description
=============  =====================  ==================================================
``int32``      ``numberOfColumns``    The number of columns that will be encoded
-------------  ---------------------  --------------------------------------------------
``numberOfColumns x``
----------------------------------------------------------------------------------------
``string``     ``columnName``         The name of the column
``int32``      ``type``               The externally visible data type for the column
``string``     ``codecName``          The codec that will be used to encode/decode data
                                      to the internal data representation
``int32``      ``numberOfBitfields``  The number of bitfields
``numberOfBitfields x``
----------------------------------------------------------------------------------------
``string``     ``bitfieldName``       The name of the bitfield
``codecData``  *...*                  Specific data for a given codec, see `Codecs`_
=============  =====================  ==================================================


.. _`column-type`:

Column Type
...........

The types referred to in the ``type`` column above correspond to the following.

=====  ============  ================================================
ID     Type          Description
=====  ============  ================================================
``0``  ``IGNORE``    *Not used*
``1``  ``INTEGER``   Any integral data types
``2``  ``REAL``      Floating point <= 32 bits
``3``  ``STRING``    Characters strings
``4``  ``BITFIELD``  A sequence of bits, packaged in an integral type
``5``  ``DOUBLE``    Floating point <= 64 bits
=====  ============  ================================================


Flags
^^^^^

Flags may be stored in the header.

==========  ============  ===============================================================================
Type        Value	        Description
==========  ============  ===============================================================================
``int32``   ``numFlags``  The number of flags that will be encoded
``numFlags x``
---------------------------------------------------------------------------------------------------------
``double``  ``flag``      A sequence of flags, the number of these included is determined by ``numFlags``
==========  ============  ===============================================================================

In production, the ODB API library always encodes 10 flags, all with value zero.


Properties
^^^^^^^^^^

Each property is a pair of key/value strings.

==========  =================  ===============================================================
Type        Value              Description
==========  =================  ===============================================================
``int32``   ``numProperties``  The number of properties that will be encoded
``numProperties x``
----------------------------------------------------------------------------------------------
``string``  ``key``            A sequence of key/value pairs, as required by ``numProperties``
``string``  ``value``
==========  =================  ===============================================================


Data
~~~~

Each row is encoded sequentially in the file. It starts with an ``uint16`` marker, and indicates which column is the first to have changed from the previous row. The marker is followed by the values for the remaining columns, which are sorted in order of rate of change. Since most columns do not change for most rows, *compression* is achieved.


Row Format
^^^^^^^^^^

Only the data associated with columns from the first one that is changed from the previous row are encoded.

================  ===============  =====================================================================================
Type              Value            Description
================  ===============  =====================================================================================
``uint16_t``      ``startColumn``  The first column which is encoded in this row (zero based index). This determines
                                   which is the ``first``
``repeat cols [startColumn to numberOfColumns - 1]``
------------------------------------------------------------------------------------------------------------------------
``[codec data]``  *...*            The data associated with each column depends on the codec assigned to it, see
                                   `Codecs`_
================  ===============  =====================================================================================


Codecs
~~~~~~

All codecs store the same common information in their header block.

==========  ================  =========================================================
Type        Value             Description
==========  ================  =========================================================
``int32``   ``hasMissing``    Non-zero if a missing value has been encoded by the codec
``double``  ``min``           The minimum value encoded for in the table
``double``  ``max``           The maximum value encoded for in the table
``double``  ``missingValue``  The missing value for this codec instance
==========  ================  =========================================================


Some codecs read further data at initialisation time. Otherwise, they consume further data whilst decoding rows.


Constant ``constant`` ``constant_string``
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The values encoded by these codecs are of the following types.

===================  ===========
Value                Type
===================  ===========
``constant``         ``double``
``constant_string``  ``char[8]``
===================  ===========

The value specified as min in the codec header is returned on all decode calls. No missing value is provided.

.. note::

   The constant string value is 8-bytes of characters cast into the space of a double, and is functionally identical to constant, except that it is independent of endianness.


.. note::

   There is no explicitly integral constant codec, but integral types can be specified in the type field of the column to direct casting.


No data is consumed when decoding rows.


Constant or Missing ``constant_or_missing`` ``real_constant_or_missing``
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The values encoded by these codecs are of the following types.

============================  ==========
Value                         Type
============================  ==========
``constant_or_missing``       ``int32``
``real_constant_or_missing``  ``double``
============================  ==========


Each data element is written out as an ``uint8`` value. This has somewhat curious properties on read.

==================  ================
Value               Returned value
==================  ================
``0xff``            ``missingValue``
``0x0`` - ``0xfe``  ``min + value``
==================  ================


.. note::

   As the current decoder always returns double values, it is perfectly possible for ``missingValue`` not to be a valid ``int32`` for ``constant_or_missing`` (which is supposed to be an integer codec).


.. note::

   Despite the name, the output value is not constant, it may vary by up to ``254``.


Character Strings ``chars``
^^^^^^^^^^^^^^^^^^^^^^^^^^^

This codec encodes data of type ``char[8]``.

During initialisation, the codec consumes one additional ``int32``.

=========  =====  ===============================================
Type       Value  Description
=========  =====  ===============================================
``int32``  ``0``  This value is unused, but must be equal to zero
=========  =====  ===============================================


This is an artefact of implementation, with this codec being used as the base codec for the other character decoding codecs, which initialise a flat list of available strings. This codec does not make use of such a list, but must be initialised to have a zero-length list.

The string data encoded by this codec must be comprised of exactly 8-byte long character strings. These can be cast to, and manipulated, as though they were doubles.

The data is transferred to the rows unchanged (8 bytes of data, in the order of characters in the string, endianness independent).


Real Values ``long_real`` ``short_real`` ``short_real2``
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The values encoded by these codecs are of the following types.

===============  ==========
Value            Type
===============  ==========
``long_real``    ``double``
``short_real``   ``float``
``short_real2``  ``float``
===============  ==========


These codecs straightforwardly transfer the supplied values into column data, of width 4 and 8 bytes appropriately.

The existing codecs use double values in the interface, and the ``missingValue`` specified in the header is a ``double``. As a result, from an API perspective the ``missingValue`` can lie outside of the range that can be encoded with a ``float``, even in the ``short_real`` codecs.

The ``short_real`` and ``short_real2`` codecs differ from the ``long_real`` codec, and each other, only by their handling of missing values. For these codecs, the ``missingValue`` in the header is only used for the API interface, and not for the data encoding. A hard-coded, fixed, missing value is used for the data encoding, which is provided after checking if the supplied data matches the set ``missingValue``. On decoding, if the hard-coded value is found, the ``missingValue`` from the header is returned.

The hard coded values are the following.

===============  ================================  =====================================================================
Codec            Integral representation of value  Description
===============  ================================  =====================================================================
``short_real``   ``0x800000``                      This is the smallest possible (closest to zero) non-zero floating
                                                   point number
``short_real2``  ``0xFF7FFFFF``                    This is the lowest possible floating point number
===============  ================================  =====================================================================


Integer Values ``int32`` ``int16`` ``int8`` ``int8_missing`` ``int16_missing``
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The values encoded by these codecs are of the following types.

============================  ==========
Value                         Type
============================  ==========
``int32``                     ``int32``
``int16``, ``int16_missing``  ``uint16``
``int8``, ``int8_missing``    ``uint8``
============================  ==========


There is currently no codec that stores data of 64-bit integral type.

These codecs encode data for which the range of the data is less than or equal to the maximum integer encoded by the specified integral type. The smallest value is stored in the min field in the header, and the value stored in the columnar data is the offset. The ``int32`` codec does not make use of the minimum value, and integers are stored directly.

If ``int8_missing`` or ``int16_missing`` are being used, an internal missing value is used to encode missing values, as the externally visible one is outside of the range of values that can be encoded.

=================  ===================================================================
Codec              Missing value
=================  ===================================================================
``int32``          ``missingValue`` as recorded in the header, normally ``2147483647``
``int16_missing``  ``0xFFFF``
``int16``          No missing values
``int8_missing``   ``0xFF``
``int8``           No missing values
=================  ===================================================================


Character Data ``int8_string`` ``int16_string``
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

These codecs encode all of the strings in the codec-specific part of the *header*, creating a list or a lookup table.

============  ==============  =====================
Type          Value           Description
============  ==============  =====================
``int32``     ``numStrings``  The number of entries
------------  --------------  ---------------------
``numStrings x``
---------------------------------------------------
``int32``     ``length``      The length of string
``[n]*char``  ``charData``    The string data
============  ==============  =====================


In the data section, encoded values are only an 8-bit or 16-bit number as appropriate to index into the list of strings.

================  ==========
Value             Type
================  ==========
``int8_string``   ``uint8``
``int16_string``  ``uint16``
================  ==========
