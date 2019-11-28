option (BUILD_OPENCV310 "Build and install third-party OpenCV 3.1.0" OFF)
if (BUILD_OPENCV310)

  # TARGETS
  set (OPENCV_SRC "${THIRD_PARTY_DIR}/opencv")
  set (OPENCV_DIR "opencv")
  set (OPENCV_CONTRIB_DIR "opencv_contrib")

  add_custom_target (opencv-310
    COMMAND rm -f ${OPENCV_DIR}/BUILT_FLAG
    COMMAND cmake ..
    COMMAND make opencv-target
    )

  if (NOT EXISTS ${CMAKE_CURRENT_BINARY_DIR}/${OPENCV_DIR}/BUILT_FLAG)
    add_custom_target (opencv-target
      COMMAND git clone https://github.com/opencv/opencv.git
      COMMAND git clone https://github.com/opencv/opencv_contrib.git
      COMMAND cd ${OPENCV_CONTRIB_DIR} && git checkout tags/3.1.0
      COMMAND cd ${OPENCV_DIR} && git checkout tags/3.1.0
      COMMAND cd ${OPENCV_DIR} && git format-patch -1 10896129b39655e19e4e7c529153cb5c2191a1db
      COMMAND cd ${OPENCV_DIR} && git am < 0001-GraphCut-deprecated-in-CUDA-7.5-and-removed-in-8.0.patch
      COMMAND cd ${OPENCV_DIR} && mkdir release
      COMMAND cd ${OPENCV_DIR}/release && cmake ../ -DOPENCV_EXTRA_MODULES_PATH=../../opencv_contrib/modules && make && sudo make install
      COMMAND sudo ldconfig
      COMMAND touch ${OPENCV_DIR}/BUILT_FLAG
      COMMAND cmake ..
      )
    add_dependencies (third-party opencv-target)
  else ()
    message (STATUS "Skipping built target opencv")
  endif ()

  add_custom_target (uninstall-opencv
    COMMAND cd ${OPENCV_DIR}/ && sudo make clean
    COMMAND sudo ldconfig
    COMMAND rm -f ${OPENCV_DIR}/BUILT_FLAG
    COMMAND cmake ..
    )
  add_dependencies (uninstall uninstall-opencv)

  add_custom_target (clean-opencv
    COMMAND cd ${OPENCV_DIR} && sudo make clean
    COMMAND rm -f ${OPENCV_DIR}/BUILT_FLAG
    COMMAND cmake ..
    )
  add_dependencies (cleanup clean-opencv)
endif (BUILD_OPENCV310)
