.. index:: Reference; SQL

SQL Reference
=============

The query language supported by **odc** is generally a small subset of the `SQL (Structured Query Language)`_, but it has some small extensions, for example `bitfields`_ allow for naming and accessing sequences of bits in a column.

.. note::

   All example commands on this page can be run against the following file:

      * :download:`example.odb </_static/example.odb>`

   The examples assume the binary directory of **odc** installation is in your ``$PATH``.


Built-in Functions
------------------

Scalar Functions
~~~~~~~~~~~~~~~~

Scalar functions return a single value, based on one or more input values.

+------------------------------------------------+-------+-------------------------------------------------------------+
| Function                                       | Arity | Description                                                 |
+================================================+=======+=============================================================+
| ``year(date)``                                 | ``1`` | Returns the year part of ``date``                           |
+------------------------------------------------+-------+-------------------------------------------------------------+
| ``month(date)``                                | ``1`` | Returns the month part of ``date``                          |
+------------------------------------------------+-------+-------------------------------------------------------------+
| ``day(date)``                                  | ``1`` | Returns the day part of ``date``                            |
+------------------------------------------------+-------+-------------------------------------------------------------+
| ``hour(time)``                                 | ``1`` | Returns the hour part of ``time``                           |
+------------------------------------------------+-------+-------------------------------------------------------------+
| ``minute(time)``                               | ``1`` | Returns the minute part of ``time``                         |
+------------------------------------------------+-------+-------------------------------------------------------------+
| ``second(time)``                               | ``1`` | Returns the second part of ``time``                         |
+------------------------------------------------+-------+-------------------------------------------------------------+
| ``timestamp(date, time)``                      | ``2`` | Returns the timestamp of ``date`` and ``time`` values       |
|                                                |       | (format: ``YYYYMMDDHHMMSS``)                                |
+------------------------------------------------+-------+-------------------------------------------------------------+
| ``julian_seconds(date, time)``                 | ``2`` | Returns the number of Julian seconds of ``date`` and        |
|                                                |       | ``time`` values                                             |
+------------------------------------------------+-------+-------------------------------------------------------------+
| ``julian(date, time)`` or ``jd(date, time)``   | ``2`` | Returns the Julian date of ``date`` and ``time`` values     |
| or ``julian_date(date, time)``                 |       |                                                             |
+------------------------------------------------+-------+-------------------------------------------------------------+
| ``tdiff(tgdate, tgtime, andate, antime)``      | ``4`` | Time difference in seconds (``tgdate``, ``tgtime``) are     |
|                                                |       | target times from which the nominal analysis timestamp      |
|                                                |       | (``andate``, ``antime``) is getting subtracted from         |
+------------------------------------------------+-------+-------------------------------------------------------------+
| ``cos(x)``                                     | ``1`` | Cosine (where ``x`` is in degrees)                          |
+------------------------------------------------+-------+-------------------------------------------------------------+
| ``sin(x)``                                     | ``1`` | Sine (where ``x`` is in degrees)                            |
+------------------------------------------------+-------+-------------------------------------------------------------+
| ``tan(x)``                                     | ``1`` | Tangent (where ``x`` is in degrees)                         |
+------------------------------------------------+-------+-------------------------------------------------------------+
| ``acos(x)``                                    | ``1`` | Arc cosine (the result is in degrees)                       |
+------------------------------------------------+-------+-------------------------------------------------------------+
| ``asin(x)``                                    | ``1`` | Arc sine (the result is in degrees)                         |
+------------------------------------------------+-------+-------------------------------------------------------------+
| ``atan(x)``                                    | ``1`` | Arc tangent (the result is in degrees)                      |
+------------------------------------------------+-------+-------------------------------------------------------------+
| ``atan2(x, y)``                                | ``2`` | Arc tangent of two variables (the result in degrees)        |
+------------------------------------------------+-------+-------------------------------------------------------------+
| ``exp(x)``                                     | ``1`` | Exponent                                                    |
+------------------------------------------------+-------+-------------------------------------------------------------+
| ``ldexp_double(x,n)``                          | ``2`` | Returns multiplied floating point value ``x`` by the number |
|                                                |       | 2 raised to ``n`` power (:math:`x*2^n`)                     |
+------------------------------------------------+-------+-------------------------------------------------------------+
| ``cosh(x)``                                    | ``1`` | Hyperbolic cosine                                           |
+------------------------------------------------+-------+-------------------------------------------------------------+
| ``sinh(x)``                                    | ``1`` | Hyperbolic sine                                             |
+------------------------------------------------+-------+-------------------------------------------------------------+
| ``tanh(x)``                                    | ``1`` | Hyperbolic tangent                                          |
+------------------------------------------------+-------+-------------------------------------------------------------+
| ``int(x)`` or ``trunc(x)``                     | ``1`` | Truncate to integer                                         |
+------------------------------------------------+-------+-------------------------------------------------------------+
| ``nint(x)``                                    | ``1`` | Round-up to integer                                         |
+------------------------------------------------+-------+-------------------------------------------------------------+
| ``floor(x)``                                   | ``1`` | Calculate floor value                                       |
+------------------------------------------------+-------+-------------------------------------------------------------+
| ``ceil(x)``                                    | ``1`` | Calculate ceiling value                                     |
+------------------------------------------------+-------+-------------------------------------------------------------+
| ``abs(x)`` or ``fabs(x)``                      | ``1`` | Absolute value                                              |
+------------------------------------------------+-------+-------------------------------------------------------------+
| ``ibits(x, pos, len)``                         | ``3`` | Extract ``len`` bits from ``x`` offset ``pos``              |
+------------------------------------------------+-------+-------------------------------------------------------------+
| ``mod(x, y)``                                  | ``2`` | ``x`` modulo ``y`` with floating point numbers accepted     |
+------------------------------------------------+-------+-------------------------------------------------------------+
| ``pow(x, y)``                                  | ``2`` | ``x`` power of ``y`` (:math:`x^y`)                          |
+------------------------------------------------+-------+-------------------------------------------------------------+
| ``sqrt(x)``                                    | ``1`` | Square root (:math:`\sqrt{x}`)                              |
+------------------------------------------------+-------+-------------------------------------------------------------+
| ``log(x)`` or ``ln(x)``                        | ``1`` | Natural logarithm                                           |
+------------------------------------------------+-------+-------------------------------------------------------------+
| ``log10(x)`` or ``lg(x)``                      | ``1`` | Base-10 logarithm                                           |
+------------------------------------------------+-------+-------------------------------------------------------------+
| ``deg2rad(x)`` or ``radians(x)``               | ``1`` | Convert degrees to radians                                  |
+------------------------------------------------+-------+-------------------------------------------------------------+
| ``rad2deg(x)`` or ``degrees(x)``               | ``1`` | Convert radians to degrees                                  |
+------------------------------------------------+-------+-------------------------------------------------------------+
| ``celsius(x)`` or ``k2c(x)``                   | ``1`` | Convert Kelvin to Celsius                                   |
+------------------------------------------------+-------+-------------------------------------------------------------+
| ``c2k(x)``                                     | ``1`` | Convert Celsius to Kelvin                                   |
+------------------------------------------------+-------+-------------------------------------------------------------+
| ``fahrenheit(x)`` or ``k2f(x)``                | ``1`` | Convert Kelvin to Fahrenheit                                |
+------------------------------------------------+-------+-------------------------------------------------------------+
| ``f2k(x)``                                     | ``1`` | Convert Fahrenheit to Kelvin                                |
+------------------------------------------------+-------+-------------------------------------------------------------+
| ``c2f(x)``                                     | ``1`` | Convert Celsius to Fahrenheit                               |
+------------------------------------------------+-------+-------------------------------------------------------------+
| ``f2c(x)``                                     | ``1`` | Convert Fahrenheit to Celsius                               |
+------------------------------------------------+-------+-------------------------------------------------------------+
| ``circle(x, x0, y, y0, r)``                    | ``4`` | Returns ``1``, if the point (``x``, ``y``) is inside (or    |
|                                                |       | on) the (planar) circle with origin at (``x0``, ``y0``)     |
|                                                |       | and radius ``r``                                            |
+------------------------------------------------+-------+-------------------------------------------------------------+
| ``rad(lat0, lon0, angle, lat, lon)``           | ``4`` | Returns ``1``, if the (``lat``, ``lon``) is inside the      |
|                                                |       | great-circle (of any radius) with origin (``lat0``,         |
|                                                |       | ``lon0``) and view ``angle`` (in degrees)                   |
+------------------------------------------------+-------+-------------------------------------------------------------+
| ``distance(lat1, lon1, lat2, lon2)``           | ``4`` | Distance (expressed in metres) between two positions        |
|                                                |       | (``lat1``, ``lon1``) and (``lat2``, ``lon2``) along a       |
|                                                |       | great-circle of the Earth (``R=6371km``)                    |
+------------------------------------------------+-------+-------------------------------------------------------------+
| ``km(lat1, lon1, lat2, lon2)``                 | ``4`` | Synonym of ``distance`` function, but expressed in          |
|                                                |       | kilometres                                                  |
+------------------------------------------------+-------+-------------------------------------------------------------+
| ``dist(reflat, reflon, refdist_km, lat, lon)`` | ``5`` | Returns 1, if (``lat``, ``lon``) is inside the great-circle |
|                                                |       | of radius ``refdist_km`` (in kilometres) with origin        |
|                                                |       | (``reflat``, ``reflon``)                                    |
+------------------------------------------------+-------+-------------------------------------------------------------+
| ``thin(value,x)``                              | ``2`` | Returns a new vector where only 1 over ``value`` is kept    |
|                                                |       | from the vector ``x``                                       |
+------------------------------------------------+-------+-------------------------------------------------------------+
| ``rownumber()``                                | ``0`` | Returns the current row number, indexed at 1                |
+------------------------------------------------+-------+-------------------------------------------------------------+
| ``nvl(expression, alt_value)``                 | ``2`` | Returns an ``alt_value`` when ``expression`` is null        |
+------------------------------------------------+-------+-------------------------------------------------------------+
| ``speed(u, v)`` or ``ff(u, v)``                | ``2`` | Returns wind speed for ``u`` and ``v`` components           |
+------------------------------------------------+-------+-------------------------------------------------------------+
| ``direction(u, v)`` or ``dd(u, v)`` or         | ``2`` | Returns wind direction for ``u`` and ``v`` components       |
| ``dir(u, v)``                                  |       |                                                             |
+------------------------------------------------+-------+-------------------------------------------------------------+


