cmake_minimum_required(VERSION 2.8.4 FATAL_ERROR)

#cmake_policy(SET CMP0002 NEW)
#cmake_policy(SET CMP0003 NEW)
#cmake_policy(SET CMP0011 NEW)

project(adtf_parsing_and_transport)

#find_package (ADTF_STREAMING 2.9.0 PATHS ${CMAKE_CURRENT_SOURCE_DIR} REQUIRED)

set_property(GLOBAL PROPERTY USE_FOLDERS true)

#set(ADTF_BUILD_EXAMPLES true)
set(ADTF_STREAMING_DIR ${CMAKE_CURRENT_SOURCE_DIR})
message(STATUS ${ADTF_STREAMING_DIR})
message(STATUS "current dir")

add_subdirectory(parsing_and_transport)

set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT parsing_and_transport)
