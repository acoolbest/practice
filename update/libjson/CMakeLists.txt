PROJECT(json)

CMAKE_MINIMUM_REQUIRED(VERSION 2.6)  

set(CMAKE_VERSION 2.0.0)

# default C / CXX flags
#set(CMAKE_C_FLAGS " -Wall  -Wno-write-strings -fpermissive")
#set(CMAKE_CXX_FLAGS " -Wall  -Wno-write-strings -fpermissive")

#set(CMAKE_C_FLAGS_DEBUG " -Wall  -Wno-write-strings -fpermissive")
#set(CMAKE_CXX_FLAGS_DEBUG " -Wall  -Wno-write-strings -fpermissive")

#set(CMAKE_C_FLAGS_RELEASE " -Wall  -Wno-write-strings -fpermissive")
#set(CMAKE_CXX_FLAGS_RELEASE " -Wall  -Wno-write-strings -fpermissive")

#set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/..)
#set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/..)
#set(CMAKE_C_OUTPUT_EXTENSION_REPLACE 1)
#set(CMAKE_CXX_OUTPUT_EXTENSION_REPLACE 1)
aux_source_directory(./_internal/Source JSON_SRC_LIST)
add_library(json SHARED EXCLUDE_FROM_ALL ${JSON_SRC_LIST})
#add_library(json_static STATIC EXCLUDE_FROM_ALL ${JSON_SRC_LIST})
#set_target_properties(json PROPERTIES VERSION ${CMAKE_VERSION}  SOVERSION ${CMAKE_VERSION})
INSTALL(TARGETS json LIBRARY DESTINATION lib)

#INSTALL(TARGETS json_static ARCHIVE DESTINATION lib)


INSTALL(FILES
  libjson.h
  JSONOptions.h
  DESTINATION include/libjson)


