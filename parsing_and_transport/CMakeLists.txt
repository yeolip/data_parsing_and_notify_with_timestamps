
project(parsing_and_transport)

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

if(WIN32)
    if(USE_OPENCV MATCHES "ON")
        message(STATUS "windows")

        set ( OPENCV_ROOT ${CMAKE_SOURCE_DIR}/lib/opencv/win/4.1.1 )
	    if ( EXISTS ${OPENCV_ROOT} )
		    set ( OPENCV_INC ${OPENCV_ROOT}/include )
		    set ( OPENCV_LIB ${OPENCV_ROOT}/lib/${CMAKE_BUILD_TYPE} )
		    set ( OPENCV_DLL ${OPENCV_ROOT}/bin/${CMAKE_BUILD_TYPE} )
	    endif (EXISTS ${OPENCV_ROOT} )

        if ( CMAKE_BUILD_TYPE MATCHES "Debug")
            set (OPENCV_LIB_NAME "opencv_world411d" )
        else()
            set (OPENCV_LIB_NAME "opencv_world411" )
        endif( CMAKE_BUILD_TYPE MATCHES "Debug")

        message(STATUS ${OPENCV_ROOT})
        message(STATUS ${OPENCV_INC})
        message(STATUS ${OPENCV_LIB})
        message(STATUS ${OPENCV_DLL})

        add_definitions( -DOPENCV_ON -DOPENCV_ON_WITH_OPENCL)

        find_library(OPENCV_LIBRARY
            NAMES ${OPENCV_LIB_NAME} 
            HINTS ${OPENCV_LIB}
        )
        message(STATUS ${OPENCV_LIBRARY})
        message(STATUS "find_lib")

    else(USE_OPENCV MATCHES "ON")
        set(OPENCV_ROOT "")
        set(OPENCV_INC "")
        set(OPENCV_LIB "")
        set(OPENCV_DLL "")
        set(OPENCV_LIB_NAME "")
    endif(USE_OPENCV MATCHES "ON")

endif(WIN32)

SET(SRC_DIR ${ADTF_STREAMING_DIR}/parsing_and_transport)

message(STATUS ${SRC_HEADER})
message(STATUS ${SRC_FILES})
message(STATUS ${ADTF_STREAMING_DIR}/include)
message(STATUS ${SRC_DIR})

add_definitions(-DWIN32 -D_DEBUG -DUNICODE)

include_directories(${ADTF_STREAMING_DIR}/include)
include_directories(${SRC_DIR})
include_directories(${SRC_DIR}/adtf)
#include_directories(${SRC_DIR}/codec)
#include_directories(${SRC_DIR}/codec)
include_directories(${OPENCV_INC})


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

target_link_directories(parsing_and_transport 
        PUBLIC ${OPENCV_LIB})
#add_library( parsing_and_transport SHARED ${ADTF_STREAMING_DIR}/lib/${ADTF_STREAMING_LIB}_${ADTF_STREAMING_LIB_VERSION}.lib )
#target_link_libraries(parsing_and_transport ${ADTF_STREAMING_LIB})
if (WIN32)
    target_link_libraries(parsing_and_transport ${OPENCV_LIB_NAME} ${ADTF_STREAMING_DIR}/lib/${ADTF_STREAMING_LIB}.lib Vfw32 winmm)
else (WIN32)
    if(ADTF_FFMPEG_DIR)
        target_link_libraries(parsing_and_transport ${OPENCV_LIB_NAME} ${ADTF_STREAMING_DIR}/lib/${ADTF_STREAMING_LIB}.lib avutil avformat avcodec)
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

##################################



add_executable(makeDat  
    ${SRC_FILES} 
)

target_compile_definitions(makeDat PUBLIC _RUN_MAKE_USER_DAT_=1)


if (WIN32)
    target_link_libraries(makeDat ${ADTF_STREAMING_DIR}/lib/${ADTF_STREAMING_LIB}.lib Vfw32 winmm)
else (WIN32)
    if(ADTF_FFMPEG_DIR)
        target_link_libraries(makeDat avutil avformat avcodec)
    endif(ADTF_FFMPEG_DIR) 
    set_target_properties(makeDat PROPERTIES INSTALL_RPATH "$ORIGIN/;${ADTF_FFMPEG_DIR}/lib")
endif (WIN32)

if(NOT WIN32)
    #this way the streaming lib will be loaded from the same directory that the binary resides in
    set_target_properties(makeDat PROPERTIES INSTALL_RPATH "$ORIGIN/")
endif(NOT WIN32)

install(TARGETS makeDat DESTINATION bin/debug CONFIGURATIONS Debug)
install(TARGETS makeDat DESTINATION bin CONFIGURATIONS RelWithDebInfo)
install(TARGETS makeDat DESTINATION bin CONFIGURATIONS Release)
