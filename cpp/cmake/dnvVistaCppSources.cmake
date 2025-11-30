#==============================================================================
# VISTA-SDK-CPP - C++ Library CMake Sources
#==============================================================================

set(PUBLIC_HEADERS
	# --- Core configuration ---
	${VISTA_SDK_CPP_INCLUDE_DIR}/dnv/vista/sdk/config/config.h

	# --- Mqtt layer headers ---
	${VISTA_SDK_CPP_INCLUDE_DIR}/dnv/vista/sdk/mqtt/LocalId.h

	# --- serialization layer headers ---
	${VISTA_SDK_CPP_INCLUDE_DIR}/dnv/vista/sdk/serialization/json/DataChannelListSerializationTraits.h
	${VISTA_SDK_CPP_INCLUDE_DIR}/dnv/vista/sdk/serialization/json/TimeSeriesDataSerializationTraits.h

	# --- Transport layer headers ---
	${VISTA_SDK_CPP_INCLUDE_DIR}/dnv/vista/sdk/transport/datachannel/DataChannel.h
	${VISTA_SDK_CPP_INCLUDE_DIR}/dnv/vista/sdk/transport/timeseries/DataChannelId.h
	${VISTA_SDK_CPP_INCLUDE_DIR}/dnv/vista/sdk/transport/timeseries/TimeSeriesData.h
	${VISTA_SDK_CPP_INCLUDE_DIR}/dnv/vista/sdk/transport/ISO19848.h
	${VISTA_SDK_CPP_INCLUDE_DIR}/dnv/vista/sdk/transport/ISO19848Constants.h
	${VISTA_SDK_CPP_INCLUDE_DIR}/dnv/vista/sdk/transport/ShipId.h

	# --- Core SDK headers ---
	${VISTA_SDK_CPP_INCLUDE_DIR}/dnv/vista/sdk/Codebook.h
	${VISTA_SDK_CPP_INCLUDE_DIR}/dnv/vista/sdk/Codebooks.h
	${VISTA_SDK_CPP_INCLUDE_DIR}/dnv/vista/sdk/CodebookName.h
	${VISTA_SDK_CPP_INCLUDE_DIR}/dnv/vista/sdk/Exceptions.h
	${VISTA_SDK_CPP_INCLUDE_DIR}/dnv/vista/sdk/Gmod.h
	${VISTA_SDK_CPP_INCLUDE_DIR}/dnv/vista/sdk/GmodNode.h
	${VISTA_SDK_CPP_INCLUDE_DIR}/dnv/vista/sdk/GmodPath.h
	${VISTA_SDK_CPP_INCLUDE_DIR}/dnv/vista/sdk/GmodPathQuery.h
	${VISTA_SDK_CPP_INCLUDE_DIR}/dnv/vista/sdk/ImoNumber.h
	${VISTA_SDK_CPP_INCLUDE_DIR}/dnv/vista/sdk/LocalId.h
	${VISTA_SDK_CPP_INCLUDE_DIR}/dnv/vista/sdk/LocalIdBuilder.h
	${VISTA_SDK_CPP_INCLUDE_DIR}/dnv/vista/sdk/LocalIdItems.h
	${VISTA_SDK_CPP_INCLUDE_DIR}/dnv/vista/sdk/LocalIdQuery.h
	${VISTA_SDK_CPP_INCLUDE_DIR}/dnv/vista/sdk/LocationBuilder.h
	${VISTA_SDK_CPP_INCLUDE_DIR}/dnv/vista/sdk/Locations.h
	${VISTA_SDK_CPP_INCLUDE_DIR}/dnv/vista/sdk/MetadataTag.h
	${VISTA_SDK_CPP_INCLUDE_DIR}/dnv/vista/sdk/MetadataTagsQuery.h
	${VISTA_SDK_CPP_INCLUDE_DIR}/dnv/vista/sdk/ParsingErrors.h
	${VISTA_SDK_CPP_INCLUDE_DIR}/dnv/vista/sdk/Results.h
	${VISTA_SDK_CPP_INCLUDE_DIR}/dnv/vista/sdk/UniversalIdBuilder.h
	${VISTA_SDK_CPP_INCLUDE_DIR}/dnv/vista/sdk/UniversalId.h
	${VISTA_SDK_CPP_INCLUDE_DIR}/dnv/vista/sdk/VIS.h
	${VISTA_SDK_CPP_INCLUDE_DIR}/dnv/vista/sdk/VISVersion.h

	# --- Transport layer implementation files ---
	${VISTA_SDK_CPP_INCLUDE_DIR}/dnv/vista/sdk/detail/transport/datachannel/DataChannel.inl
	${VISTA_SDK_CPP_INCLUDE_DIR}/dnv/vista/sdk/detail/transport/timeseries/DataChannelId.inl
	${VISTA_SDK_CPP_INCLUDE_DIR}/dnv/vista/sdk/detail/transport/timeseries/TimeSeriesData.inl
	${VISTA_SDK_CPP_INCLUDE_DIR}/dnv/vista/sdk/detail/transport/ISO19848.inl
	${VISTA_SDK_CPP_INCLUDE_DIR}/dnv/vista/sdk/detail/transport/ShipId.inl

	# --- Core implementation files ---
	${VISTA_SDK_CPP_INCLUDE_DIR}/dnv/vista/sdk/detail/Codebook.inl
	${VISTA_SDK_CPP_INCLUDE_DIR}/dnv/vista/sdk/detail/Codebooks.inl
	${VISTA_SDK_CPP_INCLUDE_DIR}/dnv/vista/sdk/detail/Gmod.inl
	${VISTA_SDK_CPP_INCLUDE_DIR}/dnv/vista/sdk/detail/GmodNode.inl
	${VISTA_SDK_CPP_INCLUDE_DIR}/dnv/vista/sdk/detail/GmodPath.inl
	${VISTA_SDK_CPP_INCLUDE_DIR}/dnv/vista/sdk/detail/GmodPathQuery.inl
	${VISTA_SDK_CPP_INCLUDE_DIR}/dnv/vista/sdk/detail/ImoNumber.inl
	${VISTA_SDK_CPP_INCLUDE_DIR}/dnv/vista/sdk/detail/LocalId.inl
	${VISTA_SDK_CPP_INCLUDE_DIR}/dnv/vista/sdk/detail/LocalIdBuilder.inl
	${VISTA_SDK_CPP_INCLUDE_DIR}/dnv/vista/sdk/detail/LocalIdItems.inl
	${VISTA_SDK_CPP_INCLUDE_DIR}/dnv/vista/sdk/detail/LocalIdQuery.inl
	${VISTA_SDK_CPP_INCLUDE_DIR}/dnv/vista/sdk/detail/LocationBuilder.inl
	${VISTA_SDK_CPP_INCLUDE_DIR}/dnv/vista/sdk/detail/Locations.inl
	${VISTA_SDK_CPP_INCLUDE_DIR}/dnv/vista/sdk/detail/MetadataTag.inl
	${VISTA_SDK_CPP_INCLUDE_DIR}/dnv/vista/sdk/detail/MetadataTagsQuery.inl
	${VISTA_SDK_CPP_INCLUDE_DIR}/dnv/vista/sdk/detail/ParsingErrors.inl
	${VISTA_SDK_CPP_INCLUDE_DIR}/dnv/vista/sdk/detail/Results.inl
	${VISTA_SDK_CPP_INCLUDE_DIR}/dnv/vista/sdk/detail/UniversalIdBuilder.inl
	${VISTA_SDK_CPP_INCLUDE_DIR}/dnv/vista/sdk/detail/UniversalId.inl
	${VISTA_SDK_CPP_INCLUDE_DIR}/dnv/vista/sdk/detail/VIS.inl
	${VISTA_SDK_CPP_INCLUDE_DIR}/dnv/vista/sdk/detail/VISVersion.inl
)

