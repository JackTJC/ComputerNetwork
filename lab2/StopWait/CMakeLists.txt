cmake_minimum_required(VERSION 3.10)
project(lab2)

SET(CMAKE_C_COMPTLER GCC)
set(CMAKE_CXX FLAGS -fexec-charset=GBK)
add_definitions(-std=c++11)
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/include)
aux_source_directory(${PROJECT_SOURCE_DIR}/src SRC_LIST)
SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
FIND_LIBRARY(NETSIM_LIB libnetsim.a ${PROJECT_SOURCE_DIR}/lib)
#StopWait
ADD_EXECUTABLE(stop_wait ${PROJECT_SOURCE_DIR}/src/StopWait.cpp ${PROJECT_SOURCE_DIR}/src/StopWaitRdtReceiver.cpp ${PROJECT_SOURCE_DIR}/src/StopWaitRdtSender.cpp)
TARGET_LINK_LIBRARIES(stop_wait  ${NETSIM_LIB})
#GBN
add_executable(gbn ${PROJECT_SOURCE_DIR}/src/GBN.cpp)
TARGET_LINK_LIBRARIES(gbn ${NETSIM_LIB})
#SR
add_executable(sr ${PROJECT_SOURCE_DIR}/src/SR.cpp)
TARGET_LINK_LIBRARIES(sr ${NETSIM_LIB})
#TCP
add_executable(tcp ${PROJECT_SOURCE_DIR}/src/TCP.cpp)
TARGET_LINK_LIBRARIES(tcp ${NETSIM_LIB})