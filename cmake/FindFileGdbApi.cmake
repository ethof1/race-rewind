include(FindPackageHandleStandardArgs)

find_library(FileGdbApi_LIBRARY_RELEASE NAMES FileGDBAPI PATHS "C:/Users/etho/Downloads/FileGDB_API_VS2019/lib64")
find_library(FileGdbApi_LIBRARY_DEBUG NAMES FileGDBAPID PATHS "C:/Users/etho/Downloads/FileGDB_API_VS2019/lib64")

find_file(FileGdbApi_DLL_RELEASE Names FileGDBAPI.dll PATHS "C:/Users/etho/Downloads/FileGDB_API_VS2019/bin64")
find_file(FileGdbApi_DLL_DEBUG Names FileGDBAPID.dll PATHS "C:/Users/etho/Downloads/FileGDB_API_VS2019/bin64")

find_path(FileGdbApi_INCLUDE_DIRS NAMES FileGDBAPI.h PATHS "C:/Users/etho/Downloads/FileGDB_API_VS2019/include")

find_package_handle_standard_args(FileGdbApi REQUIRED_VARS FileGdbApi_LIBRARY_RELEASE FileGdbApi_LIBRARY_DEBUG FileGdbApi_INCLUDE_DIRS)

if (FileGdbApi_FOUND)
  mark_as_advanced(FileGdbApi_LIBRARY_RELEASE)
  mark_as_advanced(FileGdbApi_LIBRARY_DEBUG)
  mark_as_advanced(FileGdbApi_INCLUDE_DIR)
endif()

if (FileGdbApi_FOUND)
  add_library(FileGdbApi SHARED IMPORTED)
  set_property(TARGET FileGdbApi PROPERTY IMPORTED_IMPLIB_RELEASE ${FileGdbApi_LIBRARY_RELEASE})
  set_property(TARGET FileGdbApi PROPERTY IMPORTED_IMPLIB_DEBUG ${FileGdbApi_LIBRARY_DEBUG})
  set_property(TARGET FileGdbApi PROPERTY IMPORTED_LOCATION_RELEASE ${FileGdbApi_DLL_RELEASE})
  set_property(TARGET FileGdbApi PROPERTY IMPORTED_LOCATION_DEBUG ${FileGdbApi_DLL_DEBUG})
endif()