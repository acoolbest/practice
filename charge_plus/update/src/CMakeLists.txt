project (updateservice)

include_directories(${CMAKE_SOURCE_DIR})


set(UPDATE_SOURCE_FILES  UpdateService.cpp PublicFunction.cpp JsonWriter.cpp inifile.cpp HttpRequestInterface.cpp HttpRequest.cpp FileDown.cpp fileMd5.cpp MyResultClass.cpp common.cpp)

# find_path(APR_INCLUDE_DIR apr-1/apr_time.h)
# find_path(APR_UTIL_INCLUDE_DIR apr/include/apr-1/apr_md5.h)
# find_path(MINIXML_INCLUDE_DIR mxml.h)
# find_path(CURL_INCLUDE_DIR curl/curl.h)

include_directories (${APR_INCLUDE_DIR})
include_directories (${APR_UTIL_INCLUDE_DIR})
include_directories (${MINIXML_INCLUDE_DIR})
include_directories (${CURL_INCLUDE_DIR})
include_directories ("${CMAKE_SOURCE_DIR}/oss_c_sdk")

find_library(APR_LIBRARY apr-1 PATHS /usr/local/apr/lib/)
find_library(APR_UTIL_LIBRARY aprutil-1 PATHS /usr/local/apr/lib/)
find_library(MINIXML_LIBRARY mxml)
find_library(CURL_LIBRARY curl)
find_library(PTHREAD_LIBRARY pthread)
find_library(CRYPTO_LIBRARY crypto)
find_library(SSL_LIBRARY ssl)
#find_library(IDN_LIBRARY idn)
find_library(RT_LIBRARY rt)
#find_library(OSS_C_SDK_LIBRARY oss_c_sdk HINTS ${LIBRARY_OUTPUT_PATH})

find_library(JSON_LIBRARY json HINTS ${LIBRARY_OUTPUT_PATH})
add_executable(updateservice ${UPDATE_SOURCE_FILES})

target_link_libraries(updateservice oss_c_sdk)
target_link_libraries(updateservice ${JSON_LIBRARY})
target_link_libraries(updateservice ${APR_UTIL_LIBRARY})
target_link_libraries(updateservice ${APR_LIBRARY})
target_link_libraries(updateservice ${MINIXML_LIBRARY})
target_link_libraries(updateservice ${CURL_LIBRARY})
target_link_libraries(updateservice ${PTHREAD_LIBRARY})
target_link_libraries(updateservice ${SSL_LIBRARY})
target_link_libraries(updateservice ${CRYPTO_LIBRARY})
#target_link_libraries(oss_c_sdk_test ${IDN_LIBRARY})
target_link_libraries(updateservice ${RT_LIBRARY})

