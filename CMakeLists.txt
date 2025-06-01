cmake_minimum_required(VERSION 3.16)

project(FlickInstaller VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core Quick)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Quick)

set(PROJECT_SOURCES
        main.cpp
        utild.cpp
        utild.h
        InstallerWorker.cpp
        InstallerWorker.h
        qml.qrc
)
if (ANDROID)
    include(FetchContent)
    FetchContent_Declare(
        android_openssl
        DOWNLOAD_EXTRACT_TIMESTAMP true
        URL https://github.com/KDAB/android_openssl/archive/refs/heads/master.zip
    )
    FetchContent_MakeAvailable(android_openssl)
    include(${android_openssl_SOURCE_DIR}/android_openssl.cmake)
endif()

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(FlickInstaller
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        android/AndroidManifest.xml android/build.gradle android/res/values/libs.xml android/res/xml/qtprovider_paths.xml android/gradle/wrapper/gradle-wrapper.jar android/gradle/wrapper/gradle-wrapper.properties android/gradle.properties android/gradlew android/gradlew.bat
    )
# Define target properties for Android with Qt 6 as:
   set_property(TARGET FlickInstaller APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
                ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(FlickInstaller SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(FlickInstaller
          ${PROJECT_SOURCES}
        )
    endif()
endif()
if (ANDROID)
    add_android_openssl_libraries(FlickInstaller)
endif()
target_link_libraries(FlickInstaller
  PRIVATE Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Quick)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.FlickInstaller)
endif()
set_target_properties(FlickInstaller PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS FlickInstaller
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})

if(QT_VERSION_MAJOR EQUAL 6)
    qt_import_qml_plugins(FlickInstaller)
    qt_finalize_executable(FlickInstaller)
endif()
