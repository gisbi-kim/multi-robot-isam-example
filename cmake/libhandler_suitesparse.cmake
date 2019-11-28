macro (libhandler_suitesparse)
  set (INSTALLMSG "on ubuntu `sudo apt-get install libsuitesparse-dev`")
  libhandler_find_library (amd ${INSTALLMSG} ${ARGN})
  libhandler_find_library (camd ${INSTALLMSG} ${ARGN})
  libhandler_find_library (btf ${INSTALLMSG} ${ARGN})
  libhandler_find_library (colamd ${INSTALLMSG} ${ARGN})
  libhandler_find_library (ccolamd ${INSTALLMSG} ${ARGN})
  libhandler_find_library (cholmod ${INSTALLMSG} ${ARGN})
  libhandler_find_library (csparse ${INSTALLMSG} ${ARGN})
  libhandler_find_library (cxsparse ${INSTALLMSG} ${ARGN})
  libhandler_find_library (klu ${INSTALLMSG} ${ARGN})
  libhandler_find_library (ldl ${INSTALLMSG} ${ARGN})
  libhandler_find_library (umfpack ${INSTALLMSG} ${ARGN})


  if (AMD_FOUND AND CAMD_FOUND AND BTF_FOUND AND COLAMD_FOUND AND
      CCOLAMD_FOUND AND CHOLMOD_FOUND AND CSPARSE_FOUND AND
      CXSPARSE_FOUND AND KLU_FOUND AND LDL_FOUND AND UMFPACK_FOUND)
    set (SUITESPARSE_FOUND TRUE)
    set (IRPLIB_SUITESPARSE
      ${AMD_LIBRARIES}
      ${CAMD_LIBRARIES}
      ${BTF_LIBRARIES}
      ${COLAMD_LIBRARIES}
      ${CCOLAMD_LIBRARIES}
      ${CHOLMOD_LIBRARIES}
      ${CSPARSE_LIBRARIES}
      ${CXSPARSE_LIBRARIES}
      ${KLU_LIBRARIES}
      ${LDL_LIBRARIES}
      ${UMFPACK_LIBRARIES}
      )
  else ()
    set (SUITESPARSE_FOUND FALSE)
  endif ()
endmacro ()
