.. index:: API Design

API Design
==========


.. index:: API Design; C Interface

.. _`c-interface`:

C Interface
-----------

The interface in C is consistent, and implements a couple of concepts.


Calling Convention
~~~~~~~~~~~~~~~~~~

All functions always return an integer return code. This is beneficial for :ref:`error-handling`, since it’s possible to wrap every function in the same way.

Side-effect of this design choice is that input and output values are always passed as arguments in all functions. All output parameters are passes as pointers, and many functions take a null pointer if output value is optional.

.. code-block:: c

   long row_count;

   odc_frame_row_count(frame, &row_count);


.. _`error-handling`:

Error Handling
~~~~~~~~~~~~~~

For effective error handling, return value of a function call should be checked. In case of an error, additional function can be called to get a human-readable error message.

The return code is always one of the following:

``ODC_SUCCESS``
   The function call was a success, and processing can continue.

``ODC_ITERATION_COMPLETE``
   All frames have been processed, the loop can be terminated.

``ODC_ERROR_GENERAL_EXCEPTION``
   A general error was encountered, call ``odc_error_string()`` with the returned code for details.

``ODC_ERROR_UNKNOWN_EXCEPTION``
   An unknown error was encountered, call ``odc_error_string()`` with the returned code for details.


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

   Internally, **odc** is written in C++ and the error handling is based on exceptions. However, in the C interface, all exceptions will be caught, and an appropriate error code will be returned.


To facilitate consistent error handling, it may be useful to define macro functions, as they can be effectively reused.

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

In certain scenarios, it might be more appropriate to have a callback on error. Instead of checking return code every time, a handler function can be set that will be called back in case of an error.

This approach is very useful when a specific clean-up procedure is needed, before current process is aborted.

.. code-block:: c

   void handle_failure(void* context, int error_code) {
       fprintf(stderr, "Error: %s\n", odc_error_string(error_code));
       clean_up();
       exit(1);
   }

   odc_set_failure_handler(handle_failure, NULL);


Parameter ``context`` is under user control, and can be defined at the time the failure handler is being set, as a second argument.


.. index:: API Design; Fortran Interface

Fortran Interface
-----------------

The interface in Fortran wraps around :ref:`C functions <c-interface>`, however there are a couple of notable differences.


Calling Convention
~~~~~~~~~~~~~~~~~~

All functions always return an integer return code.

Most of the Fortran interface is implemented as custom data types. Methods should be called on initialised type instances.

Where possible, output values should be declared as ``TARGET`` in Fortran. Coincidentally, all constant parameters should make use of ``PARAMETER`` keyword.

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

Return value of a function call should always be checked. In case of an error, additional function can be called to get a human-readable error message.

The return code is always one of the following:

``ODC_SUCCESS``
   The function call was a success, and processing can continue.

``ODC_ITERATION_COMPLETE``
   All frames have been processed, the loop can be terminated.

``ODC_ERROR_GENERAL_EXCEPTION``
   A general error was encountered, call ``odc_error_string()`` with the returned code for details.

``ODC_ERROR_UNKNOWN_EXCEPTION``
   An unknown error was encountered, call ``odc_error_string()`` with the returned code for details.


.. code-block:: fortran

   rc = frame%initialise(reader)

   if (rc /= ODC_SUCCESS) then
       ! Error, retrieve message and print it.
       print *, "Failed to construct frame: ", odc_error_string(rc)
   else
       ! Success, continue processing.
   end if


It may be useful to define a wrapper function for checking the return codes in a consistent manner.

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

Setting callbacks on errors is also supported. Instead of checking the return code every time, a handler function can be set that will be called back in case of an error.

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


Parameter ``context`` is under user control, and can be defined at the time the failure handler is being set, as a second argument.


Optional Parameters
~~~~~~~~~~~~~~~~~~~

Many API functions have optional parameters, especially for returning (selected) attributes about **Frames** or other objects. These parameters may be omitted as indicated in :doc:`the API Reference </content/reference/f90-reference>`.

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

The interface in C++ mainly exists as an underlying base of :ref:`the C API <c-interface>`, which wraps around it. Therefore, it is only suited to be used within an `eckit`_ environment, and if this is not the case it’s recommended to switch to C.

All C++ functions will throw an exception in case an error is encountered. The API can be used under normal C++ usage practices.


.. _`eckit`: https://github.com/ecmwf/eckit
