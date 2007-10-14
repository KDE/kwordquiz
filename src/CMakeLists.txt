
add_subdirectory( pics ) 
add_subdirectory( examples ) 

include_directories( ${CMAKE_SOURCE_DIR}/libkdeedu/keduvocdocument )


########### next target ###############

set(kwordquiz_SRCS 
   kwordquizprefs.cpp 
   kwordquiz.cpp 
   main.cpp 
   qaview.cpp 
   flashview.cpp 
   multipleview.cpp 
   kwqquiz.cpp 
   wqscore.cpp 
   prefeditor.cpp 
   prefquiz.cpp
   dlgrc.cpp 
   dlgspecchar.cpp 
   dlglanguage.cpp 
   wqprintdialogpage.cpp 
   prefcharacter.cpp 
   wqundo.cpp 
   prefcardappearance.cpp 
   kwqtablemodel.cpp
   kwqtableview.cpp
   kwqtabledelegate.cpp
   kwqsortfiltermodel.cpp
   )


kde4_add_ui_files(kwordquiz_SRCS 
   dlglanguagebase.ui 
   flashviewbase.ui 
   multipleviewbase.ui 
   qaviewbase.ui 
   prefeditorbase.ui 
   prefquizbase.ui
   dlgrcbase.ui 
   prefcharacterbase.ui 
   prefcardappearancebase.ui )

kde4_add_kcfg_files(kwordquiz_SRCS prefs.kcfgc )

kde4_add_executable(kwordquiz ${kwordquiz_SRCS})

target_link_libraries(kwordquiz  keduvocdocument ${KDE4_KIO_LIBS} ${KDE4_KNEWSTUFF2_LIBS} ${KDE4_KNOTIFYCONFIG_LIBS})

install(TARGETS kwordquiz  DESTINATION ${BIN_INSTALL_DIR} )


########### install files ###############

install( FILES kwordquiz.desktop  DESTINATION  ${XDG_APPS_INSTALL_DIR} )
install( FILES kwordquiz.kcfg  DESTINATION  ${KCFG_INSTALL_DIR} )
install( FILES kwordquiz.knsrc  DESTINATION  ${CONFIG_INSTALL_DIR} )
install( FILES kwordquizui.rc kwordquiz.notifyrc  DESTINATION  ${DATA_INSTALL_DIR}/kwordquiz )

