CMAKE_MINIMUM_REQUIRED(VERSION 2.8.4 FATAL_ERROR)
PROJECT(fec)
INCLUDE(ExternalProject)
INCLUDE(CheckCXXCompilerFlag)

# Check for C++14 support.
CHECK_CXX_COMPILER_FLAG("-std=c++14" COMPILER_SUPPORTS_CXX14)
IF(COMPILER_SUPPORTS_CXX14)
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")
ELSE()
    MESSAGE(STATUS "The compiler ${CMAKE_CXX_COMPILER} has no C++14 support. Please use a different C++ compiler.")
ENDIF()

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wno-shift-count-overflow -Wno-missing-braces")

# Set default ExternalProject root directory
SET_DIRECTORY_PROPERTIES(PROPERTIES EP_PREFIX .)

# Add fecmagic
ExternalProject_Add(
fecmagic
URL https://github.com/Venemo/fecmagic/archive/master.zip
TIMEOUT 30
# Disable install, configure and build steps
INSTALL_COMMAND ""
CONFIGURE_COMMAND ""
BUILD_COMMAND ""
# Wrap download, configure and build steps in a script to log output
LOG_DOWNLOAD ON
LOG_CONFIGURE ON
LOG_BUILD ON)

# Specify include dir
ExternalProject_Get_Property(fecmagic source_dir)
SET(fecmagic_dir ${source_dir})

# Add ezpwd_rs
ExternalProject_Add(
ezpwd_rs
URL https://github.com/pjkundert/ezpwd-reed-solomon/archive/v1.7.0.tar.gz
TIMEOUT 30
# Disable install, configure and build steps
INSTALL_COMMAND ""
CONFIGURE_COMMAND ""
BUILD_COMMAND ""
# Wrap download, configure and build steps in a script to log output
LOG_DOWNLOAD ON
LOG_CONFIGURE ON
LOG_BUILD ON)

# Specify include dir
ExternalProject_Get_Property(ezpwd_rs source_dir)
SET(ezpwd_rs_dir ${source_dir})

# Add Mersinvald Reed-Solomon
ExternalProject_Add(
mersinvald_reed_solomon
URL https://github.com/mersinvald/Reed-Solomon/archive/master.zip
TIMEOUT 30
# Disable install, configure and build steps
INSTALL_COMMAND ""
CONFIGURE_COMMAND ""
BUILD_COMMAND ""
# Wrap download, configure and build steps in a script to log output
LOG_DOWNLOAD ON
LOG_CONFIGURE ON
LOG_BUILD ON)

# Specify include dir
ExternalProject_Get_Property(mersinvald_reed_solomon source_dir)
SET(mersinvald_reed_solomon_dir ${source_dir})

ENABLE_TESTING()

ADD_SUBDIRECTORY(test EXCLUDE_FROM_ALL)
ADD_SUBDIRECTORY(benchmark EXCLUDE_FROM_ALL)
