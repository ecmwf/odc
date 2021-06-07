.. index:: Introduction

Introduction
============

ODB-2 is a compact data format suited for storage, transmission and archival of meteorological observation data in a tabular format, with each row corresponding to one observation. Observation is self-describing and can come from many different data sources and many types of instrument.

**odc** provides an encoder and decoder for the ODB-2 data. In addition to that, it also has an interface to explore the metadata without actually decoding ODB-2 data.


ODB-2 Data
----------

ODB-2 supports encoding of observation data from a range of different scientific instruments.

Data is stored in a tabular format, without requirements on row or cell sizes. Each observation is a row in the table and is treated as independent.

Each of the cells has a type constrained by the column they are in, which is defined by an appropriate external schema. A cell can have a value, or it can be missing.

.. csv-table:: An Example of Tabular Data
   :header-rows: 1
   :width: 100%
   :file: ../_static/example-table.csv

The structure is a good fit to **pandas** data frames, which can be used for Python workflows.

.. note::

   For the ODB-2 format governance information, please visit https://apps.ecmwf.int/odbgov.


Data Types
----------

Each ODB-2 column stores a specific type of information, which is defined as the data type.

=============  ===============  ======================
Type Name      Numeric Value    Corresponding API Type
=============  ===============  ======================
``IGNORE``     ``0``            ``NULL``
``INTEGER``    ``1``            ``int``
``REAL``       ``2``            ``float`` [#f1]_
``STRING``     ``3``            ``string``
``BITFIELD``   ``4``            ``int`` [#f1]_
``DOUBLE``     ``5``            ``float`` [#f1]_
=============  ===============  ======================


.. [#f1] The stored data types relate to the data types provided and used by the API, but are not exactly the same. In particular, ``BITFIELD`` decode as ``int``, while ``REAL`` and ``DOUBLE`` values are encoded and decoded to ``float``.


.. note::

   By default, the encoding is lossless, so use of the ``REAL`` type must be explicit.


.. note::

   The ``REAL``/``DOUBLE`` naming is a result of the Fortran history of this data format.


Bitfields
^^^^^^^^^

Type ``BITFIELD`` exists to provide a packed data type for *flags*.

.. figure:: /_static/odb-2-bitfield-flags.svg
   :alt: Bitfield Flags Example

   Bitfield Flags Example


Within a (32bit) integer, the bits can be identified and named by their offset. Groups of bits are identified as well as individual bits, therefore each item has an offset and a size.


Data Format
-----------

In ODB-2 format, tables of data are encoded into *Frames*, or messages. These frames are self-contained and self-describing.

.. figure:: /_static/odb-2-message-stream.svg
   :alt: ODB-2 Data Structure

   ODB-2 Data Structure


The frames can be concatenated in any order to form a valid stream of ODB-2 data, even if the encoded tables do not have the same structure, and are therefore incompatible.

This capability suits the needs of data archival, as large amount of data can be packed, indexed externally, and since the data is self-describing, it can be validated against the index.

The sequence of frames forms a stream of ODB-2 data. As such, the ODB-2 format is a message format and not a file format.
