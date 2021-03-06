#
# Copyright (c) 2016 Lukas Karas <lukas.karas@centrum.cz>
#
# Structure of OSMScout-Sailfish cmake scripts was inspired by DigiKam project
#
# Redistribution and use is allowed according to the terms of the BSD license.

project(osmscout-sailfish C CXX)

# ==================================================================================================
# Information to update before to release this package.

# project version
set(OSMSCOUT_SAILFISH_MAJOR_VERSION "0")
set(OSMSCOUT_SAILFISH_MINOR_VERSION "12")
set(OSMSCOUT_SAILFISH_PATCH_VERSION "0")

set(OSMSCOUT_SAILFISH_SUFFIX_VERSION "")
set(OSMSCOUT_SAILFISH_VERSION_SHORT  "${OSMSCOUT_SAILFISH_MAJOR_VERSION}.${OSMSCOUT_SAILFISH_MINOR_VERSION}.${OSMSCOUT_SAILFISH_PATCH_VERSION}")
set(OSMSCOUT_SAILFISH_VERSION_STRING "${OSMSCOUT_SAILFISH_VERSION_SHORT}${OSMSCOUT_SAILFISH_SUFFIX_VERSION}")

# ==================================================================================================

set(CMAKE_MIN_VERSION    "2.8.11")
set(QT_MIN_VERSION       "5.2.0")
option(OSMSCOUT_ENABLE_SSE "Enable SSE support (not working on all platforms!)" ON)

cmake_minimum_required(VERSION ${CMAKE_MIN_VERSION})

list(APPEND CMAKE_MODULE_PATH "${CMAKE_HOME_DIRECTORY}/CMakeMod")

# Cmake macros
include(FeatureSummary)

# Local macros
include(MacroUtils)

# feature detection (libraries, header, etc.)
set(OSMSCOUT_ENABLE_SSE true)
include(CheckCXXCompilerFlag)
include(features)

# ==================================================================================================
# Find dependencies

find_package( Qt5 ${QT_MIN_VERSION} 
              REQUIRED NO_MODULE COMPONENTS 
              Core
              Network
              Gui
              Positioning
              Xml
              Svg
              Qml
              Quick
              Positioning
)
find_package(Qt5LinguistTools)
find_package(OpenMP REQUIRED)
#find_package(LibXml2)
#find_package(BZip2)
find_package(SailfishApp) # https://github.com/sailfish-sdk/libsailfishapp 

# ==================================================================================================
# Dependencies Rules
# TODO

# ==================================================================================================

# ==================================================================================================
# Print build configuration

## print all variables
#message(STATUS "contractionhierarchies: "contractionhierarchies)
#get_cmake_property(_variableNames VARIABLES)
#foreach (_variableName ${_variableNames})
#    message(STATUS "${_variableName}=${${_variableName}}")
#endforeach()

message(STATUS "")
message(STATUS "----------------------------------------------------------------------------------")
message(STATUS "OSMScout-Sailfish ${OSMSCOUT_SAILFISH_VERSION_STRING} build configuration results")

message(STATUS "")
message(STATUS "Requiered dependencies:")

PRINT_LIBRARY_STATUS("Qt5"                    "http://www.qtsoftware.com"                       "(version >= ${QT_MIN_VERSION})"     Qt5_FOUND)
#PRINT_LIBRARY_STATUS("libprotobuf"            "https://github.com/google/protobuf"             ""      PROTOBUF_FOUND)
PRINT_LIBRARY_STATUS("OpenMP compiler support" "http://openmp.org/wp/openmp-compilers/"         ""      OPENMP_FOUND)
PRINT_LIBRARY_STATUS("SailfishApp library"     "https://github.com/sailfish-sdk/libsailfishapp" ""      LIBSAILFISHAPP_FOUND)

message(STATUS "")

if(Qt5_FOUND AND
  LIBSAILFISHAPP_FOUND
  )
    message(STATUS      " OSMScout-Sailfish can be compiled........ YES")
    set(OSMSCOUT_SAILFISH_CAN_BE_COMPILED true)
else()
    message(FATAL_ERROR " OSMScout-Sailfish will be compiled....... NO  (Look README file for more details about dependencies)")
    set(OSMSCOUT_SAILFISH_CAN_BE_COMPILED false)
endif()

message(STATUS "----------------------------------------------------------------------------------")
message(STATUS "")

# ==================================================================================================

