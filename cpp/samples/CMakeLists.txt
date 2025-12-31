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
    Sample_Codebooks.cpp
    Sample_DataChannelList.cpp
    Sample_Gmod.cpp
    Sample_GmodPath.cpp
    Sample_GmodVersioning.cpp
    Sample_ImoNumber.cpp
    Sample_ISO19848.cpp
    Sample_LocalId.cpp
    Sample_Locations.cpp
    Sample_Queries.cpp
    Sample_UniversalId.cpp
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
