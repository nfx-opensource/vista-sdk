#==============================================================================
# dnv-vista-sdk-cpp - ResourceGenerator executable CMake configuration
#==============================================================================

#----------------------------------------------
# Target definition
#----------------------------------------------

add_executable(dnv-vista-cpp-resourcegenerator
	${DNV_VISTA_SDK_CPP_SOURCE_DIR}/SourceGenerators/ResourceGenerator.cpp
)

#----------------------------------------------
# Target properties
#----------------------------------------------

set_target_properties(dnv-vista-cpp-resourcegenerator
	PROPERTIES
		CXX_STANDARD 20
		CXX_STANDARD_REQUIRED ON
		CXX_EXTENSIONS OFF
		POSITION_INDEPENDENT_CODE ON
		DEBUG_POSTFIX "-d"
		RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/tools"
)
