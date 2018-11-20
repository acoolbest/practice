cmake_minimum_required(VERSION 2.6)
PROJECT(messagedispatch)

AUX_SOURCE_DIRECTORY(./ SRC_LIST1)
#AUX_SOURCE_DIRECTORY(../base/pb/protocol SRC_LIST2)

#SET(LIBRARY_OUTPUT_PATH ./)
SET(EXECUTABLE_OUTPUT_PATH  ./)
if(APPLE)
    SET(PB_LIB_DIR ../base/pb/lib/mac)
elseif(UNIX)
    SET(PB_LIB_DIR ../base/pb/lib/linux)
endif()


SET(MYSQL_INCLUDE_DIR /usr/include/mysql)
SET(MYSQL_LIB /usr/lib64/mysql)


ADD_DEFINITIONS( -g -W -Wall -D_REENTRANT -D_FILE_OFFSET_BITS=64 -DAC_HAS_INFO
-DAC_HAS_WARNING -DAC_HAS_ERROR -DAC_HAS_CRITICAL -DTIXML_USE_STL
-DAC_HAS_DEBUG -DLINUX_DAEMON -std=c++11)

INCLUDE_DIRECTORIES(./ ../base ../base/pb ../base/pb/protocol ../base/jsoncpp /usr/local/include ${MYSQL_INCLUDE_DIR} ../db_proxy_server ../hiredis/hiredis-master)
LINK_DIRECTORIES(./ ../base ${PB_LIB_DIR} ../lib ../base/slog/lib /usr/local/lib64 ${MYSQL_LIB})

#INCLUDE_DIRECTORIES(./ ../base ../base/pb ../base/pb/protocol ../base/jsoncpp /usr/local/include ${MYSQL_INCLUDE_DIR} ../db_proxy_server)
#LINK_DIRECTORIES(./ ../base ${PB_LIB_DIR} ${MYSQL_LIB} ../base/slog/lib /usr/local/lib64)

#ADD_XXXX必须在TARGET_LINK_LIBRARIES前面，否则会报错
#ADD_LIBRARY(${PROJECTNAME} SHARED/STATIC ${SRC_LIST})
ADD_EXECUTABLE(messagedispatch ${SRC_LIST1} ${SRC_LIST2})

TARGET_LINK_LIBRARIES(messagedispatch base protobuf-lite slog rabbitmq mysqlclient_r curl pthread hiredis pool uuid)


