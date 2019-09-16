/***************************************************************************
                                 kwordquiz.h
                             -------------------
    begin                : Wed Jul 24 20:12:30 PDT 2002
    copyright            : (C) 2002-2010 by Peter Hedlund
    email                : peter.hedlund@kdemail.net
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef KWORDQUIZ_H
#define KWORDQUIZ_H

#include <QAction>
#include <QActionGroup>
#include <QUndoStack>
#include <QUrl>

#include <KDirWatch>
#include <KXmlGuiWindow>

#include "filterproxysearchline.h"
#include "kwqquizmodel.h"

class KActionMenu;
class KRecentFilesAction;
class KPageWidget;
class KPageWidgetItem;
class FilterProxySearchLine;
class KEduVocDocument;
class KWQTableModel;
class KWQSortFilterModel;
class KWQTableView;
class KWordQuizPrefs;
class QAView;
class QLabel;
class MultipleView;
class FlashView;

/**
  * The base class for KWordQuiz application windows. It sets up the main
  * window and reads the config file as well as providing a menubar, toolbar
  * and statusbar.
  * KWordQuizApp reimplements the methods that KXmlGuiWindow provides for main window handling and supports
  * full session management.
  * @see KXmlGuiWindow
  * @see KConfig
  */
class KWordQuizApp : public KXmlGuiWindow
{
  Q_OBJECT

  public:
    /**
     * constructor of KWordQuizApp, calls all init functions to create the application.
     */
    explicit KWordQuizApp(QWidget* parent=nullptr);

    /**
     * destructor
     */
    ~KWordQuizApp() override;

    /**
     * opens a file specified by commandline option
     * @param url the URL to be opened
     */
    void openDocumentFile(const QUrl &url=QUrl());

    /**
     * returns a pointer to the current document connected to the KXmlGuiWindow instance
     * @return
     */
    KEduVocDocument * document() const;

    /**
     * @param document the @see KEduVocDocument to save
     * @return whether the saving was successful
     */
    bool saveDocAsFileName(KEduVocDocument * document);

  protected:
    /** save general Options like all bar positions and status as well as the geometry and the recent file list to the configuration
     * file
     */
    void saveOptions();
    /** read general Options again and initialize all variables like the recent file list
     */
    void readOptions();
    /** initializes the KActions of the application */

    void initActions();
    /** sets up the statusbar for the main window by initializing a statuslabel.
     */
    void initStatusBar();
    /** initializes the document object of the main window that is connected to the view in initView().
     * @see initView();
     */
    void initDocument();
    /** creates a data model that reads and writes data from the document and displays it in a view
     * @see initDocument();
     * @see initView();
     */
    void initModel();
    /** creates the centerwidget of the KXmlGuiWindow instance and sets it as the view
     */
    void initView();
    /** queryClose is called by KXmlGuiWindow on each closeEvent of a window. Against the
     * default implementation (only returns true), this calls saveModified() on the document object to ask if the document shall
     * be saved if Modified; on cancel the closeEvent is rejected.
     * @see KXmlGuiWindow#queryClose
     * @see KXmlGuiWindow#closeEvent
     */
    bool queryClose() override;
    /** saves the window properties for each open window during session end to the session config file, including saving the currently
     * opened file by a temporary filename provided by QApplication.
     * @see KXmlGuiWindow#saveProperties
     */
    void saveProperties(KConfigGroup &_cfg) override;
    /** reads the session config file and restores the application's state including the last opened files and documents by reading the
     * temporary files saved by saveProperties()
     * @see KXmlGuiWindow#readProperties
     */
    void readProperties(const KConfigGroup &_cfg) override;

  signals:
    void settingsChanged();

  public slots:
    /** open a new application window by creating a new instance of KWordQuizApp */
    void slotFileNew();
    /** open a file and load it into the document*/
    void slotFileOpen();
    /** opens a file from the recent files menu */
    void slotFileOpenRecent(const QUrl &url);
    /** download vocabularies from the Internet */
    void slotFileGHNS();
    /** save a document */
    void slotFileSave();
    /** save a document by a new filename*/
    void slotFileSaveAs();
    /** asks for saving if the file is modified, then closes the actual file and window*/
    void slotFileClose();
    /** print the actual file */
    void slotFilePrint();
    /** preview the appearance of the vocabulary as printed */
    void slotFilePrintPreview();
    /** closes all open windows by calling close() on each memberList item until the list is empty, then quits the application.
     * If queryClose() returns false because the user canceled the saveModified() dialog, the closing breaks.*/
    void slotFileQuit();

