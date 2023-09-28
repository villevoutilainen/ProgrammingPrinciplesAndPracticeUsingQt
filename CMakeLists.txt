cmake_minimum_required(VERSION 3.5)

project(Programming_Qt VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

set(PROJECT_SOURCES
    main.cpp
    PPP/Window.h
    PPP/Graph.h
    PPP/GUI.h
    PPP/Simple_window.h
    PPP/Point.h
    PPP/std_lib_facilities.h
    Window.cpp
    Graph.cpp
    PPP/Image_private.h
    GUI.cpp
    PPP/GUI_private.h
    Simple_window.cpp
    Colormap.cpp
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(Programming_Qt
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )

# Define target properties for Android with Qt 6 as:
#    set_property(TARGET Programming_Qt APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(Programming_Qt SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(Programming_Qt
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_link_libraries(Programming_Qt PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)
target_include_directories(Programming_Qt PRIVATE .)

set_target_properties(Programming_Qt PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

if(MSVC)
  target_compile_options(Programming_Qt PRIVATE /EHsc /W4 /WX)
else()
  target_compile_options(Programming_Qt PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()

if (EMSCRIPTEN)
    target_link_options(Programming_Qt PUBLIC -sASYNCIFY -Os)
endif()

install(TARGETS Programming_Qt
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(Programming_Qt)
endif()
