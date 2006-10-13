
add_subdirectory( pics ) 
add_subdirectory( examples ) 

include_directories( ${CMAKE_SOURCE_DIR}/libkdeedu/kdeeducore )


########### next target ###############

set(kwordquiz_SRCS 
   kwordquizprefs.cpp 
   kwordquiz.cpp 
   main.cpp 
   qaview.cpp 
   flashview.cpp 
   multipleview.cpp 
   wqquiz.cpp 
   wqscore.cpp 
   prefeditor.cpp 
   prefquiz.cpp 
   dlgsort.cpp 
   dlgrc.cpp 
   dlgspecchar.cpp 
   dlglanguage.cpp 
   wqprintdialogpage.cpp 
   prefcharacter.cpp 
   wqundo.cpp 
   prefcardappearance.cpp 
   kwqnewstuff.cpp 
   kwqnewstuff.h 
   kwqtablemodel.cpp 
   kwqtableview.cpp 
   kwqtabledelegate.cpp
   )

kde4_automoc(kwordquiz ${kwordquiz_SRCS})

kde4_add_ui_files(kwordquiz_SRCS 
   dlglanguagebase.ui 
   flashviewbase.ui 
   multipleviewbase.ui 
   qaviewbase.ui 
   prefeditorbase.ui 
   prefquizbase.ui 
   dlgsortbase.ui 
   dlgrcbase.ui 
   prefcharacterbase.ui 
   prefcardappearancebase.ui )

kde4_add_kcfg_files(kwordquiz_SRCS prefs.kcfgc )

kde4_add_executable(kwordquiz ${kwordquiz_SRCS})

target_link_libraries(kwordquiz  kdeeducore ${KDE4_KDEUI_LIBS} ${KDE4_KDEPRINT_LIBS} ${KDE4_KNEWSTUFF_LIBS} knotifyconfig)

install(TARGETS kwordquiz  DESTINATION ${BIN_INSTALL_DIR} )


########### install files ###############

install( FILES kwordquiz.desktop  DESTINATION  ${XDG_APPS_DIR} )
install( FILES kwordquiz.kcfg  DESTINATION  ${KCFG_INSTALL_DIR} )
install( FILES kwordquizrc  DESTINATION  ${CONFIG_INSTALL_DIR} )
install( FILES kwordquizui.rc kwordquiz.notifyrc  DESTINATION  ${DATA_INSTALL_DIR}/kwordquiz )
install( FILES x-kwordquiz.desktop  DESTINATION  ${MIME_INSTALL_DIR}/application )