.. _`aggregate-functions`:

Aggregate Functions
~~~~~~~~~~~~~~~~~~~

Aggregate functions return a single value, calculated from values an expression passed as parameter to the function evaluates to for selected rows.

+---------------------------+-------+----------------------------------------------------------------------------------+
| Function                  | Arity | Description                                                                      |
+===========================+=======+==================================================================================+
| ``dotp(x, y)``            | ``2`` | Compute dot product of ``x`` and ``y``                                           |
+---------------------------+-------+----------------------------------------------------------------------------------+
| ``rms(x)``                | ``1`` | Compute Root Mean Square of ``x``                                                |
+---------------------------+-------+----------------------------------------------------------------------------------+
| ``stdev(x)``              | ``1`` | Compute standard deviation                                                       |
+---------------------------+-------+----------------------------------------------------------------------------------+
| ``norm(x, y)``            | ``2`` | Compute the norm of ``x`` and ``y``                                              |
+---------------------------+-------+----------------------------------------------------------------------------------+
| ``var(x)`` or ``varp(x)`` | ``1`` | Compute variance                                                                 |
+---------------------------+-------+----------------------------------------------------------------------------------+
| ``avg(x)``                | ``1`` | Returns average value                                                            |
+---------------------------+-------+----------------------------------------------------------------------------------+
| ``max(x)``                | ``1`` | Returns largest value                                                            |
+---------------------------+-------+----------------------------------------------------------------------------------+
| ``count(x)``              | ``1`` | Returns number of rows                                                           |
+---------------------------+-------+----------------------------------------------------------------------------------+
| ``min(x)``                | ``1`` | Returns smallest value                                                           |
+---------------------------+-------+----------------------------------------------------------------------------------+
| ``sum(x)``                | ``1`` | Returns sum                                                                      |
+---------------------------+-------+----------------------------------------------------------------------------------+
| ``first(x)``              | ``1`` | Returns value of the given expression evaluated for the first row of the query’s |
|                           |       | result set                                                                       |
+---------------------------+-------+----------------------------------------------------------------------------------+
| ``last(x)``               | ``1`` | Returns value of the given expression evaluated for the last row of the query’s  |
|                           |       | result set                                                                       |
+---------------------------+-------+----------------------------------------------------------------------------------+


