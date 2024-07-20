## Only present and build DCMTK options if ITKIODCMTK is requested

option(ITK_USE_SYSTEM_DCMTK "Use an outside build of DCMTK." OFF)
if(NOT WIN32)
  set(lib_prefix lib)
  if(BUILD_SHARED_LIBS)
    set(lib_suffix "${CMAKE_SHARED_LIBRARY_SUFFIX}")
    set(lib_prefix "${CMAKE_SHARED_LIBRARY_PREFIX}")
  else()
    set(lib_suffix "${CMAKE_STATIC_LIBRARY_SUFFIX}")
    set(lib_prefix "${CMAKE_STATIC_LIBRARY_PREFIX}")
  endif()
else()
  set(lib_prefix "")
  if(BUILD_SHARED_LIBS)
    set(lib_suffix "${CMAKE_IMPORT_LIBRARY_SUFFIX}")
    set(lib_prefix "${CMAKE_IMPORT_LIBRARY_PREFIX}")
  else()
    set(lib_suffix "${CMAKE_STATIC_LIBRARY_SUFFIX}")
    set(lib_prefix "${CMAKE_IMPORT_LIBRARY_PREFIX}")
  endif()
endif()

if(ITK_USE_SYSTEM_DCMTK)
  # Use local FindDCMTK.cmake.
  list(INSERT CMAKE_MODULE_PATH 0 "${CMAKE_CURRENT_LIST_DIR}/CMake")
  find_package(DCMTK REQUIRED NO_MODULE)
else(ITK_USE_SYSTEM_DCMTK)
  # Change default from OFF to ON for portability.
  option(DCMTK_ENABLE_BUILTIN_OFICONV_DATA "Embed oficonv data files into oficonv library" ON)
  # Copied and mofified from DCMTK/CMake/3rdparty.cmake
  include(CheckCXXSourceCompiles)
endif(ITK_USE_SYSTEM_DCMTK)
