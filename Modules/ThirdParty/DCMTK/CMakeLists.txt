project(ITKDCMTK)

set(ITKDCMTK_THIRD_PARTY 1)

# This depends on the external project, nothing locally built
set(ITKDCMTK_NO_SRC 1)

include(CMakeParseArguments)

set(_ITKDCMTK_LIB_NAMES dcmdata dcmimage dcmimgle dcmjpeg dcmjpls
    dcmnet dcmpstat dcmqrdb dcmsr dcmtls ijg12 ijg16 ijg8 oflog ofstd
    oficonv i2d dcmxml dcmtkcharls cmr dcmdsig dcmwlm dcmrt dcmiod dcmfg dcmseg dcmtract dcmpmap dcmect
)
if(WIN32)
  set(ITKDCMTK_LIBDEP_WIN iphlpapi ws2_32 netapi32 wsock32)
endif()

#
# if DCMTK is built elsewhere
if(ITK_USE_SYSTEM_DCMTK)
  # assign includes to std module var
  set(ITKDCMTK_SYSTEM_INCLUDE_DIRS ${DCMTK_INCLUDE_DIRS})

  # Module standard library var
  set(ITKDCMTK_LIBRARIES ${DCMTK_LIBRARIES})

  # When this module is loaded by an app, load DCMTK too.
  set(ITKDCMTK_EXPORT_CODE_INSTALL "
set(DCMTK_DIR \"${DCMTK_DIR}\")
find_package(DCMTK REQUIRED NO_MODULE)
")
  set(ITKDCMTK_EXPORT_CODE_BUILD "
if(NOT ITK_BINARY_DIR)
  set(CMAKE_MODULE_PATH
    \"${CMAKE_CURRENT_SOURCE_DIR}/CMake\" \${CMAKE_MODULE_PATH})
  set(DCMTK_DIR \"${DCMTK_DIR}\")
  find_package(DCMTK REQUIRED NO_MODULE)
endif()
")

  itk_module_impl()

else(ITK_USE_SYSTEM_DCMTK)
  set(DCMTK_EPNAME ITKDCMTK_ExtProject)
  set(lib_dir ${CMAKE_CURRENT_BINARY_DIR}/${DCMTK_EPNAME}-build/lib)

  if(BUILD_SHARED_LIBS)
    set(_ITKDCMTK_LIB_LINKAGE SHARED)
  else()
    set(_ITKDCMTK_LIB_LINKAGE STATIC)
  endif()
  foreach(lib ${_ITKDCMTK_LIB_NAMES})
    # add it as an imported  library target
    add_library(${lib} ${_ITKDCMTK_LIB_LINKAGE} IMPORTED GLOBAL)
  endforeach()

  # Use DCMTK include files in place in the build directory.
  set(ITKDCMTK_INCLUDE ${CMAKE_CURRENT_BINARY_DIR}/${DCMTK_EPNAME})

  # 'stringize' the libraries. Brad King addition
  set(ITKDCMTK_LIBRARIES "${_ITKDCMTK_LIB_NAMES}")
  #
  # add all the embedded include directories to include dirs
  foreach(lib ${_ITKDCMTK_LIB_NAMES})
    # add to include list
    list(APPEND ITKDCMTK_INCLUDE_DIRS
      ${ITKDCMTK_INCLUDE}/${lib}/include)
  endforeach()

  #
  # need the base include dir as well.
  list(APPEND ITKDCMTK_INCLUDE_DIRS
    ${CMAKE_CURRENT_BINARY_DIR}/${DCMTK_EPNAME}-build/config/include)

  set(ITKDCMTK_EXPORT_CODE_BUILD "
set(CMAKE_MODULE_PATH
  \"${CMAKE_CURRENT_SOURCE_DIR}/CMake\" \${CMAKE_MODULE_PATH})
")

set(ITKDCMTK_LINK_DEPENDENCIES ${ITKDCMTK_LIBDEP_WIN})
  # create imported targets when module is loaded from build tree
  if(CMAKE_CONFIGURATION_TYPES)
    set(ITKDCMTK_EXPORT_CODE_BUILD "
${ITKDCMTK_EXPORT_CODE_BUILD}
foreach(lib ${_ITKDCMTK_LIB_NAMES})
  if(NOT TARGET \${lib})
    add_library(\${lib} ${_ITKDCMTK_LIB_LINKAGE} IMPORTED)
    foreach(c ${CMAKE_CONFIGURATION_TYPES})
      string(TOUPPER \"\${c}\" C)
      set_property(TARGET \${lib} PROPERTY IMPORTED_LOCATION_\${C}
        \"${lib_dir}/\${c}/${lib_prefix}\${lib}${lib_suffix}\")
    set_property(TARGET \${lib} PROPERTY
      IMPORTED_LINK_INTERFACE_LIBRARIES
      ${ITKDCMTK_LINK_DEPENDENCIES})
    endforeach()
  endif()
endforeach()
")
  else()
    set(ITKDCMTK_EXPORT_CODE_BUILD "
${ITKDCMTK_EXPORT_CODE_BUILD}
foreach(lib ${_ITKDCMTK_LIB_NAMES})
  if(NOT TARGET \${lib})
    add_library(\${lib} ${_ITKDCMTK_LIB_LINKAGE} IMPORTED)
    set_property(TARGET \${lib} PROPERTY IMPORTED_LOCATION
      \"${lib_dir}/${lib_prefix}\${lib}${lib_suffix}\")
    set_property(TARGET \${lib} PROPERTY
      IMPORTED_LINK_INTERFACE_LIBRARIES
      ${ITKDCMTK_LINK_DEPENDENCIES})
  endif()
endforeach()
")
  endif()
  # create imported targets when module is loaded from install tree
  set(ITKDCMTK_EXPORT_CODE_INSTALL "
foreach(lib ${_ITKDCMTK_LIB_NAMES})
  if(NOT TARGET \${lib})
    add_library(\${lib} ${_ITKDCMTK_LIB_LINKAGE} IMPORTED)
    set_property(TARGET \${lib} PROPERTY IMPORTED_LOCATION
            \"\${ITK_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}/${lib_prefix}\${lib}${lib_suffix}\")
    set_property(TARGET \${lib} PROPERTY
      IMPORTED_LINK_INTERFACE_LIBRARIES
      ${ITKDCMTK_LINK_DEPENDENCIES})
  endif()
endforeach()
")

  # implement module before the ExternalProject, to process
  # dependencies
  itk_module_impl()

  foreach(libdep JPEG TIFF ZLIB PNG)
    #
    # if we're using ITK-built versions of libraries, then
    # have to find the actual library name, instead of the cmake logical name
    set(DCMTK${libdep}_LIBRARIES "")

    foreach(_lib ${ITK${libdep}_LIBRARIES})
      if(TARGET ${_lib})
        set(_lib $<TARGET_FILE:${_lib}>)
      endif()
      list(APPEND DCMTK${libdep}_LIBRARIES ${_lib})
    endforeach()

    # have to replace ; with another separator in order to pass lists into
    # the external project without them getting messed up.
    string(REPLACE ";" ":::" DCMTK${libdep}_LIBRARIES "${DCMTK${libdep}_LIBRARIES}")
    string(REPLACE ";" ":::" DCMTK${libdep}_INCLUDE_DIRS "${ITK${libdep}_INCLUDE_DIRS}")
  endforeach()

  include(${CMAKE_CURRENT_LIST_DIR}/DCMTKGitTag.cmake)

  set(DCMTK_EP_FLAGS
    -DCMAKE_CXX_STANDARD:STRING=${CMAKE_CXX_STANDARD}
    -DCMAKE_CXX_STANDARD_REQUIRED:BOOL=${CMAKE_CXX_STANDARD_REQUIRED}
    -DCMAKE_CXX_EXTENSIONS:BOOL=${CMAKE_CXX_EXTENSIONS}
    -DDCMTK_ENABLE_CXX11:BOOL=ON
    -DDCMTK_FORCE_FPIC_ON_UNIX:BOOL=ON
    -DBUILD_APPS:BOOL=OFF        # Only DCMTK libraries are needed
    -DDCMTK_WITH_OPENSSL:BOOL=ON
    -DDCMTK_WITH_PNG:BOOL=ON
    -DDCMTK_WITH_XML:BOOL=ON
    -DDCMTK_WITH_ICU:BOOL=ON
    -DDCMTK_WITH_TIFF:BOOL=ON
    -DDCMTK_WITH_ZLIB:BOOL=ON)

  if(MSVC)
    list(APPEND DCMTK_EP_FLAGS -DDCMTK_OVERWRITE_WIN32_COMPILER_FLAGS:BOOL=OFF)
  endif()

  if("${CMAKE_GENERATOR}" MATCHES "Unix Makefiles")
    list(APPEND DCMTK_EP_FLAGS -DCMAKE_VERBOSE_MAKEFILE:BOOL=${CMAKE_VERBOSE_MAKEFILE})
  endif()


  # build the list of libraries upon which DCMTK depends
  set(ITKDCMTK_LIBDEP "")
  list(APPEND ITKDCMTK_LIBDEP ${ITKDCMTK_LIBDEP_WIN})
  foreach(lib ITKJPEG_LIBRARIES ITKTIFF_LIBRARIES ITKPNG_LIBRARIES
      ITKZLIB_LIBRARIES)
    foreach(_lib ${${lib}})
      if(TARGET ${_lib})
        list(APPEND ITKDCMTK_LIBDEP ${_lib})
      endif()
    endforeach()
  endforeach()
  # attach actual filenames to the
  # imported libraries from the ExternalProject
  foreach(lib ${_ITKDCMTK_LIB_NAMES})
    # tell the imported library where it's file lives
    if(CMAKE_CONFIGURATION_TYPES)
      foreach(c ${CMAKE_CONFIGURATION_TYPES})
        string(TOUPPER "${c}" C)
        set_property(TARGET ${lib} PROPERTY IMPORTED_LOCATION_${C}
          ${lib_dir}/${c}/${lib_prefix}${lib}${lib_suffix})
        list(APPEND DCMTK_BYPRODUCTS "${lib_dir}/${c}/${lib_prefix}${lib}${lib_suffix}")
      endforeach()
    else()
      set_property(TARGET ${lib} PROPERTY IMPORTED_LOCATION
        ${lib_dir}/${lib_prefix}${lib}${lib_suffix})
      list(APPEND DCMTK_BYPRODUCTS "${lib_dir}/${lib_prefix}${lib}${lib_suffix}")
    endif()

    # make the imported library depend on its prerequisite
    # libraries
    set_property(TARGET ${lib} PROPERTY
      IMPORTED_LINK_INTERFACE_LIBRARIES
      ${ITKDCMTK_LIBDEP} )
  endforeach()

  if(CMAKE_VERSION VERSION_LESS 3.4)
    set(CMAKE_CXX_COMPILER_LAUNCHER_FLAG )
    set(CMAKE_C_COMPILER_LAUNCHER_FLAG )
  else()
    set(CMAKE_CXX_COMPILER_LAUNCHER_FLAG -DCMAKE_CXX_COMPILER_LAUNCHER:FILEPATH=${CMAKE_CXX_COMPILER_LAUNCHER})
    set(CMAKE_C_COMPILER_LAUNCHER_FLAG -DCMAKE_C_COMPILER_LAUNCHER:FILEPATH=${CMAKE_C_COMPILER_LAUNCHER})
  endif()
  set(_cross_compiling_flags)
  if(CMAKE_TOOLCHAIN_FILE)
    list(APPEND _cross_compiling_flags -DCMAKE_TOOLCHAIN_FILE:FILEPATH=${CMAKE_TOOLCHAIN_FILE})
  endif()
  if(CMAKE_CROSSCOMPILING_EMULATOR)
    list(APPEND _cross_compiling_flags -DCMAKE_CROSSCOMPILING_EMULATOR:FILEPATH=${CMAKE_CROSSCOMPILING_EMULATOR})
  endif()
  itk_download_attempt_check(${DCMTK_EPNAME})
  ExternalProject_Add(${DCMTK_EPNAME}
    GIT_REPOSITORY ${DCMTK_GIT_REPOSITORY}
    GIT_TAG ${DCMTK_GIT_TAG}
    SOURCE_DIR ${DCMTK_EPNAME}
    BINARY_DIR ${DCMTK_EPNAME}-build
    LIST_SEPARATOR ":::"
    INSTALL_COMMAND ""
   CMAKE_ARGS
    -DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE}
    -DBUILD_SHARED_LIBS:BOOL=${BUILD_SHARED_LIBS}
    -DCMAKE_MSVC_RUNTIME_LIBRARY:STRING=${CMAKE_MSVC_RUNTIME_LIBRARY}
    -DDCMTK_DEFAULT_DICT:STRING=builtin
    -DDCMTK_WITH_DOXYGEN:BOOL=OFF
    -DDCMTK_WITH_SNDFILE:BOOL=ON
    -DDCMTK_WITH_WRAP:BOOL=OFF
    -DDCMTK_ENABLE_PRIVATE_TAGS:BOOL=ON
    -DJPEG_LIBRARY:PATH=${DCMTKJPEG_LIBRARIES}
    -DJPEG_INCLUDE_DIR:PATH=${DCMTKJPEG_INCLUDE_DIRS}
    -DTIFF_INCLUDE_DIR:STRING=${DCMTKTIFF_INCLUDE_DIRS}
    -DTIFF_LIBRARY:PATH=${DCMTKTIFF_LIBRARIES}
    -DZLIB_INCLUDE_DIRS:STRING=${DCMTKZLIB_INCLUDE_DIRS}
    -DZLIB_LIBS:PATH=${DCMTKZLIB_LIBRARIES}
    -DPNG_INCLUDE_DIR:STRING=${DCMTKPNG_INCLUDE_DIRS}
    -DPNG_LIBRARY:PATH=${DCMTKPNG_LIBRARIES}
    ${DCMTK_EP_FLAGS}
    ${_cross_compiling_flags}
    -DCMAKE_CXX_COMPILER:FILEPATH=${CMAKE_CXX_COMPILER}
    ${CMAKE_CXX_COMPILER_LAUNCHER_FLAG}
    -DCMAKE_C_COMPILER:FILEPATH=${CMAKE_C_COMPILER}
    ${CMAKE_C_COMPILER_LAUNCHER_FLAG}
    -DCMAKE_LIBRARY_OUTPUT_DIRECTORY:PATH=${CMAKE_LIBRARY_OUTPUT_DIRECTORY}
    -DCMAKE_ARCHIVE_OUTPUT_DIRECTORY:PATH=${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}
    -DCMAKE_RUNTIME_OUTPUT_DIRECTORY:PATH=${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
    -DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_INSTALL_PREFIX}
    -DCMAKE_INSTALL_LIBDIR:PATH=${CMAKE_INSTALL_LIBDIR}
    -DCMAKE_INSTALL_BINDIR:PATH=${CMAKE_INSTALL_BINDIR}
    ${CHARSET_CONVERSION_ARGS}
    DEPENDS ${JPEG_DEPENDENCY} ${PNG_DEPENDENCY} ${TIFF_DEPENDENCY}
    BUILD_BYPRODUCTS ${DCMTK_BYPRODUCTS}
    )
  foreach(lib ${_ITKDCMTK_LIB_NAMES})
    # make imported library target depend on external project target
    add_dependencies(${lib} ${DCMTK_EPNAME})
  endforeach()

  # Tell CPack to install DCMTK stuff
  list(APPEND CPACK_INSTALL_CMAKE_PROJECTS
    "${CMAKE_CURRENT_BINARY_DIR}/${DCMTK_EPNAME}-build;DCMTK;ALL;/")

  #
  # run DCMTK's cmake install script
  install(SCRIPT
    ${CMAKE_CURRENT_BINARY_DIR}/${DCMTK_EPNAME}-build/cmake_install.cmake)
endif()