Data Types
----------

The columns in ODB API files can currently be one of the following types.

+--------------+----------------------------------------------------+
| Type         | Description                                        |
+==============+====================================================+
| ``REAL``     | Single precision (32bits) floating point number    |
+--------------+----------------------------------------------------+
| ``STRING``   | Text column                                        |
+--------------+----------------------------------------------------+
| ``INTEGER``  | 32 bits signed integer                             |
+--------------+----------------------------------------------------+
| ``BITFIELD`` | For efficient encoding of flags                    |
+--------------+----------------------------------------------------+
| ``DOUBLE``   | Double precision (64 bits) floating point numbers  |
+--------------+----------------------------------------------------+


The actual type of a column can be found using :ref:`odc header <odc-header>` tool.


Bitfields
---------

Bitfields allow for naming and accessing sequences of bits in a column.


Defining with ``CREATE TABLE``
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

When creating a table with a bitfield column using ``CREATE TABLE`` statement, the type of the column needs to be already defined with ``CREATE TYPE``, for example:

.. code-block:: sql

   CREATE TYPE bf AS (f1 bit1, f2 bit2);
   CREATE TABLE foo AS
     (x INTEGER, y DOUBLE, v STRING, status bf)
   ON 'bitfield_example.odb';

In the above example we have declared a bitfield type called ``bf``, consisting of two members: ``f1`` and ``f2``. ``f1`` occupies 1 bit (``bit1``), ``f2`` 2 bits (``bit2``).

