cmake_minimum_required (VERSION 3.8)

set(TARGET race-rewind)

find_package(Qt5 COMPONENTS Widgets Network Multimedia Positioning Sensors REQUIRED)
find_package(FileGdbApi REQUIRED)
find_package(PROJ CONFIG REQUIRED)
find_package(SQLite3 REQUIRED)
find_package(ArcGisSdk REQUIRED)

set(HEADER_FILES
	EsriUtils.h
	Forward.h
	GeodatabaseCompiler.h
	RaceRewindWindow.h
	StringUtils.h
)

set(SOURCE_FILES
	EsriUtils.cpp
	GeodatabaseCompiler.cpp
	RaceRewindWindow.cpp
	StringUtils.cpp
	main.cpp
)

add_executable(${TARGET} ${SOURCE_FILES} ${HEADER_FILES})

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET ${TARGET} PROPERTY CXX_STANDARD 20)
endif()

include_directories(${FileGdbApi_INCLUDE_DIRS} ${ArcGisSdk_INCLUDE_DIRS})
target_link_libraries(${TARGET} Qt5::Widgets Qt5::Network FileGdbApi PROJ::proj SQLite::SQLite3 ArcGisSdk::ArcGisSdk)

# Copy dependent Qt libraries to a subdirectory of the binary directory, to make them easier to find during the install step
windeployqt(${TARGET} ${CMAKE_CURRENT_BINARY_DIR}/windeployqt)

# Install this executable
install(TARGETS ${TARGET} DESTINATION bin)

# Install runtime libraries (dlls) for dependent libraries
install(IMPORTED_RUNTIME_ARTIFACTS FileGdbApi PROJ::proj ArcGisSdk::Common ArcGisSdk::Runtime Qt5::Network Qt5::Positioning Qt5::Sensors Qt5::Multimedia)

# Install dependent Qt libraries
install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/windeployqt/ DESTINATION bin)