set(PRIVATE_HEADERS
	# --- Internal core utilities ---
	${VISTA_SDK_CPP_SOURCE_DIR}/internal/core/Codebook.h
	${VISTA_SDK_CPP_SOURCE_DIR}/internal/core/EmbeddedResource.h
	${VISTA_SDK_CPP_SOURCE_DIR}/internal/core/GmodNode.h
	${VISTA_SDK_CPP_SOURCE_DIR}/internal/core/GmodVersioning.h
	${VISTA_SDK_CPP_SOURCE_DIR}/internal/core/GmodVersioningNode.h
	${VISTA_SDK_CPP_SOURCE_DIR}/internal/core/LocalId.h
	${VISTA_SDK_CPP_SOURCE_DIR}/internal/core/Locations.h
	${VISTA_SDK_CPP_SOURCE_DIR}/internal/core/LocationSetsVisitor.h

	# --- Internal data transfer objects ---
	${VISTA_SDK_CPP_SOURCE_DIR}/internal/dto/CodebooksDto.h
	${VISTA_SDK_CPP_SOURCE_DIR}/internal/dto/GmodDto.h
	${VISTA_SDK_CPP_SOURCE_DIR}/internal/dto/GmodVersioningDto.h
	${VISTA_SDK_CPP_SOURCE_DIR}/internal/dto/Keys.h
	${VISTA_SDK_CPP_SOURCE_DIR}/internal/dto/LocationsDto.h

	# --- Internal parsing utilities ---
	${VISTA_SDK_CPP_SOURCE_DIR}/internal/parsing/LocalIdParsingErrorBuilder.h
	${VISTA_SDK_CPP_SOURCE_DIR}/internal/parsing/LocationCharDict.h
	${VISTA_SDK_CPP_SOURCE_DIR}/internal/parsing/LocationParsingErrorBuilder.h

	# --- Internal transport utilities ---
	${VISTA_SDK_CPP_SOURCE_DIR}/internal/transport/dto/ISO19848Dtos.h
)

