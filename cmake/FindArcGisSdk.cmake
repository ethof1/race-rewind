include(FindPackageHandleStandardArgs)

find_library(ArcGisSdk_LIBRARY_RELEASE NAMES EsriCommonQt PATHS "C:/Program Files (x86)/ArcGIS SDKs/Qt100.15.0/sdk/windows/x64/lib")
find_library(ArcGisSdk_LIBRARY_DEBUG NAMES EsriCommonQtd PATHS "C:/Program Files (x86)/ArcGIS SDKs/Qt100.15.0/sdk/windows/x64/lib")

find_library(ArcGisSdkRuntime_LIBRARY_RELEASE NAMES EsriRuntimeQt PATHS "C:/Program Files (x86)/ArcGIS SDKs/Qt100.15.0/sdk/windows/x64/lib")
find_library(ArcGisSdkRuntime_LIBRARY_DEBUG NAMES EsriRuntimeQtd PATHS "C:/Program Files (x86)/ArcGIS SDKs/Qt100.15.0/sdk/windows/x64/lib")

find_file(ArcGisSdk_DLL_RELEASE Names EsriCommonQt.dll PATHS "C:/Program Files (x86)/ArcGIS SDKs/Qt100.15.0/sdk/windows/x64/bin")
find_file(ArcGisSdk_DLL_DEBUG Names EsriCommonQtd.dll PATHS "C:/Program Files (x86)/ArcGIS SDKs/Qt100.15.0/sdk/windows/x64/bin")

find_path(ArcGisSdk_INCLUDE_DIRS NAMES ArcGISRuntimeEnvironment.h PATHS "C:/Program Files (x86)/ArcGIS SDKs/Qt100.15.0/sdk/include")

find_package_handle_standard_args(ArcGisSdk REQUIRED_VARS ArcGisSdk_LIBRARY_RELEASE ArcGisSdk_LIBRARY_DEBUG ArcGisSdkRuntime_LIBRARY_RELEASE ArcGisSdkRuntime_LIBRARY_DEBUG ArcGisSdk_INCLUDE_DIRS)

if (ArcGisSdk_FOUND)
  add_library(ArcGisSdk::Common SHARED IMPORTED)
  set_property(TARGET ArcGisSdk::Common PROPERTY IMPORTED_IMPLIB_RELEASE ${ArcGisSdk_LIBRARY_RELEASE})
  set_property(TARGET ArcGisSdk::Common PROPERTY IMPORTED_IMPLIB_DEBUG ${ArcGisSdk_LIBRARY_DEBUG})
  set_property(TARGET ArcGisSdk::Common PROPERTY IMPORTED_LOCATION_RELEASE ${ArcGisSdk_DLL_RELEASE})
  set_property(TARGET ArcGisSdk::Common PROPERTY IMPORTED_LOCATION_DEBUG ${ArcGisSdk_DLL_DEBUG})

  add_library(ArcGisSdk::Runtime SHARED IMPORTED)
  set_property(TARGET ArcGisSdk::Runtime PROPERTY IMPORTED_IMPLIB_RELEASE ${ArcGisSdkRuntime_LIBRARY_RELEASE})
  set_property(TARGET ArcGisSdk::Runtime PROPERTY IMPORTED_IMPLIB_DEBUG ${ArcGisSdkRuntime_LIBRARY_DEBUG})
  set_property(TARGET ArcGisSdk::Runtime PROPERTY IMPORTED_LOCATION_RELEASE ${ArcGisSdk_DLL_RELEASE})
  set_property(TARGET ArcGisSdk::Runtime PROPERTY IMPORTED_LOCATION_DEBUG ${ArcGisSdk_DLL_DEBUG})

  # Need to add an entry for the "runtimecore.dll"

  add_library(ArcGisSdk::ArcGisSdk INTERFACE IMPORTED)
  set_property(TARGET ArcGisSdk::ArcGisSdk PROPERTY INTERFACE_LINK_LIBRARIES ArcGisSdk::Common ArcGisSdk::Runtime)
endif()
