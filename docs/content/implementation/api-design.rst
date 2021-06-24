.. index:: API Design

API Design
==========


.. index:: API Design; C Interface

.. _`c-interface`:

C Interface
-----------

The C API interface is designed to have a consistent approach to function call design, and to error and argument handling.


Calling Convention
~~~~~~~~~~~~~~~~~~

The library is built around a collection of objects which handle functionality. As C does not expose objects directly, the API uses pointers to opaque types (such as ``odc_frame_t``). Objects are allocated internally and these pointers provide handles to the objects. Methods called against these objects are presented as C functions of the form ``odc_<object-type>_<method-name>(<opaque-handle>, ...)``.

All functions return an integer return code, with the sole exception of ``odc_error_string`` which obtains the details of a previous error. This is beneficial for :ref:`error-handling` as it makes it possible to wrap every function in the same way.

A side effect of this design choice is that all inputs and outputs are passed as arguments to functions. All output values are passed as pointers, and many functions accept a null pointers for optional output values.

.. code-block:: c

   long row_count;

   int rc = odc_frame_row_count(frame, &row_count);


.. _`error-handling`:

Error Handling
~~~~~~~~~~~~~~

All functions return a status code, which should be checked. In case of error a human readable message can be obtained using ``odc_error_string``.

The return code is always one of the following:

``ODC_SUCCESS``
   The function completed successfully.

``ODC_ITERATION_COMPLETE``
   All frames have been returned, and the loop can be terminated successfully.

``ODC_ERROR_GENERAL_EXCEPTION``
   A known error was encountered. Call ``odc_error_string()`` with the returned code for details.

``ODC_ERROR_UNKNOWN_EXCEPTION``
   An unexpected and unrecognised error was encountered, call ``odc_error_string()`` with the returned code for details.


.. code-block:: c

   int rc = odc_new_frame(&frame, reader);

   if (rc != ODC_SUCCESS) {
       // Error, retrieve message and print it.
       fprintf(stderr, "Failed to construct frame: %s\n", odc_error_string(rc));
   }
   else {
       // Success, continue processing.
   }


.. note::

   Internally, **odc** is written in C++ and the error handling uses based on exceptions. All exceptions will be caught on the C/C++ boundary in the C API and an appropriate error code will be returned.


To facilitate consistent error handling, it may be useful to define wrapper functions or macros to handle the error checking. As a trivial example,

.. code-block:: c

   #define CHECK_RESULT(x) \
       do { \
           int rc = (x); \
           if (rc != ODC_SUCCESS) { \
               fprintf(stderr, "Error calling odc function \"%s\": %s\n", #x, odc_error_string(rc)); \
               exit(1); \
           } \
       } while (false); \

   long row_count;
   int column_count;

   CHECK_RESULT(odc_frame_row_count(frame, &row_count));
   CHECK_RESULT(odc_frame_column_count(frame, &column_count));


Failure Handler
~~~~~~~~~~~~~~~

In certain scenarios, it might be more appropriate to have a callback on error. Instead of checking return code after each call, a handler function can be set that will be called back after an error has occurred.

This approach is very useful when a specific clean-up procedure is needed, before current process is aborted.

.. code-block:: c

   void handle_failure(void* context, int error_code) {
       fprintf(stderr, "Error: %s\n", odc_error_string(error_code));
       clean_up();
       exit(1);
   }

   odc_set_failure_handler(handle_failure, NULL);


The ``context`` parameter is user-specified, and is defined as the second argument to ``odc_set_failure_handler``.


.. index:: API Design; Fortran Interface

Fortran Interface
-----------------

The Fortran interface wraps the :ref:`C functions <c-interface>`, with a number of practical differences.


Calling Convention
~~~~~~~~~~~~~~~~~~

Unlike C, Fortran supports custom types. As such, the objects referenced in the API are presented as fortran objects with the appropriate types. The appropriate function calls are thus methods on these type instances.

All functions return a status code that should be checked for error conditions. The standard Fortran mechanism is used to suport optional arguments.

.. code-block:: fortran

   type(odc_reader) :: reader
   type(odc_frame) :: frame
   logical, parameter :: aggregated = .true.
   integer(8), parameter :: max_aggregated_rows = 1000000
   integer(8), target :: row_count

   rc = frame%initialise(reader)
   rc = frame%next(aggregated, max_aggregated_rows)
   rc = frame%row_count(row_count)


Error Handling
~~~~~~~~~~~~~~

All functions return a status code, which should be checked. In case of error a human readable message can be obtained using ``odc_error_string``.

The return code is always one of the following:

``ODC_SUCCESS``
   The function completed successfully.

``ODC_ITERATION_COMPLETE``
   All frames have been returned, and the loop can be terminated successfully.

``ODC_ERROR_GENERAL_EXCEPTION``
   A known error was encountered. Call ``odc_error_string()`` with the returned code for details.

``ODC_ERROR_UNKNOWN_EXCEPTION``
   An unexpected and unrecognised error was encountered, call ``odc_error_string()`` with the returned code for details.


.. code-block:: fortran

   rc = frame%initialise(reader)

   if (rc /= ODC_SUCCESS) then
       ! Error, retrieve message and print it.
       print *, "Failed to construct frame: ", odc_error_string(rc)
   else
       ! Success, continue processing.
   end if


To facilitate consistent error handling, it may be useful to define a wrapper function for checking the return codes in a consistent manner.

.. code-block:: fortran

   integer(8), target :: row_count
   integer, target :: column_count

   call check_call(frame%row_count(row_count))
   call check_call(frame%column_count(column_count))


.. code-block:: fortran

   subroutine check_call(rc)
       integer, intent(in) :: rc

       if (rc /= ODC_SUCCESS) then
           print *, "Error: ", odc_error_string(err)
           stop 1
       end if
   end subroutine


Failure Handler
~~~~~~~~~~~~~~~

In certain scenarios, it might be more appropriate to have a callback on error. Instead of checking return code after each call, a handler function can be set that will be called back after an error has occurred.

This approach is very useful when a specific clean-up procedure is needed, before current process is aborted.

.. code-block:: fortran

   integer(8), parameter :: context = 123456
   rc = odc_set_failure_handler(error_handler, context)


.. code-block:: fortran

   subroutine error_handler(context, error)
         integer(8), intent(in) :: context
         integer, intent(in) :: error

         print *, "Custom error handler"
         print *, "Error: ", odc_error_string(error)
         print *, "Context: ", context
         stop 1
   end subroutine


The ``context`` parameter is under user control, and is defined sa the second argument to ``odc_set_failure_handler``.


Optional Parameters
~~~~~~~~~~~~~~~~~~~

Many API functions take optional parameters, especially for returning (selected) attributes about **Frames** or other objects. These parameters may be omitted as indicated in :doc:`the API Reference </content/reference/f90-reference>`.

The two calls below can be considered identical.

.. code-block:: fortran

   logical, parameter :: aggregated = .true.
   integer(8), parameter :: max_aggregated_rows = 1000000

   err = frame%next(aggregated, max_aggregated_rows)

   ! since aggregated defaults to true anyway, we can skip it and define only maximum_rows
   err = frame%next(maximum_rows=max_aggregated_rows)


.. index:: API Design; C++ Interface
   :name: cpp-interface

C++ Interface
-------------

The interface in C++ mainly exists as an underlying base for implementing :ref:`the C API <c-interface>` which wraps it. It is only suitable to be used within an environment in which `eckit`_ is being used. If this is not the case it’s recommended use the C API.

All C++ functions will throw an exception in case of error.


.. _`eckit`: https://github.com/ecmwf/eckit
