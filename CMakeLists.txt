cmake_minimum_required(VERSION 3.16)

# KDE Application Version, managed by release script
set(RELEASE_SERVICE_VERSION_MAJOR "21")
set(RELEASE_SERVICE_VERSION_MINOR "08")
set(RELEASE_SERVICE_VERSION_MICRO "0")
set(RELEASE_SERVICE_VERSION "${RELEASE_SERVICE_VERSION_MAJOR}.${RELEASE_SERVICE_VERSION_MINOR}.${RELEASE_SERVICE_VERSION_MICRO}")

project(kwordquiz VERSION ${RELEASE_SERVICE_VERSION})

if(NOT MSVC)
    set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-Wall -Wno-switch -O2 -g")
    set(CMAKE_CXX_FLAGS_RELEASE "-Wall -Wno-switch -O2")
    set(CMAKE_CXX_FLAGS_DEBUG  "-Wall -Wno-switch -g3 -ggdb -O0" CACHE STRING "Debug options." FORCE)
endif(NOT MSVC)

include(FeatureSummary)
find_package(Qt5 CONFIG REQUIRED Core Widgets PrintSupport)

find_package(ECM 5.83.0 NO_MODULE REQUIRED)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${ECM_MODULE_PATH} )

include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings NO_POLICY_SCOPE)
include(ECMInstallIcons)
include(ECMAddAppIcon)
include(ECMSetupVersion)


find_package(KF5 REQUIRED COMPONENTS
    I18n
    Crash
    Config
    ConfigWidgets
    DocTools
    GuiAddons
    ItemViews
    NotifyConfig
    NewStuff
    Notifications
    WindowSystem
    XmlGui
)
include_directories (${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR} )

find_package(Phonon4Qt5 4.6.60 NO_MODULE REQUIRED)

find_package(LibKEduVocDocument REQUIRED)
add_definitions(-DQT_USE_FAST_CONCATENATION -DQT_USE_FAST_OPERATOR_PLUS)
add_definitions(-DQT_NO_URL_CAST_FROM_STRING)


add_subdirectory(doc)
add_subdirectory(src)

ki18n_install(po)
if (KF5DocTools_FOUND)
    kdoctools_install(po)
endif()
install(FILES org.kde.kwordquiz.appdata.xml DESTINATION ${KDE_INSTALL_METAINFODIR})
feature_summary(WHAT ALL INCLUDE_QUIET_PACKAGES FATAL_ON_MISSING_REQUIRED_PACKAGES)