if (OSMSCOUT_SAILFISH_CAN_BE_COMPILED)

  add_definitions( -DOSMSCOUT_SAILFISH_VERSION_STRING="${OSMSCOUT_SAILFISH_VERSION_STRING}")
  if (QT_QML_DEBUG)
    add_definitions( -DQT_QML_DEBUG)
  endif()

  add_definitions( -Wall -pedantic -fPIC -Wno-unused-function)
  add_definitions(-std=c++11)

  # QT5 definitions
  add_definitions(${QT_DEFINITIONS})
  add_definitions(${Qt5Core_DEFINITIONS})

  add_definitions(${LIBXML2_DEFINITIONS})
  add_definitions(${LIBSAILFISHAPP_DEFINITIONS})

  # OpenMP definitions
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")

  set(HARBOUR_APP_NAME harbour-osmscout)
endif()


# ==================================================================================================
# libmarisa

set(MARISA_ROOT ${CMAKE_HOME_DIRECTORY}/dependencies/marisa-0.2.4)
set(MARISA_INCLUDE_DIRS ${CMAKE_HOME_DIRECTORY}/dependencies/marisa-0.2.4/lib)

set(MARISA_SRC
    ${MARISA_ROOT}/lib/marisa/grimoire/io/reader.cc
    ${MARISA_ROOT}/lib/marisa/grimoire/io/mapper.cc
    ${MARISA_ROOT}/lib/marisa/grimoire/io/writer.cc
    ${MARISA_ROOT}/lib/marisa/grimoire/trie/tail.cc
    ${MARISA_ROOT}/lib/marisa/grimoire/trie/louds-trie.cc
    ${MARISA_ROOT}/lib/marisa/grimoire/vector/bit-vector.cc
    ${MARISA_ROOT}/lib/marisa/trie.cc
    ${MARISA_ROOT}/lib/marisa/agent.cc
    ${MARISA_ROOT}/lib/marisa/keyset.cc
)

add_library(marisa SHARED 
    ${MARISA_SRC}
)
install(TARGETS marisa
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib)

set(MARISA_FOUND TRUE)
set(MARISA_LIBRARIES marisa)

# ==================================================================================================

set(OSMSCOUT_BASE_DIR_BUILD ${CMAKE_CURRENT_BINARY_DIR})
set(OSMSCOUT_BASE_DIR_SOURCE ${CMAKE_HOME_DIRECTORY}/dependencies/libosmscout)

include_directories(
  ${OSMSCOUT_BASE_DIR_BUILD}/include/
  ${MARISA_ROOT}/lib
)


# QT5 definitions
add_definitions(${QT_DEFINITIONS})
add_definitions(${Qt5Core_DEFINITIONS})

add_subdirectory(dependencies/libosmscout/libosmscout)
add_subdirectory(dependencies/libosmscout/libosmscout-map)
add_subdirectory(dependencies/libosmscout/libosmscout-map-qt)
add_subdirectory(dependencies/libosmscout/libosmscout-client-qt)

# ==================================================================================================
# harbour-osmscout

# OSMScout includes Config.h
include_directories(
  ${OSMSCOUT_BASE_DIR_BUILD}/dependencies/libosmscout/libosmscout/include/
)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(HEADER_FILES
#    src/DBThread.h
)

set(LANG_FILES
    translations/en.ts

    translations/cs.ts
    translations/hu.ts
)

QT5_CREATE_TRANSLATION(LANG_SRC ${LANG_FILES})
ADD_CUSTOM_TARGET (translations ALL DEPENDS ${LANG_SRC})

set(SOURCE_FILES
    src/OSMScout.cpp
    
    # keep qml files in source list - it makes qtcreator happy
    # find qml -type f
    qml/AboutDialog.qml
    qml/SearchDialog.qml
    qml/custom/DialogActionButton.qml
    qml/custom/LineEdit.qml
    qml/custom/LocationSearch.qml
    qml/custom/MapButton.qml
    qml/custom/MapDialog.qml
    qml/custom/ScrollIndicator.qml
    qml/custom/MapRenderingIndicator.qml
    qml/custom/OSMCopyright.qml
    qml/custom/MapComponent.qml
    qml/custom/Link.qml
    qml/custom/ScaleIndicator.qml
    qml/custom/POIIcon.qml
    qml/pages/Map.qml
    qml/pages/PlaceDetail.qml
    qml/pages/Cover.qml
    qml/pages/About.qml
    qml/pages/Search.qml
    qml/pages/Layers.qml
    qml/main.qml
    qml/desktop.qml

    ${LANG_FILES}
)

