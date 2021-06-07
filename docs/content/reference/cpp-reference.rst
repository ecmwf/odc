.. index:: Reference; C++ API
   :name: cpp-reference

C++ Reference
=============

Global Settings
---------------

.. doxygenclass:: odc::api::Settings
   :members:


Values and Types
----------------

.. doxygenenum:: odc::api::ColumnType


.. doxygenstruct:: odc::api::ColumnInfo
   :members:


.. doxygentypedef:: odc::api::StridedData


.. doxygentypedef:: odc::api::ConstStridedData


.. doxygenclass:: odc::api::StridedDataT
   :members:


Reader API
----------

.. doxygenclass:: odc::api::Reader
   :members:


Frame Handling
--------------

.. doxygenclass:: odc::api::Frame
   :members:


.. doxygenclass:: odc::api::Span
   :members:


.. doxygenclass:: odc::api::SpanVisitor
   :members:


Decoding Data
-------------

.. doxygenclass:: odc::api::Decoder
   :members:


Encoding Data
-------------

.. doxygenfunction:: odc::api::encode


Conversion Functions
--------------------

.. doxygenfunction:: odc::api::filter


.. doxygenfunction:: odbFromCSV(const std::string &in, eckit::DataHandle &dh_out, const std::string &delimiter = ",")


.. doxygenfunction:: odbFromCSV(eckit::DataHandle &dh_in, eckit::DataHandle &dh_out, const std::string &delimiter = ",")


.. doxygenfunction:: odbFromCSV(std::istream &is, eckit::DataHandle &dh_out, const std::string &delimiter = ",")
