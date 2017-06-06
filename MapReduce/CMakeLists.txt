cmake_minimum_required (VERSION 2.6)
project (MapReduce)
set(OUTPUT_BIN MapReduce)
set(CMAKE_CXX_FLAGS "-std=c++0x")
#set(CMAKE_CXX_FLAGS_RELEASE "-O3")

## make libray
file(GLOB MurmurHashFiles ${PROJECT_SOURCE_DIR}/third_party/MurmurHash/*.cpp)
add_library(MurmurHash ${MurmurHashFiles})

## include -I
include_directories(
  ${PROJECT_SOURCE_DIR}/third_party/boost
  ${PROJECT_SOURCE_DIR}/third_party/boost
  ${PROJECT_SOURCE_DIR}/third_party/MurmurHash
  )

## linkage -L
link_directories(${PROJECT_SOURCE_DIR}/third_party/boost/build)

## all source files
file(GLOB SOURCES "${PROJECT_SOURCE_DIR}/src/*.c*")
#message(${SOURCES})
add_executable(${OUTPUT_BIN} ${SOURCES})

## link -l
target_link_libraries(
  ${OUTPUT_BIN} 
  boost_thread 
  boost_system 
  boost_filesystem 
  MurmurHash
  )
