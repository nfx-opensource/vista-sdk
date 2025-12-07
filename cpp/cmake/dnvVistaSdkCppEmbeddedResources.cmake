#==============================================================================
# dnv-vista-sdk-cpp - EmbeddedResources library CMake configuration
#==============================================================================

#----------------------------------------------
# Target definition
#----------------------------------------------

add_library(dnv-vista-cpp-embeddedresources
	OBJECT
		${DNV_VISTA_SDK_CPP_SOURCE_DIR}/EmbeddedResources/EmbeddedResources.cpp
)

#----------------------------------------------
# Include directories
#----------------------------------------------

target_include_directories(dnv-vista-cpp-embeddedresources
	PUBLIC
		${DNV_VISTA_SDK_CPP_SOURCE_DIR}/EmbeddedResources
	PRIVATE
		${DNV_VISTA_SDK_CPP_SOURCE_DIR}/SDK/dto
)

#----------------------------------------------
# Link libraries
#-------------		---------------------------------

target_link_libraries(dnv-vista-cpp-embeddedresources
	PRIVATE
		zlib-ng
		nfx-serialization::static
		nfx-stringutils::nfx-stringutils
)

#----------------------------------------------
# Target properties
#----------------------------------------------

set_target_properties(dnv-vista-cpp-embeddedresources
	PROPERTIES
		CXX_STANDARD 20
		CXX_STANDARD_REQUIRED ON
		CXX_EXTENSIONS OFF
		POSITION_INDEPENDENT_CODE ON
		DEBUG_POSTFIX "-d"
)

#----------------------------------------------
# Embed resources at build time
#----------------------------------------------

set(DNV_VISTA_SDK_CPP_EMBEDDED_RESOURCES_OUTPUT_DIR "${CMAKE_BINARY_DIR}/generated/dnv-vista-cpp-resources-registry")

# Embedding function: converts .gz resources to C++ byte arrays
function(embed_all_resources_to_target TARGET_NAME RESOURCE_DIR OUTPUT_DIR)
	file(GLOB_RECURSE
			RESOURCE_FILES
				"${RESOURCE_DIR}/*.gz"
	)

	if(NOT RESOURCE_FILES)
		message(WARNING "No resource files found in ${RESOURCE_DIR}")
		return()
	endif()

	list(SORT RESOURCE_FILES COMPARE NATURAL)

	set(ALL_FILES "")
	set(ALL_RESOURCE_IDS "")

	foreach(RESOURCE_FILE ${RESOURCE_FILES})
		get_filename_component(RESOURCE_NAME "${RESOURCE_FILE}" NAME)
		file(RELATIVE_PATH RESOURCE_REL_PATH "${RESOURCE_DIR}" "${RESOURCE_FILE}")
		string(MAKE_C_IDENTIFIER "${RESOURCE_REL_PATH}" RESOURCE_ID)
		set(OUTPUT_CPP "${OUTPUT_DIR}/embedded_${RESOURCE_ID}.cpp")
		add_custom_command(
			OUTPUT "${OUTPUT_CPP}"
			COMMAND $<TARGET_FILE:dnv-vista-cpp-resourcegenerator> "${RESOURCE_FILE}" "${OUTPUT_CPP}" "${RESOURCE_ID}" "${RESOURCE_NAME}" "dnv::vista::sdk::resources"
			DEPENDS "${RESOURCE_FILE}" dnv-vista-cpp-resourcegenerator
			COMMENT "Embedding: ${RESOURCE_NAME}"
			VERBATIM
		)
		list(APPEND ALL_FILES "${OUTPUT_CPP}")
		list(APPEND ALL_RESOURCE_IDS "${RESOURCE_ID}")
	endforeach()

	# Generate registry implementation
	set(REGISTRY_CPP "${OUTPUT_DIR}/resources_registry.cpp")
	set(REGISTRY_INCLUDES "")
	set(REGISTRY_ENTRIES "")

	foreach(RES_ID ${ALL_RESOURCE_IDS})
		string(APPEND REGISTRY_INCLUDES "    extern const uint8_t ${RES_ID}_data[];\n")
		string(APPEND REGISTRY_INCLUDES "    extern const size_t ${RES_ID}_size;\n")
		string(APPEND REGISTRY_INCLUDES "    extern const char ${RES_ID}_name[];\n")
		string(APPEND REGISTRY_ENTRIES "        { ${RES_ID}_name, ${RES_ID}_data, ${RES_ID}_size },\n")
	endforeach()

	file(WRITE "${REGISTRY_CPP}"
"#include <string_view>
#include <vector>
#include <cstdint>
#include <cstddef>

namespace dnv::vista::sdk::resources
{
	struct Resource
	{
		std::string_view name;
		const uint8_t* data;
		size_t size;
	};

${REGISTRY_INCLUDES}

	const std::vector<Resource>& all()
	{
		static const std::vector<Resource> resources = {
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

	list(LENGTH ALL_RESOURCE_IDS NUM_RESOURCES)
	message(STATUS "Configured ${NUM_RESOURCES} embedded resources for ${TARGET_NAME}")
endfunction()

# Generate embedded C++ files for all .gz resources
embed_all_resources_to_target(
	dnv-vista-cpp-embeddedresources
	"${DNV_VISTA_SDK_CPP_RESOURCES_DIR}"
	"${DNV_VISTA_SDK_CPP_EMBEDDED_RESOURCES_OUTPUT_DIR}"
)

# Add generated files directory to include path
target_include_directories(dnv-vista-cpp-embeddedresources
	PRIVATE
		"${DNV_VISTA_SDK_CPP_EMBEDDED_RESOURCES_OUTPUT_DIR}"
)