set(PRIVATE_SOURCES
	# --- Internal core implementations ---
	${VISTA_SDK_CPP_SOURCE_DIR}/internal/core/EmbeddedResource.cpp
	${VISTA_SDK_CPP_SOURCE_DIR}/internal/core/GmodVersioning.cpp
	${VISTA_SDK_CPP_SOURCE_DIR}/internal/core/GmodVersioningNode.cpp

	# --- Internal parsing implementations ---
	${VISTA_SDK_CPP_SOURCE_DIR}/internal/parsing/LocalIdParsingErrorBuilder.cpp
	${VISTA_SDK_CPP_SOURCE_DIR}/internal/parsing/LocationParsingErrorBuilder.cpp

	# --- Mqtt layer implementations ---
	${VISTA_SDK_CPP_SOURCE_DIR}/mqtt/LocalId.cpp

	# --- Transport layer implementations ---
	${VISTA_SDK_CPP_SOURCE_DIR}/transport/datachannel/DataChannel.cpp
	${VISTA_SDK_CPP_SOURCE_DIR}/transport/timeseries/DataChannelId.cpp
	${VISTA_SDK_CPP_SOURCE_DIR}/transport/timeseries/TimeSeriesData.cpp
	${VISTA_SDK_CPP_SOURCE_DIR}/transport/ISO19848.cpp
	${VISTA_SDK_CPP_SOURCE_DIR}/transport/ShipId.cpp

	# --- Core SDK implementations ---
	${VISTA_SDK_CPP_SOURCE_DIR}/Codebook.cpp
	${VISTA_SDK_CPP_SOURCE_DIR}/CodebookName.cpp
	${VISTA_SDK_CPP_SOURCE_DIR}/Codebooks.cpp
	${VISTA_SDK_CPP_SOURCE_DIR}/Exceptions.cpp
	${VISTA_SDK_CPP_SOURCE_DIR}/Gmod.cpp
	${VISTA_SDK_CPP_SOURCE_DIR}/GmodNode.cpp
	${VISTA_SDK_CPP_SOURCE_DIR}/GmodPath.cpp
	${VISTA_SDK_CPP_SOURCE_DIR}/GmodPathQuery.cpp
	${VISTA_SDK_CPP_SOURCE_DIR}/ImoNumber.cpp
	${VISTA_SDK_CPP_SOURCE_DIR}/LocalId.cpp
	${VISTA_SDK_CPP_SOURCE_DIR}/LocalIdBuilder.cpp
	${VISTA_SDK_CPP_SOURCE_DIR}/LocalIdItems.cpp
	${VISTA_SDK_CPP_SOURCE_DIR}/LocalIdQuery.cpp
	${VISTA_SDK_CPP_SOURCE_DIR}/LocationBuilder.cpp
	${VISTA_SDK_CPP_SOURCE_DIR}/Locations.cpp
	${VISTA_SDK_CPP_SOURCE_DIR}/MetadataTag.cpp
	${VISTA_SDK_CPP_SOURCE_DIR}/MetadataTagsQuery.cpp
	${VISTA_SDK_CPP_SOURCE_DIR}/ParsingErrors.cpp
	${VISTA_SDK_CPP_SOURCE_DIR}/UniversalId.cpp
	${VISTA_SDK_CPP_SOURCE_DIR}/UniversalIdBuilder.cpp
	${VISTA_SDK_CPP_SOURCE_DIR}/VIS.cpp
)

