option (BUILD_ISAM "Build and install third-party iSAM?" OFF)
if (BUILD_ISAM)

  # EXTERNAL DEPS
  libhandler_eigen3 ()
  libhandler_suitesparse ()

  # TARGETS
  set (ISAM_SRC "${THIRD_PARTY_DIR}/isam_dev.tar.gz")
  set (ISAM_DIR "isam_dev")

  add_custom_target (isam
    COMMAND rm -f ${ISAM_DIR}/BUILT_FLAG
    COMMAND cmake ..
    COMMAND make isam-target
    )

  if (NOT EXISTS ${CMAKE_CURRENT_BINARY_DIR}/${ISAM_DIR}/BUILT_FLAG)
    add_custom_target (isam-target
      COMMAND mkdir -p ${ISAM_DIR}
      # PJO Jan 13 2014: "isam_dev.tar.gz" does not contain a the directory "isam_dev"
      COMMAND tar zxvfp  ${ISAM_SRC} -C ${ISAM_DIR} #--strip 1
      COMMAND cd ${ISAM_DIR} && patch -p1 < ${THIRD_PARTY_DIR}/isam_dev.patch
      COMMAND cd ${ISAM_DIR} && pwd && make && sudo make install
      COMMAND sudo ldconfig
      COMMAND touch ${ISAM_DIR}/BUILT_FLAG
      COMMAND cmake ..
      )
    add_dependencies (third-party isam-target)
  else ()
    message (STATUS "Skipping built target isam")
  endif ()

  add_custom_target (uninstall-isam
    COMMAND cd ${ISAM_DIR}/ && sudo make clean
    COMMAND sudo rm -rf /usr/local/include/isam
    COMMAND sudo rm -rf /usr/local/lib/libisam.a
    COMMAND sudo ldconfig
    COMMAND rm -f ${ISAM_DIR}/BUILT_FLAG
    COMMAND cmake ..
    )
  add_dependencies (uninstall uninstall-isam)

  add_custom_target (clean-isam
    COMMAND cd ${ISAM_DIR} && sudo make clean
    COMMAND rm -f ${ISAM_DIR}/BUILT_FLAG
    COMMAND cmake ..
    )
  add_dependencies (cleanup clean-isam)
endif (BUILD_ISAM)
