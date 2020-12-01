## CMake rules to build Godot Module as static library
#### To be used with gdnative-project
#### https://github.com/hlfstr/gdnative-project

CMAKE_MINIMUM_REQUIRED(VERSION 3.15)
# Set name of project to current directory name
GET_FILENAME_COMPONENT(LibName ${CMAKE_CURRENT_SOURCE_DIR} NAME)
STRING(REPLACE " " "_" LibName ${LibName})
PROJECT(${LibName} LANGUAGES CXX)

# include the version.txt
INCLUDE(${PROJECT_SOURCE_DIR}/version.txt OPTIONAL)

# include the static_gdns.txt
INCLUDE(${PROJECT_SOURCE_DIR}/static_gdns.txt OPTIONAL)

## Needed to link everything?
SET(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS TRUE)

# Set output directory
SET(OUTPATH "${PROJECT_BINARY_DIR}/lib/${SYSTEM_NAME}")
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${OUTPATH}")
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG "${OUTPATH}")
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE "${OUTPATH}")
SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${OUTPATH}")
SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG "${OUTPATH}")
SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE "${OUTPATH}")
SET(CMAKE_PDB_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")

# Variables used later in branches
SET(LIBL_FLAGS )

# Find all source files in source dir
FILE(GLOB_RECURSE LIBSOURCES ${PROJECT_SOURCE_DIR}/src/*.c**)

# Define library
ADD_LIBRARY(${LibName} STATIC ${LIBSOURCES})

# Set target features
TARGET_COMPILE_FEATURES(${LibName} PUBLIC cxx_std_17)
SET_TARGET_PROPERTIES(${LibName} PROPERTIES PREFIX "lib")
if(ANDROID)
	SET_TARGET_PROPERTIES(${LibName} PROPERTIES OUTPUT_NAME "${LibName}.${ANDROID_ABI}")
else()
  SET_TARGET_PROPERTIES(${LibName} PROPERTIES OUTPUT_NAME "${LibName}.${BITS}")
endif()

# Include godot-cpp and other needed stuff
TARGET_INCLUDE_DIRECTORIES(${LibName}
  PUBLIC
  ${PROJECT_SOURCE_DIR}/include
  ${PROJECT_SOURCE_DIR}/include/flatbuffers/include
  ${PROJECT_INCLUDES}
)

# Include godot_headers and ignore any warnings
TARGET_INCLUDE_DIRECTORIES(${LibName}
  SYSTEM PUBLIC ${SourceDir}/include/godot-cpp/godot_headers
)

# Set compile flags
SET_PROPERTY(TARGET ${LibName} APPEND_STRING PROPERTY COMPILE_FLAGS ${C_FLAGS})
SET_PROPERTY(TARGET ${LibName} APPEND_STRING PROPERTY LINK_FLAGS ${LIBL_FLAGS})

# Add this to the main target
TARGET_INCLUDE_DIRECTORIES(${ProjectId}
   PUBLIC
  ${PROJECT_SOURCE_DIR}/include
  ${PROJECT_SOURCE_DIR}/include/flatbuffers/include
)

SET(FLATBUFFERS_BUILD_TESTS OFF)
SET(FLATBUFFERS_INSTALL OFF)
SET(FLATBUFFERS_BUILD_FLATC OFF)
ADD_SUBDIRECTORY(${PROJECT_SOURCE_DIR}/include/flatbuffers ${PROJECT_BINARY_DIR}/flatbuffers EXCLUDE_FROM_ALL)
TARGET_LINK_LIBRARIES(${LibName} flatbuffers)

ADD_DEPENDENCIES(${LibName} godot-cpp)
# Add this as a dependency of the main project
ADD_DEPENDENCIES(${ProjectId} ${LibName})

#Check compiler, and set linker flags
IF ("${CMAKE_CXX_COMPILER_ID}" MATCHES "MSVC")
  TARGET_LINK_LIBRARIES(${ProjectId} ${LibName})
  SET_PROPERTY(TARGET ${ProjectId} APPEND_STRING PROPERTY LINK_FLAGS " /WHOLEARCHIVE:lib${LibName}.${BITS}.lib")
ELSE()
  TARGET_LINK_LIBRARIES(${ProjectId} -Wl,--whole-archive ${LibName} -Wl,--no-whole-archive)
ENDIF()