Later the type ``bf`` was used to declare column status in the ``CREATE TABLE`` statement.


Referring to Bitfield Members
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Syntax for accessing a member of a bitfield is:

.. code-block:: none

   <column-name>.<bitfield-member-name>


In case it is necessary to specify table name when referring to a column, correct way of referring to a member is:

.. code-block:: none

   <column-name>.<bitfield-member-name>@<table-name>


For example:

.. code-block:: none

   report_status.active@hdr


Expanding List of Members
~~~~~~~~~~~~~~~~~~~~~~~~~

The list of bitfield members can be expanded with the asterisk (``*``) operator.

For example:

.. code-block:: none

   report_status.*@hdr


will be expanded to:

.. code-block:: none

   report_status.active@hdr, report_status.passive@hdr, report_status.rejected@hdr, report_status.blacklisted@hdr, report_status.use_emiskf_only@hdr


Finding Details of Bitfield Definition
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The :ref:`odc header <odc-header>` tool can be used to find out details of bitfield definition: its members and number of bits they occupy:

.. code-block:: shell

   odc header example.odb | grep datum_status | head -n 1

   10. name: datum_status@body, type: BITFIELD [active:1;passive:1;rejected:1;blacklisted:1;use_emiskf_only:1] , codec: int8, range=<1.000000,12.000000>, hasMissing=false

Option ``-dll`` produces output in the Data Definition Language (part of SQL) format:

.. code-block:: shell

   odc header -ddl example.odb | grep datum_status | head -n 1

   CREATE TYPE datum_status@body_at_foo_t AS (active bit1, passive bit1, rejected bit1, blacklisted bit1, use_emiskf_only bit1);


Examples
--------

Unique station identifiers
   Which station identifiers are in the ODB file?

   .. code-block:: shell

      odc sql -i example.odb 'select distinct statid'

      statid@hdr
      '   96413'
      '   27707'
      '   27730'
      '   27962'
      '   27995'
      '   34009'
      '   34172'


Available columns
   What is the list of columns (or keys) available in the file?

   .. code-block:: shell

      odc header example.odb


