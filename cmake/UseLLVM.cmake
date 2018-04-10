# -------------------------------------------------------
# LLVM Info
# -------------------------------------------------------
option(USE_LLVM "Use llvm (clang) to build?" OFF)

IF($ENV{LLVM})
    set(USE_LLVM ON)
ENDIF($ENV{LLVM})

IF(USE_LLVM)
    FIND_PATH(CLANG_C_PATH NAMES clang)
    FIND_PATH(CLANG_CXX_PATH NAMES clang)
    IF(CLANG_C_PATH AND CLANG_CXX_PATH)
        message(STATUS "Using LLVM")
        set(CMAKE_C_COMPILER ${CLANG_C_PATH}/clang CACHE PATH "" FORCE)
        set(CMAKE_CXX_COMPILER ${CLANG_CXX_PATH}/clang++ CACHE PATH "" FORCE)
    ELSE(CLANG_C_PATH AND CLANG_CXX_PATH)
        message(STATUS "Wanted to use LLVM but could not find clang in the system path.")
    ENDIF(CLANG_C_PATH AND CLANG_CXX_PATH)
ENDIF(USE_LLVM)
