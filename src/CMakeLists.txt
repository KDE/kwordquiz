
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
    blankanswer.cpp
    dlglanguage.cpp
    dlglanguage.h
    dlgspecchar.cpp
    dlgspecchar.h
    filterproxysearchline.cpp
    filterproxysearchline.h
    flashview.cpp
    flashview.h
    kwordquiz.cpp
    kwordquiz.h
    kwordquizprefs.cpp
    kwordquizprefs.h
    kwqcardscene.cpp
    kwqcardscene.h
    kwqcardview.cpp
    kwqcardview.h
    kwqcleardialog.cpp
    kwqcleardialog.h
    kwqcommands.cpp
    kwqcommands.h
    kwqpixmapitem.cpp
    kwqpixmapitem.h
    kwqquizmodel.cpp
    kwqquizmodel.h
    kwqquizview.cpp
    kwqquizview.h
    kwqscorewidget.cpp
    kwqscorewidget.h
    kwqsortfiltermodel.cpp
    kwqsortfiltermodel.h
    kwqtabledelegate.cpp
    kwqtabledelegate.h
    kwqtablemodel.cpp
    kwqtablemodel.h
    kwqtableview.cpp
    kwqtableview.h
    kwqtutor.cpp
    kwqtutorflashcard.cpp
    kwqtutorflashcard.h
    kwqtutor.h
    kwqtutorprefs.cpp
    kwqtutorprefs.h
    main.cpp
    multipleview.cpp
    multipleview.h
    prefcardappearance.cpp
    prefcardappearance.h
    prefcharacter.cpp
    prefcharacter.h
    prefeditor.cpp
    prefeditor.h
    prefgeneral.cpp
    prefgeneral.h
    prefquiz.cpp
    prefquiz.h
    preftutor.cpp
    preftutor.h
    qaview.cpp
    qaview.h
    wqprintdialogpage.cpp
    wqprintdialogpage.h
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
    KF5::NewStuffWidgets
    KF5::Notifications
    KF5::NotifyConfig
    KF5::XmlGui
    KF5::GuiAddons
    LibKEduVocDocumentImport__KEduVocDocument
    Phonon::phonon4qt${QT_MAJOR_VERSION}
)

install(TARGETS kwordquiz  ${KDE_INSTALL_TARGETS_DEFAULT_ARGS} )


########### install files ###############

install( PROGRAMS org.kde.kwordquiz.desktop  DESTINATION  ${KDE_INSTALL_APPDIR} )
install( FILES kwordquiz.kcfg  DESTINATION  ${KDE_INSTALL_KCFGDIR} )
install( FILES kwordquiz.knsrc  DESTINATION  ${KDE_INSTALL_KNSRCDIR} )
install( FILES kwordquiz.notifyrc  DESTINATION  ${KDE_INSTALL_KNOTIFYRCDIR} )
install(FILES
  kwordquizui.rc
  DESTINATION  ${KDE_INSTALL_KXMLGUIDIR}/kwordquiz)

