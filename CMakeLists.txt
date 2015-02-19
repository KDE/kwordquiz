cmake_minimum_required(VERSION 2.8.12)

project(kwordquiz)

if(NOT MSVC)
    set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-Wall -Wno-switch -O2 -g")
    set(CMAKE_CXX_FLAGS_RELEASE "-Wall -Wno-switch -O2")
    set(CMAKE_CXX_FLAGS_DEBUG  "-Wall -Wno-switch -g3 -ggdb -O0" CACHE STRING "Debug options." FORCE)
endif(NOT MSVC)

include(FeatureSummary)
find_package(Qt5 CONFIG REQUIRED Core Widgets PrintSupport)

find_package(ECM 1.3.0 NO_MODULE REQUIRED)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${ECM_MODULE_PATH} ${ECM_KDE_MODULE_DIR})

include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings)
include(ECMInstallIcons)


find_package(KF5 REQUIRED COMPONENTS
    I18n
    Crash
    Sonnet
    Config
    ConfigWidgets
    DocTools
    GuiAddons
    IconThemes
    ItemViews
    NotifyConfig
    KIO
    NewStuff
    Notifications
    XmlGui
    KDELibs4Support
    Declarative
)
include_directories (${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR} )

find_package(Phonon4Qt5 4.6.60 NO_MODULE)

find_package(LibKEduVocDocument REQUIRED)
add_definitions(-DQT_USE_FAST_CONCATENATION -DQT_USE_FAST_OPERATOR_PLUS)

add_subdirectory(doc)
add_subdirectory(src)

install(FILES kwordquiz.appdata.xml DESTINATION ${SHARE_INSTALL_PREFIX}/appdata/)
feature_summary(WHAT ALL INCLUDE_QUIET_PACKAGES FATAL_ON_MISSING_REQUIRED_PACKAGES)
