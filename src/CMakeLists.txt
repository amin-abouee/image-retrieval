# relase binary folder
set(CMAKE_RELEASE_DIR "../bin/release")
# debug binary folder
set(CMAKE_DEBUG_DIR "../bin/debug")

find_package( OpenCV REQUIRED )

set(HEADER_PATH ${PROJECT_SOURCE_DIR}/headers)
set(LIBS_PATH ${PROJECT_SOURCE_DIR}/libs)
include_directories(${HEADER_PATH})

set(HEADER_FILES ${HEADER_PATH}/RobustFeatureMatching.hpp)
set(SOURCES_FILES main.cpp)

add_executable (image_retrieval ${SOURCES_FILES} ${HEADER_FILES})
target_link_libraries (image_retrieval ${OpenCV_LIBS} ${LIBS_PATH}/libRobustFeatureMatchingLib.dylib)

if(CMAKE_BUILD_TYPE STREQUAL Debug)
	set_target_properties(image_retrieval PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_DEBUG_DIR})
	set_target_properties(image_retrieval PROPERTIES OUTPUT_NAME image_retrieval${BUILD_PREFIX})
else()
	set_target_properties(image_retrieval PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RELEASE_DIR})
endif()