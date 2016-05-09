cmake_minimum_required(VERSION 2.8)

project( ecml CXX )

include( ecbuild_system NO_POLICY_SCOPE )

ecbuild_requires_macro_version( 1.9 )

ecbuild_declare_project()

ecbuild_find_lexyacc()
ecbuild_use_package( PROJECT eckit VERSION 0.9 REQUIRED )

set( ECML_INCLUDE_DIRS   ${CMAKE_CURRENT_SOURCE_DIR}/src ${CMAKE_CURRENT_BINARY_DIR}/src )
set( ECML_LIBRARIES      ecml )

include_directories( ${ECML_INCLUDE_DIRS} ${ECKIT_INCLUDE_DIRS} )

find_package(OpenMP)
if (OPENMP_FOUND)
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
endif()

list( APPEND ecml_srcs
# Parser
src/ecml/parser/Cell.h
src/ecml/parser/Cell.cc
src/ecml/parser/CellPrinter.h
src/ecml/parser/CellPrinter.cc
src/ecml/parser/CellDotPrinter.h
src/ecml/parser/CellDotPrinter.cc
src/ecml/parser/List.h
src/ecml/parser/List.cc
src/ecml/parser/Request.h
src/ecml/parser/Request.cc
src/ecml/parser/RequestParser.h
src/ecml/parser/RequestParser.cc

# AST
src/ecml/ast/FunctionDefinition.h
src/ecml/ast/FunctionDefinition.cc
src/ecml/ast/Closure.cc
src/ecml/ast/Closure.h

# Core language
src/ecml/core/Environment.h
src/ecml/core/Environment.cc
src/ecml/core/ExecutionContext.h
src/ecml/core/ExecutionContext.cc
src/ecml/core/Interpreter.h
src/ecml/core/Interpreter.cc
src/ecml/core/Module.cc
src/ecml/core/Module.h
src/ecml/core/RequestHandler.h
src/ecml/core/RequestHandler.cc
src/ecml/core/SpecialFormHandler.h
src/ecml/core/SpecialFormHandler.cc

# Prelude
src/ecml/prelude/Prelude.cc
src/ecml/prelude/Prelude.h
src/ecml/prelude/LetHandler.cc
src/ecml/prelude/LetHandler.h
src/ecml/prelude/UpdateHandler.h
src/ecml/prelude/UpdateHandler.cc
src/ecml/prelude/VariableLookupHandler.cc
src/ecml/prelude/VariableLookupHandler.h
src/ecml/prelude/DefineFunctionHandler.cc
src/ecml/prelude/DefineFunctionHandler.h
src/ecml/prelude/ApplyHandler.cc
src/ecml/prelude/ApplyHandler.h
src/ecml/prelude/ClosureHandler.h
src/ecml/prelude/ClosureHandler.cc
src/ecml/prelude/ListHandler.cc
src/ecml/prelude/ListHandler.h
src/ecml/prelude/SequenceHandler.cc
src/ecml/prelude/SequenceHandler.h
src/ecml/prelude/PrintHandler.cc
src/ecml/prelude/PrintHandler.h
src/ecml/prelude/TestHandler.cc
src/ecml/prelude/TestHandler.h
src/ecml/prelude/FirstHandler.cc
src/ecml/prelude/FirstHandler.h
src/ecml/prelude/RestHandler.cc
src/ecml/prelude/RestHandler.h
src/ecml/prelude/IfHandler.cc
src/ecml/prelude/IfHandler.h
src/ecml/prelude/TemporaryFileHandler.h
src/ecml/prelude/TemporaryFileHandler.cc
src/ecml/prelude/SystemHandler.h
src/ecml/prelude/SystemHandler.cc
src/ecml/prelude/GetenvHandler.h
src/ecml/prelude/GetenvHandler.cc
src/ecml/prelude/JoinStringsHandler.h
src/ecml/prelude/JoinStringsHandler.cc
src/ecml/prelude/QuoteHandler.h
src/ecml/prelude/QuoteHandler.cc
src/ecml/prelude/NullHandler.h
src/ecml/prelude/NullHandler.cc
src/ecml/prelude/RunHandler.h
src/ecml/prelude/RunHandler.cc
src/ecml/prelude/REPLHandler.h
src/ecml/prelude/REPLHandler.cc
src/ecml/prelude/Autocompleter.h
src/ecml/prelude/Autocompleter.cc
src/ecml/prelude/RangeHandler.h
src/ecml/prelude/RangeHandler.cc
src/ecml/prelude/ForHandler.cc
src/ecml/prelude/ForHandler.h
src/ecml/prelude/GlobHandler.cc
src/ecml/prelude/GlobHandler.h
src/ecml/prelude/ReadTextFileHandler.h
src/ecml/prelude/ReadTextFileHandler.cc
src/ecml/prelude/TryHandler.h
src/ecml/prelude/TryHandler.cc
src/ecml/prelude/ThrowHandler.h
src/ecml/prelude/ThrowHandler.cc

# Data types' support
src/ecml/data/DataHandleFactory.h
src/ecml/data/DataHandleFactory.cc
src/ecml/data/FileHandleFactory.h
src/ecml/data/FileHandleFactory.cc
src/ecml/data/PartFileHandleFactory.h
src/ecml/data/PartFileHandleFactory.cc

# Miscellaneous
# Adapter for eckit::Parametrisation
src/ecml/misc/DynamicParametrisation.h
src/ecml/misc/DynamicParametrisation.cc
src/ecml/misc/ParameterizedRequestHandler.h
src/ecml/misc/ParameterizedRequestHandler.cc
)

ecbuild_generate_yy( YYPREFIX    request_
                     YACC        src/ecml/parser/requesty
                     YACC_TARGET src/ecml/requesty
                     LEX         src/ecml/parser/requestl
                     LEX_TARGET  src/ecml/requestl
                     DEPENDANT   src/ecml/parser/RequestParser.cc )

ecbuild_add_library( TARGET             ecml
                     INSTALL_HEADERS    ALL
                     HEADER_DESTINATION ${INSTALL_INCLUDE_DIR}
                     SOURCES            ${ecml_srcs}
                     LIBS               eckit 
                                        eckit_cmd # for UserInput
                                        )

ecbuild_add_executable( TARGET ecml_test
                        SOURCES src/ecml/tests/ecml_test.cc
                        LIBS ecml eckit eckit_cmd )

ecbuild_add_executable( TARGET ecml_unittests
                        SOURCES src/ecml/tests/ecml_unittests.cc
                        LIBS ecml eckit eckit_cmd)

add_subdirectory( src/ecml/tests )

configure_file( src/ecml/prelude/prelude.ecml ${CMAKE_BINARY_DIR}/include/prelude.ecml COPYONLY)

install(FILES       ${CMAKE_CURRENT_SOURCE_DIR}/prelude/prelude.ecml
        DESTINATION ${INSTALL_INCLUDE_DIR} )


ecbuild_pkgconfig( NAME ${PROJECT_NAME}
                   DESCRIPTION "ECML language interpreter"
                   URL "https://software.ecmwf.int/wiki/display/ODB/ODB+API"
                   LIBRARIES ecml )

#ecbuild_install_project( NAME ecml )

ecbuild_print_summary()

