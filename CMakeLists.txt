cmake_minimum_required(VERSION 3.10)

# Remove when sharing with others.
#list(APPEND CMAKE_PREFIX_PATH "C:/Qt/Tools/QtCreator")

project(Vcreator)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_CXX_STANDARD 17)

find_package(QtCreator COMPONENTS Core REQUIRED)
find_package(QT NAMES Qt6 Qt5 COMPONENTS Widgets REQUIRED)
set(QtX Qt${QT_VERSION_MAJOR})

add_qtc_plugin(Vcreator
  PLUGIN_DEPENDS
    QtCreator::Core QtCreator::LanguageClient QtCreator::ProjectExplorer QtCreator::TextEditor
  DEPENDS
    ${QtX}::Widgets
    QtCreator::ExtensionSystem
    QtCreator::Utils
  SOURCES
    .github/workflows/build_cmake.yml
    .github/workflows/README.md
    README.md
    vcreator.qrc
    vcreatorplugin.cpp
    vcreatorplugin.h
    vcreator_global.h
    vcreatorconstants.h
    vcreatorproject.cpp
    vcreatorproject.h
    vcreatorsettings.cpp
    vcreatorsettings.h
    vcreatoreditor.cpp
    vcreatoreditor.h
    vcreatorindenter.cpp
    vcreatorindenter.h
    vcreatorhighlighter.cpp
    vcreatorhighlighter.h
    vcreatorlexer.cpp
    vcreatorlexer.h
)

add_subdirectory(share/qtcreator)
