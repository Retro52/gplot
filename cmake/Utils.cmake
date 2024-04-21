function(setup_custom_bin_output CUSTOM_OUTPUT_PATH)
    if (NOT CUSTOM_OUTPUT_PATH)
        set(CUSTOM_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/bin)
    endif()

    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CUSTOM_OUTPUT_PATH} PARENT_SCOPE)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CUSTOM_OUTPUT_PATH} PARENT_SCOPE)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CUSTOM_OUTPUT_PATH} PARENT_SCOPE)

    # Disable generation of build type (Debug/Release) folders
    set(CMAKE_SKIP_BUILD_RPATH TRUE PARENT_SCOPE)
    set(CMAKE_SKIP_INSTALL_RPATH TRUE PARENT_SCOPE)
    set(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE PARENT_SCOPE)
    set(CMAKE_INSTALL_RPATH_USE_LINK_PATH FALSE PARENT_SCOPE)

    # Disable generation of architecture (x64/Win32) folders for Visual Studio
    set(CMAKE_VS_INCLUDE_INSTALL_TO_DEFAULT_BUILD 1 PARENT_SCOPE)

    # For multi-config generators (e.g., Visual Studio), set the same output directory for all configurations
    if (CMAKE_CONFIGURATION_TYPES)
        foreach (CONFIG ${CMAKE_CONFIGURATION_TYPES})
            string(TOUPPER ${CONFIG} CONFIG_UPPER)
            set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${CONFIG_UPPER} ${CUSTOM_OUTPUT_PATH} PARENT_SCOPE)
            set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_${CONFIG_UPPER} ${CUSTOM_OUTPUT_PATH} PARENT_SCOPE)
            set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${CONFIG_UPPER} ${CUSTOM_OUTPUT_PATH} PARENT_SCOPE)
        endforeach ()
    endif ()

    message(STATUS "Output directory was set to: ${CUSTOM_OUTPUT_PATH}")
endfunction()