    /** put the marked text/object into the clipboard and remove it from the document */
    void slotEditCut();
    /** put the marked text/object into the clipboard */
    void slotEditCopy();
    /** paste the clipboard into the document */
    void slotEditPaste();
    /** clear the grid selection */
    void slotEditClear();
    /** insert a new row */
    void slotEditInsert();
    /** delete selected row(s) */
    void slotEditDelete();
    /** mark word as blank */
    void slotEditMarkBlank();
    /** unmark word as blank */
    void slotEditUnmarkBlank();

    /** define vocabulary settings */
    void slotVocabLanguages();
    /** define vocabulary fonts */
    void slotVocabFont();
    /** link an image to the current entry */
    void slotVocabImage();
    /** link a sound to the current entry */
    void slotVocabSound();
    /** automatically adjust row heights */
    void slotVocabAdjustRows();
    /** shuffle the vocabulary */
    void slotVocabShuffle();

    void slotModeChange();
    void slotModeActionGroupTriggered(QAction *);

    /** editor session */
    void slotQuizEditor();
    /** flashcard session */
    void slotQuizFlash();
    /** multiple choice session */
    void slotQuizMultiple();
    /** question and answer session */
    void slotQuizQA();
    /** create a new vocabulary document from quiz errors */
    void slotCreateErrorListDocument();

    void slotConfigShowSearch();
    /** configure notifications */
    void slotConfigureNotifications();
    /** configure kwordquiz */
    void slotConfigure();


    /** changes the statusbar contents for the standard label permanently, used to indicate current actions.
     * @param text the text that is displayed in the statusbar */
    void slotStatusMsg(const QString &text);

    /** applies changes from the preferences dialog */
    void slotApplyPreferences();

    void slotInsertChar(int i);

    void slotCurrentPageChanged(KPageWidgetItem *current, KPageWidgetItem *before);

    void slotCleanChanged(bool);
    void slotUndoTextChanged(const QString &);
    void slotRedoTextChanged(const QString &);

    void slotTableContextMenuRequested(const QPoint &);
    void slotLayoutActionGroupTriggered(QAction *);

  private:
    KWQQuizModel *m_quiz;

    /** view is the main widget which represents your working area. The View
     * class should handle all events of the view widget.  It is kept empty so
     * you can create your view according to your application's needs by
     * changing the view class.
     */
    KPageWidget *m_pageWidget;
    QWidget *m_editorView;
    KWQTableView *m_tableView;
    KWQTableModel *m_tableModel;
    KWQSortFilterModel *m_sortFilterModel;
    FlashView *m_flashView;
    MultipleView *m_multipleView;
    QAView *m_qaView;

    KPageWidgetItem *m_editorPage;
    KPageWidgetItem *m_flashPage;
    KPageWidgetItem *m_multiplePage;
    KPageWidgetItem *m_qaPage;

    KWordQuizPrefs *m_prefDialog;

    FilterProxySearchLine *m_searchLine;

    /** m_doc represents your actual document and is created only once. It keeps
     * information such as filename and does the serialization of your files.
     */
    KEduVocDocument * m_doc;

    // QAction pointers to enable/disable actions
    QAction* fileNew;
    QAction* fileOpen;
    KRecentFilesAction* fileOpenRecent;
    QAction * fileGHNS;
    QAction* fileSave;
    QAction* fileSaveAs;
    QAction* fileClose;
    QAction* filePrint;
    QAction* filePrintPreview;
    QAction* fileQuit;

    QAction* editUndo;
    QAction* editRedo;
    QAction* editCut;
    QAction* editCopy;
    QAction* editPaste;
    QAction* editClear;
    QAction * editInsert;
    QAction * editDelete;
    QAction * editMarkBlank;
    QAction * editUnmarkBlank;

    QAction * vocabLanguages;
    QAction* vocabFont;
    QAction* vocabAdjustRows;
    QAction* vocabShuffle;
    QAction* vocabLayouts;
    QActionGroup *m_layoutActionGroup;

    KActionMenu *m_modeActionMenu;
    QActionGroup *m_modeActionGroup;

    QAction * quizEditor;
    QAction * quizFlash;
    QAction * quizMultiple;
    QAction * quizQA;

    QAction * quizCheck;
    QAction * quizRestart;
    QAction * quizRepeatErrors;
    QAction * quizExportErrors;

    QAction * flashKnow;
    QAction * flashDontKnow;

    QAction * qaHint;
    QAction * qaMarkLastCorrect;

    QAction * quizOpt1;
    QAction * quizOpt2;
    QAction * quizOpt3;

    QAction* configShowSearchBar;

    KDirWatch * m_dirWatch;

    QUndoStack * m_undoStack;

    QLabel * m_statusLabel;

    void updateActions();
    void updateSpecialCharIcons();
    QIcon charIcon(const QChar &);
    void openUrl(const QUrl &url);
};

#endif // KWORDQUIZ_H
