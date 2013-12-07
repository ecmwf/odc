list( APPEND odbtools_src_files
CompactTool.cc
CompactTool.h
CompareTool.cc
CompareTool.h
CountTool.cc
CountTool.h
FixedSizeRowTool.cc
FixedSizeRowTool.h
ImportTool.cc
ImportTool.h
LSTool.cc
LSTool.h
MDSetTool.cc
MDSetTool.h
MergeTool.cc
MergeTool.h
ODA2RequestTool.cc
ODA2RequestTool.h
ODAHeaderTool.cc
ODAHeaderTool.h
SQLTool.cc
SQLTool.h
SetTool.cc
SetTool.h
SplitTool.cc
SplitTool.h
Tool.cc
Tool.h
ToolFactory.cc
ToolFactory.h
ToolRunnerApplication.cc
ToolRunnerApplication.h
XYVTool.cc
XYVTool.h
)

ecbuild_add_library( TARGET     odbtools
                     INSTALL_HEADERS LISTED
					 HEADER_DESTINATION
						${INSTALL_INCLUDE_DIR}/odblib
                     COMPONENT  adb_api
                     SOURCES    ${odbtools_src_files}
                     TEMPLATES  ${odbtools_templates}
                     LIBS       Odb ${ECKIT_LIBRARIES} )

ecbuild_add_executable( TARGET    odb
                      COMPONENT odb_api
                      SOURCES   odb.cc
                      LIBS      odbtools Odb ${ECKIT_LIBRARIES} )
