
add_subdirectory(pics) # private icons
add_subdirectory(icons) # global icons
add_subdirectory(examples)

# include_directories( ${LIBKDEEDU_INCLUDE_DIR} )

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
)

########### next target ###############

ecm_setup_version(${RELEASE_SERVICE_VERSION}
    VARIABLE_PREFIX KWORDQUIZ
    VERSION_HEADER kwordquiz_version.h
)

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
    blankanswer.h
    blankanswer.cpp
    fileopener.h
    fileopener.cpp
    languagelistmodel.h
    languagelistmodel.cpp
)

kconfig_add_kcfg_files(kwordquiz_SRCS GENERATE_MOC prefs.kcfgc documentsettings.kcfgc stateprefs.kcfgc)

file(GLOB ICONS_SRCS "${CMAKE_CURRENT_SOURCE_DIR}/icons/*-apps-kwordquiz.png")
ecm_add_app_icon(kwordquiz_SRCS ICONS ${ICONS_SRCS})

add_executable(kwordquiz ${kwordquiz_SRCS})

ecm_add_qml_module(kwordquiz URI org.kde.kwordquiz
    QML_FILES
        qml/Main.qml
        qml/SoundPlayer.qml
        qml/Visualization.qml
        qml/OptionsAction.qml
        qml/EditAction.qml
        qml/BasePage.qml
        qml/WelcomePage.qml
        qml/DeckEditorPage.qml
        qml/FlashCardPage.qml
        qml/QuestionAnswerPage.qml
        qml/MultipleChoicePage.qml
        qml/SettingsPage.qml
        qml/LanguageSelectorPage.qml
)

target_link_libraries(kwordquiz PRIVATE
    Qt6::Core
    Qt6::Quick
    Qt6::QuickControls2
    Qt6::PrintSupport
    Qt6::Svg
    Qt6::Multimedia
    KF6::Crash
    KF6::ConfigCore
    KF6::ConfigGui
    KF6::I18n
    KF6::NewStuffCore
    KF6::DBusAddons
    LibKEduVocDocumentImport__KEduVocDocument
)

if(TARGET KF6::I18nQml)
target_link_libraries(kwordquiz PRIVATE
        KF6::I18nQml
)
endif()

install(TARGETS kwordquiz ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})

########### install files ###############

install(PROGRAMS org.kde.kwordquiz.desktop DESTINATION ${KDE_INSTALL_APPDIR})
install(FILES kwordquiz.kcfg DESTINATION ${KDE_INSTALL_KCFGDIR})
install(FILES kwordquiz.knsrc DESTINATION ${KDE_INSTALL_KNSRCDIR})

