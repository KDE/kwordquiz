
add_subdirectory( pics ) # private icons
add_subdirectory( icons ) # global icons
add_subdirectory( examples )

# include_directories( ${LIBKDEEDU_INCLUDE_DIR} )

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
)

########### next target ###############

ecm_setup_version(${RELEASE_SERVICE_VERSION} VARIABLE_PREFIX KWORDQUIZ VERSION_HEADER kwordquiz_version.h)

set(kwordquiz_SRCS
    main.cpp
    kwqcardmodel.h
    kwqcardmodel.cpp
    kwqdocumentmodel.h
    kwqdocumentmodel.cpp
    kwqrandomsortmodel.h
    kwqrandomsortmodel.cpp
    exporter.h
    exporter.cpp
    audioprober.h
    audioprober.cpp
    blankanswer.h
    blankanswer.cpp
    resources.qrc
)

kconfig_add_kcfg_files(kwordquiz_SRCS GENERATE_MOC prefs.kcfgc documentsettings.kcfgc)

file(GLOB ICONS_SRCS "${CMAKE_CURRENT_SOURCE_DIR}/icons/*-apps-kwordquiz.png")
ecm_add_app_icon(kwordquiz_SRCS ICONS ${ICONS_SRCS})

add_executable(kwordquiz ${kwordquiz_SRCS})

target_link_libraries(kwordquiz
    Qt::Core
    Qt::Quick
    Qt::PrintSupport
    Qt::Svg
    Qt::Multimedia
    KF${KF_MAJOR_VERSION}::Crash
    KF${KF_MAJOR_VERSION}::ConfigCore
    KF${KF_MAJOR_VERSION}::ConfigGui
    KF${KF_MAJOR_VERSION}::I18n
    KF${KF_MAJOR_VERSION}::NewStuffCore
    KF${KF_MAJOR_VERSION}::Notifications
    KF${KF_MAJOR_VERSION}::NotifyConfig
    LibKEduVocDocumentImport__KEduVocDocument
)

install(TARGETS kwordquiz ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})

########### install files ###############

install(PROGRAMS org.kde.kwordquiz.desktop DESTINATION ${KDE_INSTALL_APPDIR})
install(FILES kwordquiz.kcfg DESTINATION ${KDE_INSTALL_KCFGDIR})
install(FILES kwordquiz.knsrc DESTINATION ${KDE_INSTALL_KNSRCDIR})
install(FILES kwordquiz.notifyrc DESTINATION ${KDE_INSTALL_KNOTIFYRCDIR})