add_executable(harbour-osmscout ${SOURCE_FILES} ${HEADER_FILES})

target_include_directories(harbour-osmscout PRIVATE 
		${OSMSCOUT_BASE_DIR_SOURCE}/libosmscout/include
		${OSMSCOUT_BASE_DIR_SOURCE}/libosmscout-map/include
		${OSMSCOUT_BASE_DIR_SOURCE}/libosmscout-map-qt/include
		${OSMSCOUT_BASE_DIR_SOURCE}/libosmscout-client-qt/include
    ${LIBSAILFISHAPP_INCLUDE_DIRS}
)

target_link_libraries(harbour-osmscout
        Qt5::Core
        Qt5::Gui
        Qt5::Qml
        Qt5::Quick
        Qt5::Svg
        Qt5::Positioning
        
        marisa
        osmscout
        osmscout_map
        osmscout_map_qt
        osmscout_client_qt
        ${LIBSAILFISHAPP_LIBRARIES}
)

# https://github.com/sailfish-sdk/cmakesample/blob/master/CMakeLists.txt
install(TARGETS harbour-osmscout
RUNTIME DESTINATION bin)

install(DIRECTORY qml
DESTINATION share/${HARBOUR_APP_NAME})

install(DIRECTORY resources
DESTINATION share/${HARBOUR_APP_NAME})

install(DIRECTORY pics
DESTINATION share/${HARBOUR_APP_NAME})

install(FILES ${LANG_SRC}
DESTINATION share/${HARBOUR_APP_NAME}/translations)

install(FILES harbour-osmscout.desktop
DESTINATION share/applications)

install(FILES icons/86x86/harbour-osmscout.png
DESTINATION share/icons/hicolor/86x86/apps)

install(FILES icons/108x108/harbour-osmscout.png
DESTINATION share/icons/hicolor/108x108/apps)

install(FILES icons/128x128/harbour-osmscout.png
DESTINATION share/icons/hicolor/128x128/apps)

install(FILES icons/256x256/harbour-osmscout.png
DESTINATION share/icons/hicolor/256x256/apps)

install(DIRECTORY icons/poi/
DESTINATION share/${HARBOUR_APP_NAME}/poi-icons
FILES_MATCHING PATTERN "*.svg")

install(FILES 
  dependencies/libosmscout/libosmscout/data/icons/14x14/standard/bus_stop.png
  dependencies/libosmscout/libosmscout/data/icons/14x14/standard/cemetery.png
  dependencies/libosmscout/libosmscout/data/icons/14x14/standard/forest.png
  dependencies/libosmscout/libosmscout/data/icons/14x14/standard/hospital.png
  dependencies/libosmscout/libosmscout/data/icons/14x14/standard/parking.png
  dependencies/libosmscout/libosmscout/data/icons/14x14/standard/pharmacy.png
  dependencies/libosmscout/libosmscout/data/icons/14x14/standard/scrub.png
DESTINATION share/${HARBOUR_APP_NAME}/map-icons)

install(FILES 
  stylesheets/boundaries.oss
  stylesheets/motorways.oss
  stylesheets/railways.oss
  stylesheets/standard.oss
DESTINATION share/${HARBOUR_APP_NAME}/map-styles)

# ==================================================================================================
# PerformanceTest binary
set(SOURCE_FILES
    src/PerformanceTest.cpp
)

add_executable(PerformanceTest ${SOURCE_FILES})

target_include_directories(PerformanceTest PRIVATE 
		${OSMSCOUT_BASE_DIR_SOURCE}/libosmscout/include
		${OSMSCOUT_BASE_DIR_SOURCE}/libosmscout-map/include
		${OSMSCOUT_BASE_DIR_SOURCE}/libosmscout-map-qt/include
		${OSMSCOUT_BASE_DIR_SOURCE}/libosmscout-client-qt/include
    ${LIBSAILFISHAPP_INCLUDE_DIRS}
)

target_link_libraries(PerformanceTest
        Qt5::Core
        Qt5::Gui
        Qt5::Quick # requiered by sailfishapp when configred with QT_QML_DEBUG
        
        marisa
        osmscout
        osmscout_map
        osmscout_map_qt
        ${LIBSAILFISHAPP_LIBRARIES}
)

