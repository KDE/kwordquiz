cmake_minimum_required(VERSION 3.16)

# KDE Application Version, managed by release script
set(RELEASE_SERVICE_VERSION_MAJOR "23")
set(RELEASE_SERVICE_VERSION_MINOR "07")
set(RELEASE_SERVICE_VERSION_MICRO "80")
set(RELEASE_SERVICE_VERSION "${RELEASE_SERVICE_VERSION_MAJOR}.${RELEASE_SERVICE_VERSION_MINOR}.${RELEASE_SERVICE_VERSION_MICRO}")

project(kwordquiz VERSION ${RELEASE_SERVICE_VERSION})

if(NOT MSVC)
    set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-Wall -Wno-switch -O2 -g")
    set(CMAKE_CXX_FLAGS_RELEASE "-Wall -Wno-switch -O2")
    set(CMAKE_CXX_FLAGS_DEBUG  "-Wall -Wno-switch -g3 -ggdb -O0" CACHE STRING "Debug options." FORCE)
endif(NOT MSVC)

include(FeatureSummary)
set(KF5_MIN_VERSION "5.92.0")
set(REQUIRED_QT_VERSION "5.15.0")
set(KDE_COMPILERSETTINGS_LEVEL "5.82")
find_package(ECM ${KF5_MIN_VERSION} NO_MODULE REQUIRED)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${ECM_MODULE_PATH} )


include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings NO_POLICY_SCOPE)
include(ECMInstallIcons)
include(ECMAddAppIcon)
include(KDEClangFormat)
include(ECMSetupVersion)
include(ECMAddTests)
include(ECMDeprecationSettings)
include(KDEGitCommitHooks)

if (QT_MAJOR_VERSION STREQUAL "6")
    set(QT_REQUIRED_VERSION "6.4.0")
    set(KF_MIN_VERSION "5.240.0")
    set(KF_MAJOR_VERSION "6")
else()
    set(KF_MAJOR_VERSION "5")
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt${QT_MAJOR_VERSION} ${REQUIRED_QT_VERSION} NO_MODULE COMPONENTS
    Core
    Widgets
    PrintSupport
    Quick
    Qml
    Svg
    Multimedia
    Xml
)
set_package_properties(Qt${QT_MAJOR_VERSION} PROPERTIES
    TYPE REQUIRED
    PURPOSE "Base application components"
)

find_package(KF${QT_MAJOR_VERSION} ${KF5_MIN_VERSION} COMPONENTS
    I18n
    Crash
    Config
    DocTools
    NotifyConfig
    NewStuff
    Notifications
    Kirigami2
)
set_package_properties(KF${QT_MAJOR_VERSION} PROPERTIES
    TYPE REQUIRED
    PURPOSE "Base application components"
)

find_package(KF${QT_MAJOR_VERSION}KirigamiAddons 0.9.0)
set_package_properties(KF${QT_MAJOR_VERSION}KirigamiAddons PROPERTIES
    TYPE REQUIRED
    PURPOSE "Kirigami addons application UI framework"
)

include_directories (${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR} )

find_package(LibKEduVocDocument REQUIRED)
add_definitions(-DQT_USE_FAST_CONCATENATION -DQT_USE_FAST_OPERATOR_PLUS)
add_definitions(-DQT_NO_URL_CAST_FROM_STRING)
ecm_set_disabled_deprecation_versions(QT 6.4  KF 5.89.0)

add_subdirectory(doc)
add_subdirectory(src)

if(BUILD_TESTING)
    find_package(Qt${QT_MAJOR_VERSION}Test REQUIRED)

    add_subdirectory(autotests)
endif()

ki18n_install(po)
if (KF5DocTools_FOUND)
    kdoctools_install(po)
endif()

install(FILES org.kde.kwordquiz.appdata.xml DESTINATION ${KDE_INSTALL_METAINFODIR})
install(FILES org.kde.kwordquiz.svg DESTINATION ${KDE_INSTALL_FULL_ICONDIR}/hicolor/scalable/apps)

feature_summary(WHAT ALL INCLUDE_QUIET_PACKAGES FATAL_ON_MISSING_REQUIRED_PACKAGES)

file(GLOB_RECURSE ALL_CLANG_FORMAT_SOURCE_FILES src/*.cpp src/*.h)
kde_clang_format(${ALL_CLANG_FORMAT_SOURCE_FILES})

kde_configure_git_pre_commit_hook(CHECKS CLANG_FORMAT)
