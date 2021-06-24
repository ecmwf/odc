.. index:: Introduction

Introduction
============

ODB-2 is a compact data format for the storage, transmission and archival of tabular meteorological observation data. ODB-2 data streams are comprised of independent, self-describing messages. Each of these messages contains a number of rows of data sharing the same columnar format.

**odc** provides C, C++ and Fortran libraries for encoding and decoding ODB-2 data. It also provides an interface the data and metadata without decoding it, and a collection of command line tools for handling and manipulating ODB-2 data.


Observation Data
----------------

ODB-2 supports encoding of observation data from a range of different scientific instruments and sources. The data is in a tabular format, with the types and data sizes of each column of data normally defined by an appropriate external schema. Each cell in the table may contain a value, or be marked as missing. Each row in the table corresponds to an observation, and is treated independently of other rows.

A stream of ODB-2 data consists of a sequence of these tables, which may be unrelated to each other in content or structure. These tables may be grouped according to the needs of the data producer. For archival purposes, a subset of the columns will be used for indexing the data - and in these cases the tables should be grouped such that the data in the index columns is constant within a table.

.. csv-table:: An Example of Tabular Data
   :header-rows: 1
   :width: 100%
   :file: ../_static/example-table.csv

This structure matches **pandas** data frames extremely well. Please see the `pyodc`_ python package for a python library handling ODB-2 data.

.. note::

   For information on ODB-2 format governance and schemas, please visit https://apps.ecmwf.int/odbgov.


Data Types
----------

Each ODB-2 column stores a specific type of information, which is defined as the data type.

=============  ===============  ======================
Type Name      Numeric Value    Corresponding API Type
=============  ===============  ======================
``IGNORE``     ``0``            ``NULL``
``INTEGER``    ``1``            ``long`` [#f1]_
``REAL``       ``2``            ``double`` [#f2]_ [#f3]_
``STRING``     ``3``            ``string``
``BITFIELD``   ``4``            ``long`` [#f1]_ [#f3]_
``DOUBLE``     ``5``            ``double`` [#f3]_
=============  ===============  ======================

.. [#f1] 64-bit integral types are used in the API. Please note the section on Integer Handling in the :ref:`First Steps` section.

.. [#f2] The ``REAL`` data type truncates a 64-bit floating point value to a 32-bit floating point value prior to encoding, which results in smaller encoded data at a cost of a loss of precision. Encoding is lossless by default, so the use of the ``REAL`` type must be explicit.

.. [#f3] The data types stored in the encoded data are not identical to the types used in the API. In particular, ``BITFIELD`` data is represented in integral form, and both ``REAL`` and ``DOUBLE`` data is presented as 64-bit doubles in the API even though they have different encoded precision.


.. note::

   The ``REAL``/``DOUBLE`` naming is a legacy of the Fortran history of this data format.


Bitfields
^^^^^^^^^

The ``BITFIELD`` type exists to provide a packed data type for *flags*.

.. figure:: /_static/odb-2-bitfield-flags.svg
   :alt: Bitfield Flags Example

   Bitfield Flags Example


Within a (32bit) integer, bits can be identified and named by their offset. Groups of bits can be named and identified as well as individual bits, anh hence each elements has an offset and a size.


Data Format
-----------

For encoding as ODB-2 data, first large tables will be split into a sequence of smaller chunks. The columns within these chunks are then sorted and compressed to form *Frames* which comprise the ODB-2 messages. These frames are self-contained and self-describing.

.. figure:: /_static/odb-2-message-stream.svg
   :alt: ODB-2 Data Structure

   ODB-2 Data Structure



The frames can be concatenated in any order to form a valid stream of ODB-2 data, even if the encoded tables do not have the same structure, and are therefore incompatible. This capability suits the needs of data archival, as large amount of data can be packed, indexed externally, and since the data is self-describing, it can be validated against the index.

The data stream ODB-2 data need not be stored in files - it is used equally as an in-memory format, and for transmitting collections of observation data over the network. As such, ODB-2 is considered to be a message format rather than a file format.

.. _`pyodc`: https://pyodc.readthedocs.io
