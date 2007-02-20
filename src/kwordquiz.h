/***************************************************************************
                          kwordquiz.h  -  description
                             -------------------
    begin                : Wed Jul 24 20:12:30 PDT 2002
    copyright            : (C) 2002-2006 by Peter Hedlund
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

// include files for Qt
#include <QSignalMapper>
#include <QLabel>
#include <QVBoxLayout>

// include files for KDE
#include <kapplication.h>
#include <kmainwindow.h>
#include <kaction.h>
#include <kdirwatch.h>
#include <kurl.h>

// forward declaration of the KWordQuiz classes
#include "wqquiz.h"

class KActionMenu;
class KRecentFilesAction;
class KToggleAction;
class KEduVocDocument;
class KWQTableModel;
class KWQTableView;
class KWordQuizPrefs;
class QAView;
class MultipleView;
class FlashView;
class KWQNewStuff;

/**
  * The base class for KWordQuiz application windows. It sets up the main
  * window and reads the config file as well as providing a menubar, toolbar
  * and statusbar.
  * KWordQuizApp reimplements the methods that KMainWindow provides for main window handling and supports
  * full session management as well as using KActions.
  * @see KMainWindow
  * @see KApplication
  * @see KConfig
  */
class KWordQuizApp : public KMainWindow
{
  Q_OBJECT

  public:
    /**
     * construtor of KWordQuizApp, calls all init functions to create the application.
     */
    KWordQuizApp(QWidget* parent=0);

    /**
     * destructor
     */
    ~KWordQuizApp();

    /**
     * opens a file specified by commandline option
     * @param url the URL to be opened
     */
    void openDocumentFile(const KUrl& url=KUrl());

    /**
     * returns a pointer to the current document connected to the KMainWindow instance
     * @return
     */
    KEduVocDocument * document() const;

    /**
     * @return whether the saving was successful
     */
    bool saveAsFileName();

    bool checkSyntax(bool blanks);

    static KWordQuizApp * self() { return m_self; }

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
    /** sets up the statusbar for the main window by initialzing a statuslabel.
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
    /** creates the centerwidget of the KMainWindow instance and sets it as the view
     */
    void initView();
    /** queryClose is called by KMainWindow on each closeEvent of a window. Against the
     * default implementation (only returns true), this calles saveModified() on the document object to ask if the document shall
     * be saved if Modified; on cancel the closeEvent is rejected.
     * @see KMainWindow#queryClose
     * @see KMainWindow#closeEvent
     */
    virtual bool queryClose();
    /** queryExit is called by KMainWindow when the last window of the application is going to be closed during the closeEvent().
     * Against the default implementation that just returns true, this calls saveOptions() to save the settings of the last window's
     * properties.
     * @see KMainWindow#queryExit
     * @see KMainWindow#closeEvent
     */
    virtual bool queryExit();
    /** saves the window properties for each open window during session end to the session config file, including saving the currently
     * opened file by a temporary filename provided by KApplication.
     * @see KMainWindow#saveProperties
     */
    virtual void saveProperties(KConfigGroup &_cfg);
    /** reads the session config file and restores the application's state including the last opened files and documents by reading the
     * temporary files saved by saveProperties()
     * @see KMainWindow#readProperties
     */
    virtual void readProperties(const KConfigGroup &_cfg);

  signals:
    void settingsChanged();

  public slots:
    /** open a new application window by creating a new instance of KWordQuizApp */
    void slotFileNew();
    /** open a file and load it into the document*/
    void slotFileOpen();
    /** opens a file from the recent files menu */
    void slotFileOpenRecent(const KUrl& url);
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
    /** closes all open windows by calling close() on each memberList item until the list is empty, then quits the application.
     * If queryClose() returns false because the user canceled the saveModified() dialog, the closing breaks.*/
    void slotFileQuit();

    /** undo the last command */
    void slotEditUndo();
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
    /** find and replace text */
    void slotEditFind();

