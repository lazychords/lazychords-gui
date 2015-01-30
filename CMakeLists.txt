CMAKE_MINIMUM_REQUIRED(VERSION 2.8.8)
ENABLE_TESTING()

PROJECT(LazyChords)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR})


FIND_PACKAGE(Boost REQUIRED)


INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIRS})

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -O3 -Wall -Wextra")

ADD_DEFINITIONS(-DNOT_USE_ML)


############################
# LIBS Output
############################

INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/libs/)
include(ExternalProject)
ExternalProject_Add(output
  DOWNLOAD_COMMAND echo "download"
  PREFIX ${PROJECT_SOURCE_DIR}/libs/output
  SOURCE_DIR ${PROJECT_SOURCE_DIR}/libs/output
  BINARY_DIR ${PROJECT_SOURCE_DIR}/libs/output
  INSTALL_COMMAND echo "success"
)
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/libs/liblazychord/src/)

############################
# LazyChords Lib
############################

ExternalProject_Add(liblazychord
  DOWNLOAD_COMMAND echo "download"
  PREFIX ${PROJECT_SOURCE_DIR}/libs/liblazychord
  SOURCE_DIR ${PROJECT_SOURCE_DIR}/libs/liblazychord
  BINARY_DIR ${PROJECT_SOURCE_DIR}/libs/liblazychord
  INSTALL_COMMAND echo "success"
)
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/libs/liblazychord/src/)



############################
# GUI
############################
set(CMAKE_AUTOMOC ON)
find_package(Qt5Core)
find_package(Qt5Gui)
find_package(Qt5Widgets)
if(Qt5Widgets_FOUND AND Qt5Gui_FOUND AND Qt5Core_FOUND)
    SET(CMAKE_INCLUDE_CURRENT_DIR ON)

    SET(gui_SRCS
        ${PROJECT_SOURCE_DIR}/src/mainwindow.cpp
        ${PROJECT_SOURCE_DIR}/src/main.cpp
        ${PROJECT_SOURCE_DIR}/src/myslider.cpp
    )
    SET(gui_QRCS
        ${PROJECT_SOURCE_DIR}/res/icons.qrc
        ${PROJECT_SOURCE_DIR}/res/rendering.qrc
    )

    QT5_WRAP_UI(UIS_HDRS
        ${PROJECT_SOURCE_DIR}/src/mainwindow.ui
    )
    QT5_ADD_RESOURCES (QRCS ${gui_QRCS})

    INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/src/)

    ADD_EXECUTABLE(LazyChords ${gui_SRCS} ${UIS_HDRS} ${QRCS})

    ADD_DEPENDENCIES(LazyChords liblazychord output)
    QT5_USE_MODULES(LazyChords Widgets Core Gui WebKitWidgets)
    TARGET_LINK_LIBRARIES(LazyChords ABCToMidi)
    TARGET_LINK_LIBRARIES(LazyChords LazyChordsLib)
    TARGET_LINK_LIBRARIES(LazyChords m)
    IF(USE_ML)
        TARGET_LINK_LIBRARIES(LazyChords ${Boost_PROGRAM_OPTIONS_LIBRARY} ${Boost_LIBRARIES})
    ENDIF()

ELSE()
    FATAL("Qt5 was not found on your system.")
ENDIF()
