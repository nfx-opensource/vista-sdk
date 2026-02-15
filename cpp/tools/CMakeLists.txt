#==============================================================================
# dnv-vista-sdk-cpp - Tools
#==============================================================================

#----------------------------------------------
# Tools condition check
#----------------------------------------------

if(NOT DNV_VISTA_SDK_CPP_BUILD_TOOLS)
    return()
endif()

#----------------------------------------------
# Tools source files
#----------------------------------------------

set(tools_sources)

list(APPEND tools_sources
    codebooks-cli.cpp
    gmod-cli.cpp
    json-validator-cli.cpp
    localid-cli.cpp
    locations-cli.cpp
)

#----------------------------------------------
# Configure tools executables
#----------------------------------------------

foreach(tools_source ${tools_sources})
    get_filename_component(tool_target_name ${tools_source} NAME_WE)

    if(NOT TARGET ${tool_target_name})
        add_executable(${tool_target_name} ${tools_source})

        #----------------------------------------------
        # Target linking
        #----------------------------------------------

    # Choose library type based on option
    if(DNV_VISTA_SDK_CPP_LINK_TESTS_SHARED AND TARGET dnv-vista-sdk-cpp)
        set(sdk_library_target dnv-vista-sdk-cpp)
    else()
        set(sdk_library_target dnv-vista-sdk-cpp::static)
    endif()

    target_link_libraries(${tool_target_name}
            PRIVATE
                ${sdk_library_target}
        )

        if(tool_target_name STREQUAL "json-validator-cli")
            target_link_libraries(${tool_target_name} PRIVATE
                $<TARGET_OBJECTS:dnv-vista-cpp-embeddedschemas>
            )

            if(WIN32 AND CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND DNV_VISTA_SDK_CPP_LINK_TESTS_SHARED AND TARGET dnv-vista-sdk-cpp)
                target_link_options(${tool_target_name}
                    PRIVATE
                        -Wl,--allow-multiple-definition
                )
            endif()
        endif()

        #----------------------------------------------
        # Include directories
        #----------------------------------------------

        target_include_directories(${tool_target_name} PRIVATE
            ${DNV_VISTA_SDK_CPP_SOURCE_DIR}
        )

        #----------------------------------------------
        # Enable specific CPU features
        #----------------------------------------------

        if(DNV_VISTA_SDK_CPP_ENABLE_SIMD)
            target_compile_options(${tool_target_name}
                PRIVATE
                    $<$<CXX_COMPILER_ID:MSVC>:/arch:AVX2>
                    $<$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>:-march=native>
            )
        endif()

        #----------------------------------------------
        # Compiler warnings
        #----------------------------------------------

        target_compile_options(${tool_target_name}
            PRIVATE
                $<$<CXX_COMPILER_ID:MSVC>:/W4 /WX>
                $<$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>:-Wall -Wextra -Werror>
        )

        #----------------------------------------------
        # Properties
        #----------------------------------------------

        set_target_properties(${tool_target_name}
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

        #----------------------------------------------
        # Installation
        #----------------------------------------------

        if(DNV_VISTA_SDK_CPP_INSTALL_PROJECT)
            install(TARGETS ${tool_target_name}
                RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
            )
        endif()
    endif()
endforeach()