    /** define vocabulary languages */
    void slotVocabLanguages();
    /** define vocabulary fonts */
    void slotVocabFont();
    /** define keyboard layouts */
    void slotVocabKeyboard();
    /** define row and column properties */
    void slotVocabRC();
    /** sort vocabulary */
    void slotVocabSort();
    /** shuffle the vocabulary */
    void slotVocabShuffle();
    /** enable the Leitner system */
    void slotLeitnerSystem();
    /** configure the Leitner system */
    void slotConfigLeitner();

    void slotMode0();
    /** mode 1 */
    void slotMode1();
    /** mode 2 */
    void slotMode2();
    /** mode 3 */
    void slotMode3();
    /** mode 4 */
    void slotMode4();
    /** mode 5 */
    void slotMode5();

    /** editor session */
    void slotQuizEditor();
    /** flashcard session */
    void slotQuizFlash();
    /** multiple choice session */
    void slotQuizMultiple();
    /** question and answer session */
    void slotQuizQA();

    /** configure notifications */
    void slotConfigureNotifications();
    /** configure kwordquiz */
    void slotConfigure();


    /** changes the statusbar contents for the standard label permanently, used to indicate current actions.
     * @param text the text that is displayed in the statusbar */
    void slotStatusMsg(const QString &text);

    /** applies changes from the preferences dialog */
    void slotApplyPreferences();

    void slotUndoChange(const QString & text, bool enabled);

    void slotInsertChar(int i);

    void contextMenuEvent(QContextMenuEvent * event);
  private:

    WQQuiz::QuizType m_quizType;

    WQQuiz *m_quiz;

    /** view is the main widget which represents your working area. The View
     * class should handle all events of the view widget.  It is kept empty so
     * you can create your view according to your application's needs by
     * changing the view class.
     */
    KWQTableView *m_tableView;
    KWQTableModel *m_tableModel;
    FlashView *m_flashView;
    MultipleView *m_multipleView;
    QAView *m_qaView;

    KWordQuizPrefs *m_prefDialog;

    /** m_doc represents your actual document and is created only once. It keeps
     * information such as filename and does the serialization of your files.
     */
    KEduVocDocument * m_doc;

    // KAction pointers to enable/disable actions
    QAction* fileNew;
    QAction* fileOpen;
    KRecentFilesAction* fileOpenRecent;
    QAction* fileGHNS;
    QAction* fileSave;
    QAction* fileSaveAs;
    QAction* fileClose;
    QAction* filePrint;
    QAction* fileQuit;

    QAction* editUndo;
    QAction* editCut;
    QAction* editCopy;
    QAction* editPaste;
    QAction* editClear;
    QAction* editInsert;
    QAction* editDelete;
    QAction* editMarkBlank;
    QAction* editUnmarkBlank;
    //QAction* editFind;

    QAction* vocabLanguages;
    QAction* vocabFont;
    //QAction* vocabKeyboard;
    QAction* vocabRC;
    QAction* vocabSort;
    QAction* vocabShuffle;
    QAction* vocabLeitner;
    QAction* vocabConfigLeitner;

    KActionMenu* mode;
    KToggleAction* mode1;
    KToggleAction* mode2;
    KToggleAction* mode3;
    KToggleAction* mode4;
    KToggleAction* mode5;

    QAction* quizEditor;
    QAction* quizFlash;
    QAction* quizMultiple;
    QAction* quizQA;

    QAction* quizCheck;
    QAction* quizRestart;
    QAction* quizRepeatErrors;

    QAction* flashKnow;
    QAction* flashDontKnow;

    QAction* qaHint;

    QSignalMapper* charMapper;
    QAction* specialChar1;
    QAction* specialChar2;
    QAction* specialChar3;
    QAction* specialChar4;
    QAction* specialChar5;
    QAction* specialChar6;
    QAction* specialChar7;
    QAction* specialChar8;
    QAction* specialChar9;

    KDirWatch * m_dirWatch;
    KWQNewStuff * m_newStuff;

    QLabel * m_modeLabel;
    QVBoxLayout * m_topLayout;

    static KWordQuizApp * m_self;

    void updateMode(int m);
    void updateSession(WQQuiz::QuizType qt);
    void updateActions(WQQuiz::QuizType qt);
    void updateSpecialCharIcons();
    QString charIcon(const QChar &);
    void openUrl(const KUrl& url);
};

#endif // KWORDQUIZ_H
