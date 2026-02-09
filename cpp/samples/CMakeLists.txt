#==============================================================================
# dnv-vista-sdk-cpp - Samples
#==============================================================================

#----------------------------------------------
# Sample condition check
#----------------------------------------------

if(NOT DNV_VISTA_SDK_CPP_BUILD_SAMPLES)
    return()
endif()

#----------------------------------------------
# Samples source files
#----------------------------------------------

set(sample_sources)

list(APPEND sample_sources
    advanced/Sample_GmodSubset.cpp
    advanced/Sample_Queries.cpp
    core/Sample_Codebooks.cpp
    core/Sample_Gmod.cpp
    core/Sample_GmodPath.cpp
    core/Sample_GmodVersioning.cpp
    core/Sample_Locations.cpp
    identifiers/Sample_ImoNumber.cpp
    identifiers/Sample_LocalId.cpp
    identifiers/Sample_UniversalId.cpp
    transport/Sample_ISO19848.cpp
    transport/Sample_DataChannelList.cpp
    transport/Sample_TimeSeriesData.cpp
)

#----------------------------------------------
# Configure samples executables
#----------------------------------------------

foreach(sample_source ${sample_sources})
    get_filename_component(sample_target_name ${sample_source} NAME_WE)

    if(NOT TARGET ${sample_target_name})
        add_executable(${sample_target_name} ${sample_source})

        #----------------------------------------------
        # Target linking
        #----------------------------------------------

        target_link_libraries(${sample_target_name}
            PRIVATE
                dnv-vista-sdk-cpp::static
        )

        #----------------------------------------------
        # Enable specific CPU features
        #----------------------------------------------

        target_compile_options(${sample_target_name}
            PRIVATE
                $<$<CXX_COMPILER_ID:MSVC>:/arch:AVX2>
                $<$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>:-march=native>
        )

        #----------------------------------------------
        # Properties
        #----------------------------------------------

        set_target_properties(${sample_target_name}
            PROPERTIES
                CXX_STANDARD 20
                CXX_STANDARD_REQUIRED ON
                CXX_EXTENSIONS OFF
                POSITION_INDEPENDENT_CODE ON
                DEBUG_POSTFIX "-d"
                RUNTIME_OUTPUT_DIRECTORY         "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}"
                RUNTIME_OUTPUT_DIRECTORY_DEBUG   "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}"
                RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}"
        )
    endif()
endforeach()
