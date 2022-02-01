
FILE(GLOB_RECURSE SRC_HEADER *.h *.hpp )
FILE(GLOB_RECURSE SRC_FILES *.cpp *.c)

set (EXCLUDE_DIR "/codec/")
file (GLOB_RECURSE SRC_FILES "*.cpp" "*.c")
foreach (TMP_PATH ${SRC_FILES})
    string (FIND ${TMP_PATH} ${EXCLUDE_DIR} EXCLUDE_DIR_FOUND)
    if (NOT ${EXCLUDE_DIR_FOUND} EQUAL -1)
        list (REMOVE_ITEM SRC_FILES ${TMP_PATH})
    endif ()
endforeach(TMP_PATH)


#if(WIN32)
#    set(CODEC_VFW_HDRS codec/codec_vfw/codec_vfw.h)
#    set(CODEC_VFW_SRCS codec/codec_vfw/codec_vfw.cpp)
#else(WIN32)
#    if(ADTF_FFMPEG_DIR)
#        set(CODEC_FFMPEG_HDRS codec/codec_ffmpeg/codec_ffmpeg.h)
#        set(CODEC_FFMPEG_SRCS codec/codec_ffmpeg/codec_ffmpeg.cpp)
#    endif(ADTF_FFMPEG_DIR)
#endif(WIN32)

#set(CODEC_ADTF_HDRS codec/codec_adtf/codec_adtf.h codec/codec_adtf/rice.h codec/codec_adtf/codec_adtf.h codec/codec_adtf/codec_rice.h)
#set(CODEC_ADTF_SRCS codec/codec_adtf/codec_adtf.cpp codec/codec_adtf/rice.cpp codec/codec_adtf/codec_adtf.cpp codec/codec_adtf/codec_rice.cpp)


SET(SRC_DIR ${PROJECT_SOURCE_DIR}/examples/parsing_and_transport)

message(STATUS ${SRC_HEADER})
message(STATUS ${SRC_FILES})
message(STATUS ${PROJECT_SOURCE_DIR}/include)
message(STATUS ${SRC_DIR})

add_definitions(-DWIN32 -D_DEBUG -DUNICODE)

include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(${SRC_DIR})
include_directories(${SRC_DIR}/adtf)
include_directories(${SRC_DIR}/codec)

add_executable(parsing_and_transport 
    ${SRC_FILES} 
 #   ${CODEC_ADTF_HDRS}
 #   ${CODEC_ADTF_SRCS}
 #   ${CODEC_FFMPEG_HDRS}
 #   ${CODEC_FFMPEG_SRCS}
 #   ${CODEC_VFW_HDRS}
 #   ${CODEC_VFW_SRCS}
    )

#message(STATUS ${CODEC_ADTF_HDRS})
#message(STATUS ${CODEC_ADTF_SRCS})
#message(STATUS ${CODEC_FFMPEG_HDRS})
#message(STATUS ${CODEC_FFMPEG_SRCS})
#message(STATUS ${CODEC_VFW_HDRS})
#message(STATUS ${CODEC_VFW_SRCS})

message(STATUS "current dir2")
message(STATUS ${ADTF_STREAMING_DIR})

#target_link_directories(
#	parsing_and_transport 
#	PUBLIC
#    ${ADTF_STREAMING_DIR}/lib
#)

#target_link_directories(parsing_and_transport ${ADTF_STREAMING_DIR}/lib)

if(${CMAKE_BUILD_TYPE} STREQUAL "Debug")
	set(ADTF_STREAMING_LIB adtfstreamingD_290)
else (${CMAKE_BUILD_TYPE} STREQUAL  "Debug")
	set(ADTF_STREAMING_LIB adtfstreaming_290)
endif (${CMAKE_BUILD_TYPE} STREQUAL  "Debug")

#add_library( parsing_and_transport SHARED ${ADTF_STREAMING_DIR}/lib/${ADTF_STREAMING_LIB}_${ADTF_STREAMING_LIB_VERSION}.lib )
#target_link_libraries(parsing_and_transport ${ADTF_STREAMING_LIB})
if (WIN32)
    target_link_libraries(parsing_and_transport ${ADTF_STREAMING_DIR}/lib/${ADTF_STREAMING_LIB}.lib Vfw32 winmm)
else (WIN32)
    if(ADTF_FFMPEG_DIR)
        target_link_libraries(parsing_and_transport avutil avformat avcodec)
    endif(ADTF_FFMPEG_DIR) 
    set_target_properties(parsing_and_transport PROPERTIES INSTALL_RPATH "$ORIGIN/;${ADTF_FFMPEG_DIR}/lib")
endif (WIN32)

if(NOT WIN32)
    #this way the streaming lib will be loaded from the same directory that the binary resides in
    set_target_properties(parsing_and_transport PROPERTIES INSTALL_RPATH "$ORIGIN/")
endif(NOT WIN32)

install(TARGETS parsing_and_transport DESTINATION bin/debug CONFIGURATIONS Debug)
install(TARGETS parsing_and_transport DESTINATION bin CONFIGURATIONS RelWithDebInfo)
install(TARGETS parsing_and_transport DESTINATION bin CONFIGURATIONS Release)

if (WIN32)
    install(FILES
        ${ADTF_STREAMING_DIR}/lib/${ADTF_STREAMING_LIB}D_${ADTF_STREAMING_LIB_VERSION}.dll
        DESTINATION bin/debug
        CONFIGURATIONS Debug
    )
    install(FILES
        ${ADTF_STREAMING_DIR}/lib/${ADTF_STREAMING_LIB}_${ADTF_STREAMING_LIB_VERSION}.dll
        DESTINATION bin
        CONFIGURATIONS RelWithDebInfo
    )
    install(FILES
        ${ADTF_STREAMING_DIR}/lib/${ADTF_STREAMING_LIB}_${ADTF_STREAMING_LIB_VERSION}.dll
        DESTINATION bin
        CONFIGURATIONS Release
    )
else (WIN32)
    install(FILES
        ${ADTF_STREAMING_DIR}/lib/lib${ADTF_STREAMING_LIB}D_${ADTF_STREAMING_LIB_VERSION}.so
        DESTINATION bin/debug
        CONFIGURATIONS Debug
    )
    install(FILES
        ${ADTF_STREAMING_DIR}/lib/lib${ADTF_STREAMING_LIB}_${ADTF_STREAMING_LIB_VERSION}.so
        DESTINATION bin
        CONFIGURATIONS RelWithDebInfo
    )
    install(FILES
        ${ADTF_STREAMING_DIR}/lib/lib${ADTF_STREAMING_LIB}_${ADTF_STREAMING_LIB_VERSION}.so
        DESTINATION bin
        CONFIGURATIONS Release
    )
endif (WIN32)