cmake_minimum_required(VERSION 3.16)

project(Flick-Installer VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
option(STATIC_LINK "Enable static linking" ON)
option(ENABLE_TESTS "GTests" OFF)
if(STATIC_LINK)
  set(BUILD_SHARED_LIBS OFF)
  if (WIN32)
      set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
  else()
      set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static")
  endif()
else()
  set(BUILD_SHARED_LIBS ON)
endif()
find_package(Qt6 REQUIRED COMPONENTS Quick)
find_package(Qt6 REQUIRED COMPONENTS Network)
find_package(Qt6 REQUIRED COMPONENTS Core)

qt_standard_project_setup(REQUIRES 6.8)

qt_add_executable(Flick-Installer
    main.cpp
)

qt_add_qml_module(Flick-Installer
    URI RC
    VERSION 1.0
    QML_FILES
        Main.qml
        SOURCES utild.h utild.cpp
        SOURCES InstallerWorker.h
        SOURCES InstallerWorker.cpp
        RESOURCES rs.qrc
)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
set_target_properties(Flick-Installer PROPERTIES
#    MACOSX_BUNDLE_GUI_IDENTIFIER com.example.appRC
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

target_link_libraries(Flick-Installer PRIVATE Qt6::Quick)
target_link_libraries(Flick-Installer PRIVATE Qt6::Network)
target_link_libraries(Flick-Installer PRIVATE Qt6::Core)

include(GNUInstallDirs)
install(TARGETS Flick-Installer
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
