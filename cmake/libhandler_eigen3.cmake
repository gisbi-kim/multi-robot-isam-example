macro (libhandler_eigen3)
  libhandler_find_package (Eigen3 "on ubuntu `sudo apt-get install libeigen3-dev`" ${ARGN})
  if (EIGEN3_FOUND)
    set (IRPLIB_EIGEN3)
    include_directories (${EIGEN3_INCLUDE_DIR})

    if (CMAKE_CXX_FLAGS)
      set (CMAKE_CXX_FLAGS_ORIG ${CMAKE_CXX_FLAGS})
      string (REPLACE "-Wfloat-equal" "" CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS})
      if (NOT CMAKE_CXX_FLAGS STREQUAL CMAKE_CXX_FLAGS_ORIG)
        message (STATUS "libhandler_eigen3: Stripping -Wfloat-equal from CMAKE_CXX_FLAGS ${CMAKE_CURRENT_SOURCE_DIR}")
      endif ()
    endif ()

    if (NOT CMAKE_BUILD_TYPE STREQUAL "Release" AND NOT CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
      libhandler_warning ("For maximum performance, compile exe's linking against Eigen3 with Release or RelWithDebInfo. Current CMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}")
    endif ()
  endif ()
endmacro ()
