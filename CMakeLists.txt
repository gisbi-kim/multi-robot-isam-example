cmake_minimum_required (VERSION 2.6)
project (main)

set (CMAKE_CXX_STANDARD 14)

# filename 
set(MAIN "main")
set (SRC
  ${MAIN}.cpp
  utils/robot2d.h     utils/robot2d.cpp
)

# find dependencies
SET(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)
find_package(Eigen3 REQUIRED)
find_package(Cholmod REQUIRED)

# headers
include_directories(${EIGEN3_INCLUDE_DIR})
include_directories(${CHOLMOD_INCLUDES})
include_directories("${PROJECT_SOURCE_DIR}/include")
include_directories("${PROJECT_SOURCE_DIR}/utils")

# link
link_directories(${CMAKE_SOURCE_DIR}/lib)

# build and link 
add_executable(main_program ${SRC})
target_link_libraries(main_program ${CMAKE_SOURCE_DIR}/lib/libisam.a)
target_link_libraries(main_program ${CHOLMOD_LIBRARIES})


