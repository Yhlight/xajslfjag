# FindANTLR4.cmake
# 查找ANTLR4运行时库

find_path(ANTLR4_INCLUDE_DIR
    NAMES antlr4-runtime.h
    PATHS ${ANTLR4_ROOT}/include/antlr4-runtime
    NO_DEFAULT_PATH
)

if(WIN32)
    find_library(ANTLR4_LIBRARY
        NAMES antlr4-runtime
        PATHS ${ANTLR4_ROOT}/lib
        NO_DEFAULT_PATH
    )
else()
    find_library(ANTLR4_LIBRARY
        NAMES antlr4-runtime
        PATHS ${ANTLR4_ROOT}/lib
        NO_DEFAULT_PATH
    )
endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(antlr4-runtime
    REQUIRED_VARS ANTLR4_LIBRARY ANTLR4_INCLUDE_DIR
)

if(antlr4-runtime_FOUND)
    add_library(antlr4-runtime SHARED IMPORTED)
    set_target_properties(antlr4-runtime PROPERTIES
        IMPORTED_LOCATION ${ANTLR4_LIBRARY}
        INTERFACE_INCLUDE_DIRECTORIES ${ANTLR4_INCLUDE_DIR}
    )
    
    if(WIN32)
        # Windows平台需要设置DLL路径
        get_filename_component(ANTLR4_LIB_DIR ${ANTLR4_LIBRARY} DIRECTORY)
        set_target_properties(antlr4-runtime PROPERTIES
            IMPORTED_IMPLIB ${ANTLR4_LIBRARY}
            IMPORTED_LOCATION ${ANTLR4_LIB_DIR}/antlr4-runtime.dll
        )
    endif()
endif()

mark_as_advanced(ANTLR4_INCLUDE_DIR ANTLR4_LIBRARY)