Geophysical variables
   What geophysical variables are in the ODB file?

   .. code-block:: shell

      odc sql -i example.odb 'select distinct varno'

          varno@body
                   1
                  39
                  40
                  58
                   7
                 111
                 112
                  41
                  42
                   2
                  59
                  29
                   3
                   4


See `ODB Governance`_ for description of numeric values of ``varno``. For example:

+-----------+---------------------------+
| ``varno`` | Description               |
+===========+===========================+
|   ``1``   | Geopotential              |
+-----------+---------------------------+
|   ``2``   | Upper air temperature     |
+-----------+---------------------------+
|   ``3``   | Upper air meridional wind |
+-----------+---------------------------+
|   ``4``   | Upper air zonal wind      |
+-----------+---------------------------+
|   ``7``   | Specific humidity         |
+-----------+---------------------------+
|   ``29``  | Upper air rel. humidity   |
+-----------+---------------------------+
|   ``39``  | 2m temperature            |
+-----------+---------------------------+
|   ``40``  | 2m dew point              |
+-----------+---------------------------+
|   ``41``  | 10m meridional component  |
+-----------+---------------------------+
|   ``42``  | 10m zonal component       |
+-----------+---------------------------+
|   ``58``  | 2m relative humidity      |
+-----------+---------------------------+
|   ``59``  | Upper air dew point       |
+-----------+---------------------------+
|  ``111``  | Wind direction            |
+-----------+---------------------------+
|  ``112``  | Wind speed                |
+-----------+---------------------------+


Number of temperature records
   Count the number of temperature records.

   .. code-block:: shell

      odc sql -i example.odb 'select count(*) where varno=2'

            count(1)
          448.000000


``count(*)`` is an :ref:`aggregation function <aggregate-functions>`. Based on the other keys present in the SQL query (here: filtering to select only the temperature entries), each population of identified entries see the data subjected to the aggregation function.


Number of temperature records per station identifier
   Count the number of temperature records, this time per station identifier, where the observation values are not missing.

   .. code-block:: shell

      odc sql -i example.odb 'select count(*), statid where varno=2 and obsvalue is not null'

            count(1)    statid@hdr
           39.000000    '   27707'
           49.000000    '   27730'
           37.000000    '   27962'
           44.000000    '   27995'
           38.000000    '   34009'
            9.000000    '   34172'
           50.000000    '   96413'


Average temperature at 100 hPa
   Get the observation count at one station and average temperature observation value by pressure level bins of 100 hPa each, showing also the average pressure in each pressure bin.

   .. code-block:: shell

      odc sql -i example.odb 'select count(*), avg(fg_depar), floor(vertco_reference_1/10000.0), avg(vertco_reference_1/100.0) where varno=2 and statid="27707" and fg_depar is not null'

            count(1)     avg(fg_depar)    floor(/(vertco_reference_1,10000))    avg(/(vertco_reference_1,100))
           11.000000          0.505271                                     0                         43.318182
            5.000000         -0.160176                                     1                        120.600000
            5.000000         -0.227211                                     2                        218.400000
            2.000000          1.077009                                     3                        316.500000
            3.000000          0.370587                                     4                        438.666667
            3.000000          0.385093                                     5                        553.666667
            1.000000          0.324625                                     6                        637.000000
            1.000000         -0.013323                                     7                        700.000000
            4.000000          0.635424                                     8                        836.000000
            4.000000          0.118218                                     9                        932.500000


Meridional and zonal wind near 500 hPa
   Get the observation count at one station and mean observation minus first-guess departure for meridional wind and zonal wind near 500 hPa.

   .. code-block:: shell

      odc sql -i example.odb 'select count(*), avg(fg_depar) where varno in (3,4) and statid="27707" and vertco_reference_1>=45000. and vertco_reference_1<55000. and fg_depar is not null'

            count(1)     avg(fg_depar)
            4.000000         -0.238838


.. _`SQL (Structured Query Language)`: https://en.wikipedia.org/wiki/SQL
.. _`ODB Governance`: http://apps.ecmwf.int/odbgov/varno/
