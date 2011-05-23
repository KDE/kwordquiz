
add_subdirectory( pics ) # private icons
add_subdirectory( icons ) # global icons
add_subdirectory( examples )

find_package(LibKdeEdu REQUIRED)
include_directories( ${LIBKEDUVOCDOCUMENT_INCLUDE_DIR} )


########### next target ###############

set(kwordquiz_SRCS
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
   )


kde4_add_ui_files(kwordquiz_SRCS 
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

kde4_add_kcfg_files(kwordquiz_SRCS prefs.kcfgc documentsettings.kcfgc)

#uncomment this if oxygen icons for kwordquiz are available

#kde4_add_app_icon(kwordquiz_SRCS "${KDE4_ICON_DIR}/oxygen/*/apps/kwordquiz.png")
kde4_add_app_icon(kwordquiz_SRCS "${CMAKE_CURRENT_SOURCE_DIR}/icons/hi*-app-kwordquiz.png")

kde4_add_executable(kwordquiz ${kwordquiz_SRCS})

target_link_libraries(kwordquiz  ${LIBKDEEDU_KEDUVOCDOCUMENT_LIBRARIES} ${KDE4_KIO_LIBS} ${KDE4_KNEWSTUFF3_LIBS} ${KDE4_KNOTIFYCONFIG_LIBS} ${KDE4_KUTILS_LIBS} ${KDE4_PHONON_LIBRARY})

install(TARGETS kwordquiz  ${INSTALL_TARGETS_DEFAULT_ARGS} )


########### install files ###############

install( PROGRAMS kwordquiz.desktop  DESTINATION  ${XDG_APPS_INSTALL_DIR} )
install( FILES kwordquiz.kcfg  DESTINATION  ${KCFG_INSTALL_DIR} )
install( FILES kwordquiz.knsrc  DESTINATION  ${CONFIG_INSTALL_DIR} )
install( FILES kwordquizui.rc kwordquiz.notifyrc  DESTINATION  ${DATA_INSTALL_DIR}/kwordquiz )

