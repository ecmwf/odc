
set( ECML_INCLUDE_DIRS   ${CMAKE_CURRENT_SOURCE_DIR}/.. ${CMAKE_CURRENT_BINARY_DIR}/.. )
set( ECML_LIBRARIES      ecml )

include_directories( ${ECML_INCLUDE_DIRS} ${ECKIT_INCLUDE_DIRS} ${METKIT_INCLUDE_DIRS} )

list( APPEND ecml_srcs
# Parser
parser/Cell.h
parser/Cell.cc
parser/CellPrinter.h
parser/CellPrinter.cc
parser/CellDotPrinter.h
parser/CellDotPrinter.cc
parser/List.h
parser/List.cc
parser/Request.h
parser/Request.cc
parser/RequestParser.h
parser/RequestParser.cc

# AST
ast/FunctionDefinition.h
ast/FunctionDefinition.cc
ast/Closure.cc
ast/Closure.h

# Core language
core/Environment.h
core/Environment.cc
core/ExecutionContext.h
core/ExecutionContext.cc
core/Interpreter.h
core/Interpreter.cc
core/Module.cc
core/Module.h
core/RequestHandler.h
core/RequestHandler.cc
core/SpecialFormHandler.h
core/SpecialFormHandler.cc

# Prelude
prelude/Prelude.cc
prelude/Prelude.h
prelude/LetHandler.cc
prelude/LetHandler.h
prelude/UpdateHandler.h
prelude/UpdateHandler.cc
prelude/VariableLookupHandler.cc
prelude/VariableLookupHandler.h
prelude/DefineFunctionHandler.cc
prelude/DefineFunctionHandler.h
prelude/ApplyHandler.cc
prelude/ApplyHandler.h
prelude/ClosureHandler.h
prelude/ClosureHandler.cc
prelude/ListHandler.cc
prelude/ListHandler.h
prelude/SequenceHandler.cc
prelude/SequenceHandler.h
prelude/PrintHandler.cc
prelude/PrintHandler.h
prelude/TestHandler.cc
prelude/TestHandler.h
prelude/FirstHandler.cc
prelude/FirstHandler.h
prelude/RestHandler.cc
prelude/RestHandler.h
prelude/IfHandler.cc
prelude/IfHandler.h
prelude/TemporaryFileHandler.h
prelude/TemporaryFileHandler.cc
prelude/SystemHandler.h
prelude/SystemHandler.cc
prelude/GetenvHandler.h
prelude/GetenvHandler.cc
prelude/JoinStringsHandler.h
prelude/JoinStringsHandler.cc
prelude/QuoteHandler.h
prelude/QuoteHandler.cc
prelude/NullHandler.h
prelude/NullHandler.cc
prelude/RunHandler.h
prelude/RunHandler.cc
prelude/REPLHandler.h
prelude/REPLHandler.cc
prelude/Autocompleter.h
prelude/Autocompleter.cc
prelude/RangeHandler.h
prelude/RangeHandler.cc
prelude/ForHandler.cc
prelude/ForHandler.h
prelude/GlobHandler.cc
prelude/GlobHandler.h
prelude/MatchHandler.h
prelude/MatchHandler.cc
prelude/ReadTextFileHandler.h
prelude/ReadTextFileHandler.cc
prelude/TryHandler.h
prelude/TryHandler.cc
prelude/ThrowHandler.h
prelude/ThrowHandler.cc

# Data types' support
data/DataHandleFactory.h
data/DataHandleFactory.cc
data/FileHandleFactory.h
data/FileHandleFactory.cc
data/PartFileHandleFactory.h
data/PartFileHandleFactory.cc
data/HttpHandle.h
data/HttpHandle.cc
data/HttpHandleFactory.h
data/HttpHandleFactory.cc
# requires  metkit
data/MarsHandleFactory.cc
data/MarsHandleFactory.h

# Miscellaneous
# Adapter for eckit::Parametrisation
misc/DynamicParametrisation.h
misc/DynamicParametrisation.cc
misc/ParameterizedRequestHandler.h
misc/ParameterizedRequestHandler.cc
)

ecbuild_generate_yy( YYPREFIX    request_
                     YACC        parser/requesty
                     YACC_TARGET requesty
                     LEX         parser/requestl
                     LEX_TARGET  requestl
                     DEPENDANT   parser/RequestParser.cc )

ecbuild_add_library( TARGET             ecml
                     INSTALL_HEADERS    LISTED
                     HEADER_DESTINATION ${INSTALL_INCLUDE_DIR}/ecml
                     SOURCES            ${ecml_srcs}
                     LIBS               eckit 
                                        eckit_cmd # for UserInput
                                        metkit
                                        )

ecbuild_add_executable( TARGET  ecml_test
                        SOURCES tests/ecml_test.cc
                        LIBS    ecml )

ecbuild_add_executable( TARGET  ecml_unittests
                        SOURCES tests/ecml_unittests.cc
                        LIBS    ecml )

add_subdirectory( tests )

configure_file( prelude/prelude.ecml ${CMAKE_BINARY_DIR}/include/prelude.ecml COPYONLY)

install(DIRECTORY src/ 
        DESTINATION ${INSTALL_INCLUDE_DIR} 
        FILES_MATCHING PATTERN "*.h")

install(FILES       ${CMAKE_CURRENT_SOURCE_DIR}/prelude/prelude.ecml
        DESTINATION ${INSTALL_INCLUDE_DIR} )

configure_file(       ecml_config.h.in   ecml_config.h )
install( FILES        ${CMAKE_CURRENT_BINARY_DIR}/ecml_config.h
         DESTINATION  ${INSTALL_INCLUDE_DIR}/ecml/ )


#ecbuild_pkgconfig( NAME ${PROJECT_NAME}
#                   DESCRIPTION "ECML language interpreter"
#                   URL "https://software.ecmwf.int/wiki/display/ODB/ODB+API"
#                   LIBRARIES ecml )

# export to other packages and define install targets
#ecbuild_install_project( NAME ecml )

#ecbuild_print_summary()