#----------------------------------------------
# Library definition
#----------------------------------------------

# --- Create shared library if requested ---
if(VISTA_SDK_CPP_BUILD_SHARED)
	add_library(${PROJECT_NAME} SHARED)
	target_sources(${PROJECT_NAME}
		PRIVATE
			${PUBLIC_HEADERS}
			${PRIVATE_HEADERS}
			${PRIVATE_SOURCES}
	)

	set_target_properties(${PROJECT_NAME} PROPERTIES
		LIBRARY_OUTPUT_DIRECTORY ${VISTA_SDK_CPP_BUILD_DIR}/lib
		ARCHIVE_OUTPUT_DIRECTORY ${VISTA_SDK_CPP_BUILD_DIR}/lib
	)

	add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})
endif()

# --- Create static library if requested ---
if(VISTA_SDK_CPP_BUILD_STATIC)
	add_library(${PROJECT_NAME}-static STATIC)
	target_sources(${PROJECT_NAME}-static
		PRIVATE
			${PUBLIC_HEADERS}
			${PRIVATE_HEADERS}
			${PRIVATE_SOURCES}
	)

	set_target_properties(${PROJECT_NAME}-static PROPERTIES
		OUTPUT_NAME ${PROJECT_NAME}-static
		ARCHIVE_OUTPUT_DIRECTORY ${VISTA_SDK_CPP_BUILD_DIR}/lib
	)

	add_library(${PROJECT_NAME}::static ALIAS ${PROJECT_NAME}-static)
endif()

#----------------------------------------------
# Target properties
#----------------------------------------------

function(configure_vista_target target_name)
	# --- Include directories---
	target_include_directories(${target_name}
		PUBLIC
			$<BUILD_INTERFACE:${VISTA_SDK_CPP_INCLUDE_DIR}>
			$<INSTALL_INTERFACE:include>
		PRIVATE
			${VISTA_SDK_CPP_SOURCE_DIR}
	)

	# --- Link libraries---
	get_target_property(target_type ${target_name} TYPE)
	if(target_type STREQUAL "SHARED_LIBRARY")
		if(TARGET nfx-meta::nfx-meta)
			target_link_libraries(${target_name} PRIVATE $<BUILD_INTERFACE:nfx-meta::nfx-meta>
		)
		endif()
	else()
		if(TARGET nfx-meta::nfx-meta)
			target_link_libraries(${target_name} PRIVATE $<BUILD_INTERFACE:nfx-meta::nfx-meta>
		)
		endif()
	endif()

	if(TARGET zlib-ng::zlib-ng)
		target_link_libraries(${target_name} PRIVATE $<BUILD_INTERFACE:zlib-ng::zlib-ng>)
	elseif(TARGET zlibstatic)
		target_link_libraries(${target_name} PRIVATE $<BUILD_INTERFACE:zlibstatic>)
	elseif(TARGET zlib)
		target_link_libraries(${target_name} PRIVATE $<BUILD_INTERFACE:zlib>)
	endif()

	set_target_properties(${target_name} PROPERTIES
		CXX_STANDARD 20
		CXX_STANDARD_REQUIRED ON
		CXX_EXTENSIONS OFF
		VERSION ${PROJECT_VERSION}
		SOVERSION ${PROJECT_VERSION_MAJOR}
		POSITION_INDEPENDENT_CODE ON
		DEBUG_POSTFIX "-d"
	)

	# --- Configure resource ---
	target_compile_definitions(${target_name} PRIVATE
		VISTA_SDK_RESOURCES_DIR="${VISTA_SDK_RESOURCES_DIR}"
	)
endfunction()

# --- Apply configuration to both targets ---
if(VISTA_SDK_CPP_BUILD_SHARED)
	configure_vista_target(${PROJECT_NAME})
	if(WIN32)
		set_target_properties(${PROJECT_NAME} PROPERTIES
			WINDOWS_EXPORT_ALL_SYMBOLS TRUE
		)

		configure_file(
			${CMAKE_CURRENT_SOURCE_DIR}/cmake/dnvVistaCppVersion.rc.in
			${CMAKE_BINARY_DIR}/version.rc
			@ONLY
		)
		target_sources(${PROJECT_NAME} PRIVATE ${CMAKE_BINARY_DIR}/version.rc)
	endif()
endif()

if(VISTA_SDK_CPP_BUILD_STATIC)
	configure_vista_target(${PROJECT_NAME}-static)
endif()
