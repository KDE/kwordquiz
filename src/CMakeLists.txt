
add_subdirectory( pics ) # private icons
add_subdirectory( icons ) # global icons
add_subdirectory( examples )

# include_directories( ${LIBKDEEDU_INCLUDE_DIR} )

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
    ${PHONON_INCLUDES}
)

########### next target ###############

ecm_setup_version(${RELEASE_SERVICE_VERSION} VARIABLE_PREFIX KWORDQUIZ VERSION_HEADER kwordquiz_version.h)

set(kwordquiz_SRCS
   filterproxysearchline.cpp
   kwordquizprefs.cpp
   kwordquiz.cpp
   main.cpp
   qaview.cpp
   flashview.cpp
   multipleview.cpp
   kwqquizmodel.cpp
   prefgeneral.cpp
   prefeditor.cpp
   prefquiz.cpp
   preftutor.cpp
   dlgspecchar.cpp
   dlglanguage.cpp
   wqprintdialogpage.cpp
   prefcharacter.cpp
   prefcardappearance.cpp
   kwqtablemodel.cpp
   kwqtableview.cpp
   kwqtabledelegate.cpp
   kwqtutor.cpp
   kwqtutorflashcard.cpp
   kwqtutorprefs.cpp
   kwqsortfiltermodel.cpp
   kwqcommands.cpp
   kwqscorewidget.cpp
   kwqcardscene.cpp
   kwqcardview.cpp
   kwqquizview.cpp
   kwqcleardialog.cpp
   kwqpixmapitem.cpp
   )


ki18n_wrap_ui(kwordquiz_SRCS
   columndialogbase.ui
   flashviewbase.ui
   multipleviewbase.ui
   qaviewbase.ui
   prefeditorbase.ui
   prefquizbase.ui
   kwqscorewidgetbase.ui
   prefcharacterbase.ui
   prefcardappearancebase.ui
   prefgeneralbase.ui
   preftutorbase.ui
   cleardialogbase.ui
   )

kconfig_add_kcfg_files(kwordquiz_SRCS prefs.kcfgc documentsettings.kcfgc)

file(GLOB ICONS_SRCS "${CMAKE_CURRENT_SOURCE_DIR}/icons/*-apps-kwordquiz.png")
ecm_add_app_icon(kwordquiz_SRCS ICONS ${ICONS_SRCS})

add_executable(kwordquiz ${kwordquiz_SRCS})

target_link_libraries(kwordquiz
    Qt::Core
    Qt::PrintSupport
    Qt::Widgets
    KF5::Crash
    KF5::ConfigCore
    KF5::I18n
    KF5::ItemViews
    KF5::NewStuff
    KF5::Notifications
    KF5::NotifyConfig
    KF5::XmlGui
    KF5::GuiAddons
    KF5::WindowSystem
    LibKEduVocDocumentImport__KEduVocDocument
    Phonon::phonon4qt5
)

install(TARGETS kwordquiz  ${KDE_INSTALL_TARGETS_DEFAULT_ARGS} )


########### install files ###############

install( PROGRAMS org.kde.kwordquiz.desktop  DESTINATION  ${KDE_INSTALL_APPDIR} )
install( FILES kwordquiz.kcfg  DESTINATION  ${KDE_INSTALL_KCFGDIR} )
install( FILES kwordquiz.knsrc  DESTINATION  ${KDE_INSTALL_KNSRCDIR} )
install( FILES kwordquiz.notifyrc  DESTINATION  ${KDE_INSTALL_KNOTIFY5RCDIR} )
install(FILES
  kwordquizui.rc
  DESTINATION  ${KDE_INSTALL_KXMLGUI5DIR}/kwordquiz)

