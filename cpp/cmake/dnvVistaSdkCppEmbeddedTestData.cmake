#==============================================================================
# dnv-vista-sdk-cpp - EmbeddedTestData library CMake configuration
#==============================================================================

#----------------------------------------------
# Test condition check
#----------------------------------------------

if(NOT DNV_VISTA_SDK_CPP_BUILD_TESTS)
	return()
endif()

#----------------------------------------------
# Target definition
#----------------------------------------------

add_library(dnv-vista-cpp-embeddedtestdata
	OBJECT
		${DNV_VISTA_SDK_CPP_SOURCE_DIR}/EmbeddedTestData/EmbeddedTestData.cpp
)

#----------------------------------------------
# Include directories
#----------------------------------------------

target_include_directories(dnv-vista-cpp-embeddedtestdata
	PUBLIC
		${DNV_VISTA_SDK_CPP_SOURCE_DIR}/EmbeddedTestData
)

#----------------------------------------------
# Link libraries
#----------------------------------------------

target_link_libraries(dnv-vista-cpp-embeddedtestdata
	PRIVATE
		nfx-serialization::static
)

#----------------------------------------------
# Target properties
#----------------------------------------------

set_target_properties(dnv-vista-cpp-embeddedtestdata
	PROPERTIES
		CXX_STANDARD 20
		CXX_STANDARD_REQUIRED ON
		CXX_EXTENSIONS OFF
		POSITION_INDEPENDENT_CODE ON
		DEBUG_POSTFIX "-d"
)

#----------------------------------------------
# Embed test data at build time
#----------------------------------------------

set(DNV_VISTA_SDK_CPP_EMBEDDED_TESTDATA_OUTPUT_DIR "${CMAKE_BINARY_DIR}/generated/dnv-vista-cpp-testdata-registry")

# Embedding function: converts testdata files to C++ byte arrays
function(embed_all_testdata_to_target TARGET_NAME TESTDATA_DIR OUTPUT_DIR)
	file(GLOB
		TESTDATA_FILES
			"${TESTDATA_DIR}/*.json"
			"${TESTDATA_DIR}/*.txt"
			"${TESTDATA_DIR}/*.gz"
	)

	# Also check for additional test files directory
	if(EXISTS "${DNV_VISTA_SDK_CPP_DIR}/test/SDK/transport/serialization/json/_files")
		file(GLOB
			ADDITIONAL_FILES
				"${DNV_VISTA_SDK_CPP_DIR}/test/SDK/transport/serialization/json/_files/*.json"
		)
		list(APPEND TESTDATA_FILES ${ADDITIONAL_FILES})
	endif()

	if(NOT TESTDATA_FILES)
		message(WARNING "No testdata files found in ${TESTDATA_DIR}")
		return()
	endif()

	list(SORT TESTDATA_FILES COMPARE NATURAL)

	set(ALL_FILES "")
	set(ALL_TESTDATA_IDS "")

	foreach(TESTDATA_FILE ${TESTDATA_FILES})
		get_filename_component(TESTDATA_NAME "${TESTDATA_FILE}" NAME)
		string(MAKE_C_IDENTIFIER "${TESTDATA_NAME}" TESTDATA_ID)
		set(OUTPUT_CPP "${OUTPUT_DIR}/testdata_${TESTDATA_ID}.cpp")

		add_custom_command(
			OUTPUT "${OUTPUT_CPP}"
			COMMAND $<TARGET_FILE:dnv-vista-cpp-resourcegenerator> "${TESTDATA_FILE}" "${OUTPUT_CPP}" "${TESTDATA_ID}" "${TESTDATA_NAME}" "dnv::vista::sdk::testdata"
			DEPENDS "${TESTDATA_FILE}" dnv-vista-cpp-resourcegenerator
			COMMENT "Embedding test data: ${TESTDATA_NAME}"
			VERBATIM
		)

		list(APPEND ALL_FILES "${OUTPUT_CPP}")
		list(APPEND ALL_TESTDATA_IDS "${TESTDATA_ID}")
	endforeach()

	# Generate testdata registry implementation
	set(REGISTRY_CPP "${OUTPUT_DIR}/testdata_registry.cpp")
	set(REGISTRY_INCLUDES "")
	set(REGISTRY_ENTRIES "")

	foreach(TD_ID ${ALL_TESTDATA_IDS})
		string(APPEND REGISTRY_INCLUDES "    extern const uint8_t ${TD_ID}_data[];\n")
		string(APPEND REGISTRY_INCLUDES "    extern const size_t ${TD_ID}_size;\n")
		string(APPEND REGISTRY_INCLUDES "    extern const char ${TD_ID}_name[];\n")
		string(APPEND REGISTRY_ENTRIES "        { ${TD_ID}_name, ${TD_ID}_data, ${TD_ID}_size },\n")
	endforeach()

	file(WRITE "${REGISTRY_CPP}"
"#include <string_view>
#include <vector>
#include <cstdint>
#include <cstddef>

namespace dnv::vista::sdk::testdata
{
${REGISTRY_INCLUDES}

	struct TestResource
	{
		std::string_view name;
		const uint8_t* data;
		size_t size;
	};

	const std::vector<TestResource>& all()
	{
		static const std::vector<TestResource> resources = {
${REGISTRY_ENTRIES}
		};
		return resources;
	}
}
")

	# Add all files to target
	list(APPEND ALL_FILES "${REGISTRY_CPP}")
	set_source_files_properties(${ALL_FILES} PROPERTIES GENERATED TRUE)
	target_sources(${TARGET_NAME} PRIVATE ${ALL_FILES})

	list(LENGTH ALL_TESTDATA_IDS NUM_TESTDATA)
	message(STATUS "Configured ${NUM_TESTDATA} embedded test data files for ${TARGET_NAME}")
endfunction()

# Generate embedded C++ files for all testdata files
embed_all_testdata_to_target(
	dnv-vista-cpp-embeddedtestdata
	"${DNV_VISTA_SDK_CPP_TESTDATA_DIR}"
	"${DNV_VISTA_SDK_CPP_EMBEDDED_TESTDATA_OUTPUT_DIR}"
)

# Add generated files directory to include path
target_include_directories(dnv-vista-cpp-embeddedtestdata
	PRIVATE
		"${DNV_VISTA_SDK_CPP_EMBEDDED_TESTDATA_OUTPUT_DIR}"
)
