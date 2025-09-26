#==============================================================================
# VISTA-SDK-CPP - C++ Library CMake Sources
#==============================================================================

set(PUBLIC_HEADERS
	# --- Core configuration ---
	${VISTA_SDK_CPP_INCLUDE_DIR}/dnv/vista/sdk/config/config.h

	# --- Mqtt layer headers ---
	${VISTA_SDK_CPP_INCLUDE_DIR}/dnv/vista/sdk/mqtt/LocalId.h

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
	${VISTA_SDK_CPP_SOURCE_DIR}/internal/transport/ISO19848.h
)

set(PRIVATE_SOURCES
	# --- Internal core implementations ---
	${VISTA_SDK_CPP_SOURCE_DIR}/internal/core/EmbeddedResource.cpp
	${VISTA_SDK_CPP_SOURCE_DIR}/internal/core/GmodVersioning.cpp
	${VISTA_SDK_CPP_SOURCE_DIR}/internal/core/GmodVersioningNode.cpp

	# --- Internal DTO implementations ---
	${VISTA_SDK_CPP_SOURCE_DIR}/internal/dto/CodebooksDto.cpp
	${VISTA_SDK_CPP_SOURCE_DIR}/internal/dto/GmodDto.cpp
	${VISTA_SDK_CPP_SOURCE_DIR}/internal/dto/GmodVersioningDto.cpp
	${VISTA_SDK_CPP_SOURCE_DIR}/internal/dto/LocationsDto.cpp

	# --- Internal parsing implementations ---
	${VISTA_SDK_CPP_SOURCE_DIR}/internal/parsing/LocalIdParsingErrorBuilder.cpp
	${VISTA_SDK_CPP_SOURCE_DIR}/internal/parsing/LocationParsingErrorBuilder.cpp

	# --- Internal transport implementations ---
	${VISTA_SDK_CPP_SOURCE_DIR}/internal/transport/dto/ISO19848Dtos.cpp

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
