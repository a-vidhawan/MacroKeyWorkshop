cmake_minimum_required(VERSION 3.16)

project(MacroPad VERSION 0.1 LANGUAGES CXX)

message(STATUS "CMAKE_SOURCE_DIR = ${CMAKE_SOURCE_DIR}")

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_VERBOSE_MAKEFILE ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Quick QuickWidgets SerialPort)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Quick QuickWidgets SerialPort)

set(PROJECT_SOURCES
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)

    if(APPLE)
        set(APPTRACKER_SOURCES apptracker.h apptracker.mm)  # Use Objective-C++ on macOS
        set_source_files_properties(apptracker.mm PROPERTIES COMPILE_FLAGS "-x objective-c++")  # Ensure ObjC++ compilation
    else()
        set(APPTRACKER_SOURCES apptracker.h apptracker.cpp)  # Use regular C++ for Windows/Linux
    endif()

    qt_add_executable(MacroPad
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        macro.h macro.cpp
        profile.h profile.cpp
        config.h config.cpp
        fileio.h fileio.cpp
        serialhandler.h serialhandler.cpp
        ${APPTRACKER_SOURCES}
        serialhandler.h
        serialhandler.cpp
    )
    qt_add_resources(MacroPad "resources"
        PREFIX "/"
        FILES
            Main.qml
            KeyConfig.qml
            ProfileManager.qml
    )

else()
    if(ANDROID)
        add_library(MacroPad SHARED
            ${PROJECT_SOURCES}
        )
    else()
        add_executable(MacroPad
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_link_libraries(MacroPad PRIVATE Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Quick Qt${QT_VERSION_MAJOR}::QuickWidgets Qt${QT_VERSION_MAJOR}::SerialPort)

# Platform-Specific Configurations
if(APPLE)
    set(MACOSX_BUNDLE_ICON_FILE MPIcon.icns)
    set(MACOSX_BUNDLE_ICON ${CMAKE_SOURCE_DIR}/MPIcon.icns)

    set_source_files_properties(${MACOSX_BUNDLE_ICON} PROPERTIES MACOSX_PACKAGE_LOCATION Resources)

    add_custom_command(TARGET MacroPad POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E echo "Copying .icns icon..."
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${CMAKE_SOURCE_DIR}/MPIcon.icns
            $<TARGET_FILE_DIR:MacroPad>/../Resources/MPIcon.icns
    )
    add_custom_command(TARGET MacroPad POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E echo "Copying .png icon..."
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${CMAKE_SOURCE_DIR}/MPIcon.png
            $<TARGET_FILE_DIR:MacroPad>/../Resources/MPIcon.png
    )

    target_link_libraries(MacroPad PRIVATE "-framework Carbon" "-framework ApplicationServices")
    set_target_properties(MacroPad PROPERTIES
        MACOSX_BUNDLE TRUE
        MACOSX_BUNDLE_GUI_IDENTIFIER com.example.MacroPad
        MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
        MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
        MACOSX_BUNDLE_ICON_FILE MPIcon.icns
    )
elseif(WIN32)
    target_link_libraries(MacroPad PRIVATE user32.lib)
    set_target_properties(MacroPad PROPERTIES WIN32_EXECUTABLE TRUE)
elseif(UNIX AND NOT APPLE) # Linux
    find_package(X11 REQUIRED)
    target_link_libraries(MacroPad PRIVATE X11)
endif()

include(GNUInstallDirs)
install(TARGETS MacroPad
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(MacroPad)
endif()
