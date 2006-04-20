/***************************************************************************
                          kwordquiz.cpp  -  description
                             -------------------
    begin         : Wed Jul 24 20:12:30 PDT 2002
    copyright     : (C) 2002-2006 Peter Hedlund <peter.hedlund@kdemail.net>

 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QPainter>
#include <QBitmap>
#include <QCheckBox>

#include <kmessagebox.h>
#include <kfiledialog.h>
#include <kstatusbar.h>
#include <klocale.h>
#include <kedittoolbar.h>
#include <kstandarddirs.h>
#include <kfontdialog.h>
#include <kmenu.h>
#include <knotifydialog.h>
#include <kiconloader.h>
#include <kdebug.h>
#include <kxmlguifactory.h>
#include <kglobal.h>
#include <ktoolbar.h>

#include "kwordquiz.h"
#include "keduvocdocument.h"
#include "kwqtablemodel.h"
#include "kwqtableview.h"
#include "dlglanguage.h"
#include "kwordquizprefs.h"
#include "qaview.h"
#include "flashview.h"
#include "multipleview.h"
#include "wqprintdialogpage.h"
#include "prefs.h"
#include "kwqnewstuff.h"
#include "version.h"
#include "prefleitner.h"

KWordQuizApp::KWordQuizApp(QWidget* , const char* name):KMainWindow(0, name)
{

  ///////////////////////////////////////////////////////////////////
  // call inits to invoke all other construction parts
  initStatusBar();
  initActions();
  initDocument();

  readOptions();

  initView();

  m_dirWatch = KDirWatch::self();
  m_quizType = WQQuiz::qtEditor;
  m_quiz = 0;
  m_flashView = 0;
  m_multipleView = 0;
  m_qaView = 0;

  slotQuizEditor();
  slotUndoChange(i18n("Cannot &Undo"), false);
  updateMode(Prefs::mode());

  m_prefDialog = 0;

  editMarkBlank->setEnabled(Prefs::enableBlanks());
  editUnmarkBlank->setEnabled(Prefs::enableBlanks());

  if (Prefs::firstRun())
  {
    fileOpenRecent->addUrl(KUrl::fromPath(locate("data", "kwordquiz/examples/example.kvtml")));
    fileOpenRecent->addUrl(KUrl::fromPath(locate("data", "kwordquiz/examples/french_verbs.kvtml")));
    fileOpenRecent->addUrl(KUrl::fromPath(locate("data", "kwordquiz/examples/fill_in_the_blank.kvtml")));
    fileOpenRecent->addUrl(KUrl::fromPath(locate("data", "kwordquiz/examples/us_states_and_capitals.kvtml")));
    Prefs::setFirstRun(false);
  }
}

KWordQuizApp::~KWordQuizApp()
{

}

void KWordQuizApp::initActions()
{
  KAction* configToolbar;
  KAction* configNotifications;
  KAction* configApp;

  fileNew = KStdAction::openNew(this, SLOT(slotFileNew()), actionCollection());
  fileNew->setWhatsThis(i18n("Creates a new blank vocabulary document"));
  fileNew->setToolTip(fileNew->whatsThis());

  fileOpen = KStdAction::open(this, SLOT(slotFileOpen()), actionCollection());
  fileOpen->setWhatsThis(i18n("Opens an existing vocabulary document"));
  fileOpen->setToolTip(fileOpen->whatsThis());

  fileOpenRecent = KStdAction::openRecent(this, SLOT(slotFileOpenRecent(const KUrl&)), actionCollection());

  fileGHNS = new KAction(KIcon("knewstuff"), i18n("&Get New Vocabularies..."), actionCollection(), "file_ghns");
  fileGHNS->setShortcut(Qt::CTRL + Qt::Key_G);
  fileGHNS->setWhatsThis(i18n("Downloads new vocabularies"));
  fileGHNS->setToolTip(fileGHNS->whatsThis());
  connect(fileGHNS, SIGNAL(triggered(bool)), this, SLOT(slotFileGHNS()));

  fileSave = KStdAction::save(this, SLOT(slotFileSave()), actionCollection());
  fileSave->setWhatsThis(i18n("Saves the active vocabulary document"));
  fileSave->setToolTip(fileSave->whatsThis());

  fileSaveAs = KStdAction::saveAs(this, SLOT(slotFileSaveAs()), actionCollection());
  fileSaveAs->setWhatsThis(i18n("Saves the active vocabulary document with a different name"));
  fileSaveAs->setToolTip(fileSaveAs->whatsThis());

  fileClose = KStdAction::close(this, SLOT(slotFileClose()), actionCollection());
  fileClose->setWhatsThis(i18n("Closes the active vocabulary document"));
  fileClose->setToolTip(fileClose->whatsThis());

  filePrint = KStdAction::print(this, SLOT(slotFilePrint()), actionCollection());
  filePrint->setWhatsThis(i18n("Prints the active vocabulary document"));
  filePrint->setToolTip(filePrint->whatsThis());

  fileQuit = KStdAction::quit(this, SLOT(slotFileQuit()), actionCollection());
  fileQuit->setWhatsThis(i18n("Quits KWordQuiz"));
  fileQuit->setToolTip(fileQuit->whatsThis());

  editUndo = KStdAction::undo(this, SLOT(slotEditUndo()), actionCollection());
  editUndo->setWhatsThis(i18n("Undoes the last command"));
  editUndo->setToolTip(editUndo->whatsThis());

  editCut = KStdAction::cut(this, SLOT(slotEditCut()), actionCollection());
  editCut->setWhatsThis(i18n("Cuts the text from the selected cells and places it on the clipboard"));
  editCut->setToolTip(editCut->whatsThis());

  editCopy = KStdAction::copy(this, SLOT(slotEditCopy()), actionCollection());
  editCopy->setWhatsThis(i18n("Copies the text from the selected cells and places it on the clipboard"));
  editCopy->setToolTip(editCopy->whatsThis());

  editPaste = KStdAction::paste(this, SLOT(slotEditPaste()), actionCollection());
  editPaste->setWhatsThis(i18n("Pastes previously cut or copied text from the clipboard into the selected cells"));
  editPaste->setToolTip(editPaste->whatsThis());

  editClear = KStdAction::clear(this, SLOT(slotEditClear()), actionCollection());
  editClear->setWhatsThis(i18n("Clears the content of the selected cells"));
  editClear->setToolTip(editClear->whatsThis());

  editInsert = new KAction(KIcon("insert_table_row"), i18n("&Insert Row"), actionCollection(), "edit_insert");
  editInsert->setShortcut(Qt::CTRL + Qt::Key_I);
  editInsert->setWhatsThis(i18n("Inserts a new row above the current row"));
  editInsert->setToolTip(editInsert->whatsThis());
  connect(editInsert, SIGNAL(triggered(bool)), this, SLOT(slotEditInsert()));

  editDelete = new KAction(KIcon("delete_table_row"), i18n("&Delete Row"), actionCollection(), "edit_delete");
  editDelete->setShortcut(Qt::CTRL + Qt::Key_K);
  editDelete->setWhatsThis(i18n("Deletes the selected row(s)"));
  editDelete->setToolTip(editDelete->whatsThis());
  connect(editDelete, SIGNAL(triggered(bool)), this, SLOT(slotEditDelete()));

  editMarkBlank = new KAction(KIcon("markasblank"), i18n("&Mark as Blank"), actionCollection(), "edit_mark_blank");
  editMarkBlank->setShortcut(Qt::CTRL + Qt::Key_M);
  editMarkBlank->setWhatsThis(i18n("Marks the current or selected word as a blank for Fill-in-the-blank"));
  editMarkBlank->setToolTip(editMarkBlank->whatsThis());
  connect(editMarkBlank, SIGNAL(triggered(bool)), this, SLOT(slotEditMarkBlank()));

  editUnmarkBlank = new KAction(KIcon("unmarkasblank"), i18n("&Unmark Blanks"), actionCollection(), "edit_unmark_blank");
  editUnmarkBlank->setWhatsThis(i18n("Removes blanks from the current or selected word"));
  editUnmarkBlank->setToolTip(editUnmarkBlank->whatsThis());
  connect(editUnmarkBlank, SIGNAL(triggered(bool)), this, SLOT(slotEditUnmarkBlank()));

  //@todo implement editFind = KStdAction::find(this, SLOT(slotEditFind()), actionCollection());

  vocabLanguages = new KAction(KIcon("languages"), i18n("&Column Titles..."), actionCollection(), "vocab_languages");
  vocabLanguages->setShortcut(Qt::CTRL + Qt::Key_L);
  vocabLanguages->setWhatsThis(i18n("Defines the column titles for the active vocabulary"));
  vocabLanguages->setToolTip(vocabLanguages->whatsThis());
  connect(vocabLanguages, SIGNAL(triggered(bool)), this, SLOT(slotVocabLanguages()));

  vocabFont = new KAction(KIcon("fonts"), i18n("&Font..."), actionCollection(),"vocab_font");
  vocabFont->setWhatsThis(i18n("Defines the font used by the editor"));
  vocabFont->setToolTip(vocabFont->whatsThis());
  connect(vocabFont, SIGNAL(triggered(bool)), this, SLOT(slotVocabFont()));

  //@todo implement vocabKeyboard = new KAction(i18n("&Keyboard..."), "kxkb", 0, this, SLOT(slotVocabKeyboard()), actionCollection(),"vocab_keyboard");

  vocabRC = new KAction(KIcon("rowcol"), i18n("&Rows/Columns..."), actionCollection(), "vocab_rc");
  vocabRC->setWhatsThis(i18n("Defines the number of rows, row heights, and column widths for the active vocabulary"));
  vocabRC->setToolTip(vocabRC->whatsThis());
  connect(vocabRC, SIGNAL(triggered(bool)), this, SLOT(slotVocabRC()));

  vocabSort = new KAction(KIcon("sort_incr"), i18n("&Sort..."), actionCollection(), "vocab_sort");
  vocabSort->setWhatsThis(i18n("Sorts the vocabulary in ascending or descending order based on the left or right column"));
  vocabSort->setToolTip(vocabSort->whatsThis());
  connect(vocabSort, SIGNAL(triggered(bool)), this, SLOT(slotVocabSort()));

  vocabShuffle = new KAction(KIcon("shuffle"), i18n("Sh&uffle"), actionCollection(), "vocab_shuffle");
  vocabShuffle->setWhatsThis(i18n("Shuffles the entries of the active vocabulary"));
  vocabShuffle->setToolTip(vocabShuffle->whatsThis());
  connect(vocabShuffle, SIGNAL(triggered(bool)), this, SLOT(slotVocabShuffle()));

  vocabLeitner = new KAction(KIcon("leitner"), i18n("Enable Leitner system"), actionCollection(), "vocab_leitner");
  vocabLeitner->setWhatsThis(i18n("Enables the Leitner system for the active vocabulary"));
  vocabLeitner->setToolTip(vocabLeitner->whatsThis());
  connect(vocabLeitner, SIGNAL(triggered(bool)), this, SLOT(slotLeitnerSystem()));

  vocabConfigLeitner = new KAction(KIcon("config_leitner"), i18n("Configure Leitner system"),actionCollection(), "vocab_leitner_config");
  vocabConfigLeitner->setWhatsThis(i18n("Configure the Leitner system used for the active vocabulary"));
  vocabConfigLeitner->setToolTip(vocabConfigLeitner->whatsThis());
  vocabConfigLeitner->setEnabled(false);
  connect(vocabConfigLeitner, SIGNAL(triggered(bool)), this, SLOT(slotConfigLeitner()));

  mode = new KActionMenu(KIcon("mode1"), i18n("Change Mode"), actionCollection(), "mode_0");
  mode->setWhatsThis(i18n("Changes the mode used in quiz sessions"));
  mode->setToolTip(mode->whatsThis());
  connect(mode, SIGNAL(triggered(bool)), this, SLOT(slotMode0()));

  ///@todo use a QActionGroup here?
  mode1 = new KToggleAction(KIcon("mode1"), "", actionCollection(), "mode_1");
  mode1->setWhatsThis(i18n("Selects this mode"));
  mode1->setToolTip(mode1->whatsThis());
  connect(mode1, SIGNAL(triggered(bool)), this, SLOT(slotMode1()));
  mode->addAction(mode1);

  mode2 = new KToggleAction(KIcon("mode2"), "", actionCollection(), "mode_2");
  mode2->setWhatsThis(i18n("Selects this mode"));
  mode2->setToolTip(mode2->whatsThis());
  connect(mode1, SIGNAL(triggered(bool)), this, SLOT(slotMode2()));
  mode->addAction(mode2);

  mode3 = new KToggleAction(KIcon("mode3"), "", actionCollection(), "mode_3");
  mode3->setWhatsThis(i18n("Selects this mode"));
  mode3->setToolTip(mode3->whatsThis());
  connect(mode3, SIGNAL(triggered(bool)), this, SLOT(slotMode3()));
  mode->addAction(mode3);

  mode4 = new KToggleAction(KIcon("mode4"), "", actionCollection(), "mode_4");
  mode4->setWhatsThis(i18n("Selects this mode"));
  mode4->setToolTip(mode4->whatsThis());
  connect(mode4, SIGNAL(triggered(bool)), this, SLOT(slotMode4()));
  mode->addAction(mode4);

  mode5 = new KToggleAction(KIcon("mode5"), "", actionCollection(), "mode_5");
  mode5->setWhatsThis(i18n("Selects this mode"));
  mode5->setToolTip(mode5->whatsThis());
  connect(mode5, SIGNAL(triggered(bool)), this, SLOT(slotMode5()));
  mode->addAction(mode5);

  quizEditor = new KAction(KIcon("editor"), i18n("&Editor"), actionCollection(), "quiz_editor");
  quizEditor->setShortcut(Qt::Key_F6);
  quizEditor->setWhatsThis(i18n("Activates the vocabulary editor"));
  quizEditor->setToolTip(quizEditor->whatsThis());
  connect(quizEditor, SIGNAL(triggered(bool)), this, SLOT(slotQuizEditor()));

  quizFlash = new KAction(KIcon("flash"), i18n("&Flashcard"), actionCollection(), "quiz_flash");
  quizFlash->setShortcut(Qt::Key_F7);
  quizFlash->setWhatsThis(i18n("Starts a flashcard session using the active vocabulary"));
  quizFlash->setToolTip(quizFlash->whatsThis());
  connect(quizFlash, SIGNAL(triggered(bool)), this, SLOT(slotQuizFlash()));

  quizMultiple = new KAction(KIcon("multiple"), i18n("&Multiple Choice"), actionCollection(), "quiz_multiple");
  quizMultiple->setShortcut(Qt::Key_F8);
  quizMultiple->setWhatsThis(i18n("Starts a multiple choice session using the active vocabulary"));
  quizMultiple->setToolTip(quizMultiple->whatsThis());
  connect(quizMultiple, SIGNAL(triggered(bool)), this, SLOT(slotQuizMultiple()));

  quizQA = new KAction(KIcon("qa"), i18n("&Question && Answer"), actionCollection(), "quiz_qa");
  quizQA->setShortcut(Qt::Key_F9);
  quizQA->setWhatsThis(i18n("Starts a question and answer session using the active vocabulary"));
  quizQA->setToolTip(quizQA->whatsThis());
  connect(quizQA, SIGNAL(triggered(bool)), this, SLOT(slotQuizQA()));

  quizCheck = new KAction(KIcon("check"), i18n("&Check"), actionCollection(), "quiz_check");
  quizCheck->setShortcut(Qt::Key_Return);
  quizCheck->setWhatsThis(i18n("Checks your answer to this question"));
  quizCheck->setToolTip(quizCheck->whatsThis());
  connect(quizCheck, SIGNAL(triggered(bool)), this, SLOT(()));

  flashKnow = new KAction(KIcon("know"), i18n("I &Know"), actionCollection(), "flash_know");
  flashKnow->setShortcut(Qt::Key_K);
  flashKnow->setWhatsThis(i18n("Counts this card as correct and shows the next card"));
  flashKnow->setToolTip(flashKnow->whatsThis());
  connect(flashKnow, SIGNAL(triggered(bool)), this, SLOT(()));

  flashDontKnow = new KAction(KIcon("dontknow"), i18n("I &Do Not Know"), actionCollection(), "flash_dont_know");
  flashDontKnow->setShortcut(Qt::Key_D);
  flashDontKnow->setWhatsThis(i18n("Counts this card as incorrect and shows the next card"));
  flashDontKnow->setToolTip(flashDontKnow->whatsThis());
  connect(flashDontKnow, SIGNAL(triggered(bool)), this, SLOT(()));

  qaHint = new KAction(KIcon("hint"), i18n("&Hint"), actionCollection(), "qa_hint");
  qaHint->setShortcut(Qt::CTRL + Qt::Key_H);
  qaHint->setWhatsThis(i18n("Gets the next correct letter of the answer"));
  qaHint->setToolTip(qaHint->whatsThis());
  connect(qaHint, SIGNAL(triggered(bool)), this, SLOT(()));

  quizRestart = new KAction(KIcon("restart"), i18n("&Restart"), actionCollection(), "quiz_restart");
  quizRestart->setShortcut(Qt::CTRL + Qt::Key_R);
  quizRestart->setWhatsThis(i18n("Restarts the quiz session from the beginning"));
  quizRestart->setToolTip(quizRestart->whatsThis());
  connect(quizRestart, SIGNAL(triggered(bool)), this, SLOT(()));

  quizRepeatErrors = new KAction(KIcon("repeat"), i18n("Repeat &Errors"), actionCollection(), "quiz_repeat_errors");
  quizRepeatErrors->setShortcut(Qt::CTRL + Qt::Key_E);
  quizRepeatErrors->setWhatsThis(i18n("Repeats all incorrectly answered questions"));
  quizRepeatErrors->setToolTip(quizRepeatErrors->whatsThis());
  connect(quizRepeatErrors, SIGNAL(triggered(bool)), this, SLOT(()));

  configNotifications = KStdAction::configureNotifications(this, SLOT(slotConfigureNotifications()), actionCollection());
  configNotifications->setWhatsThis(i18n("Configures sound and other notifications for certain events"));
  configNotifications->setToolTip(configNotifications->whatsThis());

  configApp = KStdAction::preferences(this, SLOT( slotConfigure()), actionCollection());
  configApp->setWhatsThis(i18n("Specifies preferences for the vocabulary editor and quiz sessions"));
  configApp->setToolTip(configApp->whatsThis());

  charMapper = new QSignalMapper(this);
  connect(charMapper, SIGNAL(mapped(int)), this, SLOT(slotInsertChar(int)));

  specialChar1 = new KAction(i18n("Special Character 1"), actionCollection(), "char_1") ;
  specialChar1->setShortcut(Qt::CTRL + Qt::Key_1);
  specialChar1->setWhatsThis(i18n("Insert this character"));
  specialChar1->setToolTip(specialChar1->whatsThis());
  connect(specialChar1, SIGNAL(triggered(bool)), charMapper, SLOT(map()));

  specialChar2 = new KAction(i18n("Special Character 2"), actionCollection(), "char_2") ;
  specialChar2->setShortcut(Qt::CTRL + Qt::Key_2);
  specialChar2->setWhatsThis(i18n("Insert this character"));
  specialChar2->setToolTip(specialChar2->whatsThis());
  connect(specialChar2, SIGNAL(triggered(bool)), charMapper, SLOT(map()));

  specialChar3 = new KAction(i18n("Special Character 3"), actionCollection(), "char_3") ;
  specialChar3->setShortcut(Qt::CTRL + Qt::Key_3);
  specialChar3->setWhatsThis(i18n("Insert this character"));
  specialChar3->setToolTip(specialChar3->whatsThis());
  connect(specialChar3, SIGNAL(triggered(bool)), charMapper, SLOT(map()));

  specialChar4 = new KAction(i18n("Special Character 4"), actionCollection(), "char_4") ;
  specialChar4->setShortcut(Qt::CTRL + Qt::Key_4);
  specialChar4->setWhatsThis(i18n("Insert this character"));
  specialChar4->setToolTip(specialChar4->whatsThis());
  connect(specialChar4, SIGNAL(triggered(bool)), charMapper, SLOT(map()));

  specialChar5 = new KAction(i18n("Special Character 5"), actionCollection(), "char_5") ;
  specialChar5->setShortcut(Qt::CTRL + Qt::Key_5);
  specialChar5->setWhatsThis(i18n("Insert this character"));
  specialChar5->setToolTip(specialChar5->whatsThis());
  connect(specialChar5, SIGNAL(triggered(bool)), charMapper, SLOT(map()));

  specialChar6 = new KAction(i18n("Special Character 6"), actionCollection(), "char_6") ;
  specialChar6->setShortcut(Qt::CTRL + Qt::Key_6);
  specialChar6->setWhatsThis(i18n("Insert this character"));
  specialChar6->setToolTip(specialChar6->whatsThis());
  connect(specialChar6, SIGNAL(triggered(bool)), charMapper, SLOT(map()));

  specialChar7 = new KAction(i18n("Special Character 7"), actionCollection(), "char_7") ;
  specialChar7->setShortcut(Qt::CTRL + Qt::Key_7);
  specialChar7->setWhatsThis(i18n("Insert this character"));
  specialChar7->setToolTip(specialChar7->whatsThis());
  connect(specialChar7, SIGNAL(triggered(bool)), charMapper, SLOT(map()));

  specialChar8 = new KAction(i18n("Special Character 8"), actionCollection(), "char_8") ;
  specialChar8->setShortcut(Qt::CTRL + Qt::Key_8);
  specialChar8->setWhatsThis(i18n("Insert this character"));
  specialChar8->setToolTip(specialChar8->whatsThis());
  connect(specialChar8, SIGNAL(triggered(bool)), charMapper, SLOT(map()));

  specialChar9 = new KAction(i18n("Special Character 9"), actionCollection(), "char_9") ;
  specialChar9->setShortcut(Qt::CTRL + Qt::Key_9);
  specialChar9->setWhatsThis(i18n("Insert this character"));
  specialChar9->setToolTip(specialChar9->whatsThis());
  connect(specialChar9, SIGNAL(triggered(bool)), charMapper, SLOT(map()));

  charMapper->setMapping(specialChar1, 1);
  charMapper->setMapping(specialChar2, 2);
  charMapper->setMapping(specialChar3, 3);
  charMapper->setMapping(specialChar4, 4);
  charMapper->setMapping(specialChar5, 5);
  charMapper->setMapping(specialChar6, 6);
  charMapper->setMapping(specialChar7, 7);
  charMapper->setMapping(specialChar8, 8);
  charMapper->setMapping(specialChar9, 9);

  ///@todo port actionCollection()->setHighlightingEnabled(true);

  connect(actionCollection(), SIGNAL(actionStatusText(const QString &)), statusBar(), SLOT(showMessage(const QString &)));
  connect(actionCollection(), SIGNAL(clearStatusText()), statusBar(), SLOT(clearMessage()));

  updateSpecialCharIcons();

  if (!initialGeometrySet())
    resize( QSize(650, 500).expandedTo(minimumSizeHint()));
  setupGUI(ToolBar | Keys | StatusBar | Create);
  setAutoSaveSettings();

  configToolbar = actionCollection()->action("options_configure_toolbars");
  configToolbar->setWhatsThis(i18n("Toggles display of the toolbars"));
  configToolbar->setToolTip(configToolbar->whatsThis());
}

void KWordQuizApp::initStatusBar()
{
  m_modeLabel = new QLabel();
  m_modeLabel->setFixedWidth(250);
  m_modeLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
  statusBar()->addPermanentWidget(m_modeLabel, 0);
}

void KWordQuizApp::initDocument()
{
  doc = new KEduVocDocument(this);
  doc->appendIdentifier(i18n("Column 1"));
  doc->appendIdentifier(i18n("Column 2"));
  for (int i=0; i<20; i++)
  {
    doc->appendEntry(new KEduVocExpression());
  }
  m_tableModel = new KWQTableModel(doc, this);
  m_tableModel->setHeaderData(0, Qt::Horizontal, QSize(250, 25), Qt::SizeHintRole);
  m_tableModel->setHeaderData(1, Qt::Horizontal, QSize(250, 25), Qt::SizeHintRole);
  doc->setModified(false);
}

void KWordQuizApp::initView()
{
  QWidget * mainWidget = new QWidget(this);
  setCentralWidget(mainWidget);
  m_topLayout = new QVBoxLayout(mainWidget, 0, KDialog::spacingHint());
  m_tableView = new KWQTableView(centralWidget());
  m_tableView->setFrameStyle(QFrame::NoFrame);
  m_topLayout->addWidget(m_tableView);


  m_tableView->setModel(m_tableModel);
  m_tableView->setColumnWidth(0, qvariant_cast<QSize>(m_tableModel->headerData(0, Qt::Horizontal, Qt::SizeHintRole)).width());
  m_tableView->setColumnWidth(1, qvariant_cast<QSize>(m_tableModel->headerData(1, Qt::Horizontal, Qt::SizeHintRole)).width());
  setCaption(doc->URL().fileName(),false);
  connect(m_tableView, SIGNAL(undoChange(const QString&, bool )), this, SLOT(slotUndoChange(const QString&, bool)));
  connect(m_tableView, SIGNAL(contextMenuRequested(int, int, const QPoint &)), this, SLOT(slotContextMenuRequested(int, int, const QPoint& )));
  doc->setModified(false);
}

void KWordQuizApp::openURL(const KUrl& url)
{
  if(!url.isEmpty()) {
    if (m_dirWatch->contains(url.path()))
    {
      KMainWindow* w;
      if(!memberList().isEmpty())
      {
        for (int i = 0; i < memberList().size(); ++i)
        {
          w = memberList().at(i);
          KWordQuizApp *a =(KWordQuizApp *) w;
          if(a->doc ->URL().path() == url.path())
          {
            if (w->isMinimized())
              w->showNormal();
            w->setActiveWindow();
            w->raise();
            break;
          }
        }
      }
    }
    else
    {
      ///@todo this doesn't really check if the entries are empty. Is it worth it to have such as function?
      if (doc -> URL().fileName() == i18n("Untitled")  && doc -> numEntries() == 0){
        // neither saved nor has content, as good as new
        openDocumentFile(url);
      }
      else
      {
        KWordQuizApp *new_window= new KWordQuizApp();
        new_window->show();
        new_window->openDocumentFile(url);
      }
    }
  }
}

void KWordQuizApp::openDocumentFile(const KUrl& url)
{
  slotStatusMsg(i18n("Opening file..."));
  if (!url.isEmpty()) {
    doc->open(url, false);
    m_tableView->setColumnWidth(0, qvariant_cast<QSize>(m_tableModel->headerData(0, Qt::Horizontal, Qt::SizeHintRole)).width());
    m_tableView->setColumnWidth(1, qvariant_cast<QSize>(m_tableModel->headerData(1, Qt::Horizontal, Qt::SizeHintRole)).width());
    if (doc->font() == NULL)
      doc->setFont(new QFont(Prefs::editorFont()));
    m_dirWatch->addFile(url.path());
    setCaption(doc->URL().fileName(), false);
    fileOpenRecent->addUrl( url );
    updateMode(Prefs::mode());
  }
  slotStatusMsg(i18n("Ready"));
}


KEduVocDocument *KWordQuizApp::getDocument() const
{
  return doc;
}

void KWordQuizApp::saveOptions()
{
  fileOpenRecent->saveEntries(KGlobal::config(), "Recent Files");
  Prefs::writeConfig();
}


void KWordQuizApp::readOptions()
{
  //Prefs::readConfig();
  fileOpenRecent->loadEntries(KGlobal::config(), "Recent Files");
}

void KWordQuizApp::saveProperties(KConfig *_cfg)
{
  if(doc->URL().fileName()!=i18n("Untitled") && !doc->isModified())
  {
    // saving to tempfile not necessary

  }
  else
  {
    KUrl url=doc->URL();
    _cfg->writeEntry("filename", url.url());
    _cfg->writeEntry("modified", doc->isModified());
    QString tempname = kapp->tempSaveName(url.url());
    QString tempurl= KUrl::toPercentEncoding(tempname);
    KUrl _url(tempurl);
    doc->saveAs(this, _url, KEduVocDocument::automatic, QString("kwordquiz %1").arg(KWQ_VERSION));
  }
}


void KWordQuizApp::readProperties(KConfig* _cfg)
{
  QString filename = _cfg->readEntry("filename", "");
  KUrl url(filename);
  bool modified = _cfg->readEntry("modified", false);
  if(modified)
  {
    bool canRecover;
    QString tempname = kapp->checkRecoverFile(filename, canRecover);
    KUrl _url(tempname);

    if(canRecover)
    {
      doc->open(_url, false);
      if (doc->font() == NULL)
        doc->setFont(new QFont(Prefs::editorFont()));
      doc->setModified();
      setCaption(_url.fileName(),true);
      QFile::remove(tempname);
    }
  }
  else
  {
    if(!filename.isEmpty())
    {
      doc->open(url, false);
      if (doc->font() == NULL)
        doc->setFont(new QFont(Prefs::editorFont()));
      setCaption(url.fileName(),false);
    }
  }
}

bool KWordQuizApp::queryClose()
{
  bool completed=true;

  if(doc->isModified())
  {
    int want_save = KMessageBox::warningYesNoCancel(this,
                    i18n("The current file has been modified.\n"
                         "Do you want to save it?"),
                    i18n("Warning"));
    switch(want_save)
    {
    case KMessageBox::Yes:
      if (doc->URL().fileName() == i18n("Untitled"))
      {
        completed = saveAsFileName();
      }
      else
      {
        completed = doc->saveAs(this, doc->URL(), KEduVocDocument::automatic, QString("kwordquiz %1").arg(KWQ_VERSION));
      }

      break;

    case KMessageBox::No:
      doc->setModified(false);
      completed=true;
      break;

    case KMessageBox::Cancel:
      completed=false;
      break;

    default:
      completed=false;
      break;
    }
  }

  if (completed)
    if (m_dirWatch->contains(doc->URL().path()))
      m_dirWatch->removeFile(doc->URL().path());
  return completed;
}

bool KWordQuizApp::queryExit()
{
  saveOptions();
  return true;
}

bool KWordQuizApp::checkSyntax(bool blanks)
{
  int errorCount = 0;

  for (int r = 0; r < doc->numEntries(); ++r)
  {
    QString s = doc->entry(r)->original();
    if (s.length() > 0)
      for (int i = 0; i <= s.length(); ++i)
        if (s[i] == delim_start || s[i] == delim_end)
          if (!m_tableView->checkForBlank(s, blanks))
            errorCount++;
    s = doc->entry(r)->translation(1);
    if (s.length() > 0)
      for (int i = 0; i <= s.length(); ++i)
        if (s[i] == delim_start || s[i] == delim_end)
          if (!m_tableView->checkForBlank(s, blanks))
            errorCount++;
    }
  return (errorCount == 0);
}

/////////////////////////////////////////////////////////////////////
// SLOT IMPLEMENTATION
/////////////////////////////////////////////////////////////////////

void KWordQuizApp::slotFileNew()
{
  slotStatusMsg(i18n("Opening a new document window..."));
  if (doc -> URL().fileName() == i18n("Untitled")  && doc -> numEntries() == 0){
    // neither saved nor has content, as good as new
  }
  else
  {
    KWordQuizApp *new_window= new KWordQuizApp();
    new_window->show();
  }
  slotStatusMsg(i18n("Ready"));
}

void KWordQuizApp::slotFileOpen()
{
  slotStatusMsg(i18n("Opening file..."));

  QCheckBox * cb = new QCheckBox(i18n("&Join selected files into one list"), 0);
  cb -> setChecked(false);
  ///@todo make append work again
  cb -> setEnabled(false);

  QString filter = i18n("*.kvtml *.wql *.xml.gz *.csv|All Supported Documents\n*.kvtml|KDE Vocabulary Document\n*.wql|KWordQuiz Document\n*.xml.gz|Pauker Lesson\n*.csv|Comma-Separated Values");
  KFileDialog *fd = new KFileDialog(QString(), filter, this, cb);
  fd -> setOperationMode(KFileDialog::Opening);
  fd -> setMode(KFile::Files | KFile::ExistingOnly);
  fd -> setCaption(i18n("Open Vocabulary Document"));

  if (fd->exec() == QDialog::Accepted)
  {
    KUrl::List l = fd -> selectedURLs();
    bool append = ((cb -> isChecked()) && (l.count() > 1));

    if (append)
    {
      KWordQuizApp * w;
      if (doc->URL().fileName() == i18n("Untitled")  && doc->numEntries() == 0){
        // neither saved nor has content, as good as new
        w = this;
      }
      else
      {
        w = new KWordQuizApp();
        w->show();
      }

      KUrl::List::iterator it;
      int i = 0;
      for(it = l.begin(); it != l.end(); ++it)
      {
        w->getDocument()->open(*it, true);
        i++;
      }
    }
    else
    {
      KUrl::List::iterator it;
      for(it = l.begin(); it != l.end(); ++it)
      {
        openURL(*it);
      }
    }
  }

  delete (fd); //deletes cb also

  slotStatusMsg(i18n("Ready"));
}

void KWordQuizApp::slotFileOpenRecent(const KUrl& url)
{
  slotStatusMsg(i18n("Opening file..."));
  fileOpenRecent->setCurrentItem(-1);
  openURL(url);
  slotStatusMsg(i18n("Ready"));
}

void KWordQuizApp::slotFileSave()
{
  slotStatusMsg(i18n("Saving file..."));
  if (doc->URL().fileName() == i18n("Untitled") )
  {
    slotFileSaveAs();
  }
  else
  {
    doc->saveAs(this, doc->URL(), KEduVocDocument::automatic, QString("kwordquiz %1").arg(KWQ_VERSION));
  }
  slotStatusMsg(i18n("Ready"));
}

void KWordQuizApp::slotFileGHNS()
{
  if (!m_newStuff)
    m_newStuff = new KWQNewStuff(this);
  m_newStuff->download();
}

void KWordQuizApp::slotFileSaveAs()
{
  slotStatusMsg(i18n("Saving file with a new filename..."));
  saveAsFileName();
  slotStatusMsg(i18n("Ready"));
}

bool KWordQuizApp::saveAsFileName( )
{

  bool success = false;

  QString filter = i18n("*.kvtml|KDE Vocabulary Document\n*.wql|KWordQuiz Document\n*.csv|Comma-Separated Values\n*.html|Hypertext Markup Language");
  KFileDialog *fd = new KFileDialog(QString(), filter, this);
  fd -> setOperationMode(KFileDialog::Saving);
  fd -> setCaption(i18n("Save Vocabulary Document As"));

  if (fd->exec() == QDialog::Accepted)
  {
    KUrl url = fd -> selectedURL();
    if(!url.isEmpty()){

      ///@todo check that a valid extension was really given
      if (!url.fileName().contains('.'))
      {
        ///@todo make sure all these formats are really available
        if  (fd->currentFilter() == "*.wql")
          url = KUrl(url.path() + ".wql");
        else if (fd->currentFilter() == "*.csv")
          url = KUrl(url.path() + ".csv");
        else if (fd->currentFilter() == "*.html")
          url = KUrl(url.path() + ".html");
        else
          url = KUrl(url.path() + ".kvtml");
      }

      QFileInfo fileinfo(url.path());
      if (fileinfo.exists() && KMessageBox::warningContinueCancel(0,
          i18n("<qt>The file<br><b>%1</b><br>already exists. Do you want to overwrite it?</qt>",
               url.path()),QString(),i18n("Overwrite")) == KMessageBox::Cancel)
      {
      // do nothing
      }
      else
      {
        if (m_dirWatch ->contains(doc->URL().path()))
          m_dirWatch ->removeFile(doc->URL().path());
        doc->saveAs(this, url, KEduVocDocument::automatic, QString("kwordquiz %1").arg(KWQ_VERSION));
        m_dirWatch->addFile(url.path());
        fileOpenRecent->addUrl(url);
        setCaption(doc->URL().fileName(), doc->isModified());
        success = true;
      }
    }
  }
  delete(fd);
  return success;
}

void KWordQuizApp::slotFileClose()
{
  slotStatusMsg(i18n("Closing file..."));

  if (memberList().count() > 1)
    close();
  else
    if (queryClose())
    {
      delete doc;
      initDocument();
      setCaption(doc->URL().fileName(), doc->isModified());
      delete (m_tableView);
      initView();
      slotQuizEditor();
      slotUndoChange(i18n("Cannot &Undo"), false);
      updateMode(Prefs::mode());
      m_tableView ->setFocus();
    }

  slotStatusMsg(i18n("Ready"));
}

void KWordQuizApp::slotFilePrint()
{
  slotStatusMsg(i18n("Printing..."));
  WQPrintDialogPage * p = new WQPrintDialogPage(this);
  KPrinter printer;
  printer.addDialogPage(p);
  printer.setFullPage(true);
  if (printer.setup(this))
  {
    m_tableView->print(&printer);
  }

  slotStatusMsg(i18n("Ready"));
}

void KWordQuizApp::slotFileQuit()
{
  slotStatusMsg(i18n("Exiting..."));
  saveOptions();
  // close the first window, the list makes the next one the first again.
  // This ensures that queryClose() is called on each window to ask for closing
  KMainWindow* w;
  if(!memberList().isEmpty())
  {
  for (int i = 0; i < memberList().size(); ++i)
    {
    w = memberList().at(i);
      // only close the window if the closeEvent is accepted. If the user presses Cancel on the saveModified() dialog,
      // the window and the application stay open.
      if(!w->close())
        break;
    }
  }
}

void KWordQuizApp::slotUndoChange( const QString & text, bool enabled )
{
  editUndo->setText(text);
  editUndo->setEnabled(enabled);
}

void KWordQuizApp::slotEditUndo()
{
  slotStatusMsg(i18n("Undoing previous command..."));
  m_tableView->doEditUndo();
  slotStatusMsg(i18n("Ready"));
}

void KWordQuizApp::slotEditCut()
{
  slotStatusMsg(i18n("Cutting selection..."));
  m_tableView->doEditCut();
  slotStatusMsg(i18n("Ready"));
}

void KWordQuizApp::slotEditCopy()
{
  slotStatusMsg(i18n("Copying selection to clipboard..."));
  m_tableView->doEditCopy();
  slotStatusMsg(i18n("Ready"));
}

void KWordQuizApp::slotEditPaste()
{
  slotStatusMsg(i18n("Inserting clipboard contents..."));
  m_tableView->doEditPaste();
  slotStatusMsg(i18n("Ready"));
}

void KWordQuizApp::slotEditClear()
{
  slotStatusMsg(i18n("Clearing the selected cells..."));
  m_tableView->doEditClear();
  slotStatusMsg(i18n("Ready"));
}

void KWordQuizApp::slotEditInsert()
{
  slotStatusMsg(i18n("Inserting rows..."));
  m_tableView->doEditInsert();
  slotStatusMsg(i18n("Ready"));
}

void KWordQuizApp::slotEditDelete()
{
  slotStatusMsg(i18n("Deleting selected rows..."));
  m_tableView->doEditDelete();
  slotStatusMsg(i18n("Ready"));
}

void KWordQuizApp::slotEditMarkBlank()
{
  slotStatusMsg(i18n("Marking selected text as a blank..."));
  m_tableView->doEditMarkBlank();
  slotStatusMsg(i18n("Ready"));
}

void KWordQuizApp::slotEditUnmarkBlank()
{
  slotStatusMsg(i18n("Removing blank markings..."));
  m_tableView->doEditUnmarkBlank();
  slotStatusMsg(i18n("Ready"));
}

void KWordQuizApp::slotEditFind()
{
  slotStatusMsg(i18n("Searching for indicated text..."));
  KMessageBox::sorry(0, i18n("Not implemented yet"));
  slotStatusMsg(i18n("Ready"));
}

void KWordQuizApp::slotVocabLanguages()
{
  slotStatusMsg(i18n("Setting the column titles of the vocabulary..."));
  DlgLanguage* dlg;
  dlg = new DlgLanguage(this);
  dlg->setLanguage(1, m_tableModel->headerData(0, Qt::Horizontal, Qt::DisplayRole).toString());
  dlg->setLanguage(2, m_tableModel->headerData(1, Qt::Horizontal, Qt::DisplayRole).toString());
  if (dlg->exec() == KDialog::Accepted)
  {
    m_tableModel->setHeaderData(0, Qt::Horizontal, dlg->Language(1), Qt::EditRole);
    m_tableModel->setHeaderData(1, Qt::Horizontal, dlg->Language(2), Qt::EditRole);
    updateMode(Prefs::mode());
  }
  slotStatusMsg(i18n("Ready"));
}

void KWordQuizApp::slotVocabFont()
{
  slotStatusMsg(i18n("Setting the font of the vocabulary..."));
  KFontDialog* dlg;
  dlg = new KFontDialog(this, false, true);
  dlg->setObjectName("dlg_font");
  dlg->setFont(Prefs::editorFont());
  if (dlg->exec() == KFontDialog::Accepted)
  {
    doc->setFont(new QFont(dlg->font()));
    m_tableView->reset();
    Prefs::setEditorFont(dlg->font());
    doc->setModified(true);
  }
  slotStatusMsg(i18n("Ready"));
}

void KWordQuizApp::slotVocabKeyboard()
{
  slotStatusMsg(i18n("Changing the keyboard layout..."));
  KMessageBox::sorry(0, i18n("Not implemented yet"));
  slotStatusMsg(i18n("Ready"));
}

void KWordQuizApp::slotVocabRC()
{
  slotStatusMsg(i18n("Changing row and column properties..."));
  m_tableView->doVocabRC();
  slotStatusMsg(i18n("Ready"));
}

void KWordQuizApp::slotVocabSort()
{
  slotStatusMsg(i18n("Sorting the vocabulary..."));
  m_tableView->doVocabSort();
  slotStatusMsg(i18n("Ready"));
}

void KWordQuizApp::slotVocabShuffle()
{
  slotStatusMsg(i18n("Randomizing the vocabulary..."));
  m_tableView->doVocabShuffle();
  m_tableModel->shuffle();
  slotStatusMsg(i18n("Ready"));
}

void KWordQuizApp::slotMode0()
{
  slotStatusMsg(i18n("Updating mode..."));
  if (Prefs::mode() < 5) {
    updateMode(Prefs::mode() + 1);
  }
  else
  {
  updateMode(1);
  }
  slotStatusMsg(i18n("Ready"));
}

void KWordQuizApp::slotMode1()
{
  slotStatusMsg(i18n("Updating mode..."));
  updateMode(1);
  slotStatusMsg(i18n("Ready"));
}

void KWordQuizApp::slotMode2()
{
  slotStatusMsg(i18n("Updating mode..."));
  updateMode(2);
  slotStatusMsg(i18n("Ready"));
}

void KWordQuizApp::slotMode3()
{
  slotStatusMsg(i18n("Updating mode..."));
  updateMode(3);
  slotStatusMsg(i18n("Ready"));
}

void KWordQuizApp::slotMode4()
{
  slotStatusMsg(i18n("Updating mode..."));
  updateMode(4);
  slotStatusMsg(i18n("Ready"));
}

void KWordQuizApp::slotMode5()
{
  slotStatusMsg(i18n("Updating mode..."));
  updateMode(5);
  slotStatusMsg(i18n("Ready"));
}

void KWordQuizApp::slotQuizEditor()
{
  slotStatusMsg(i18n("Starting editor session..."));
  updateSession(WQQuiz::qtEditor);
  slotStatusMsg(i18n("Ready"));
}

void KWordQuizApp::slotQuizFlash()
{
  slotStatusMsg(i18n("Starting flashcard session..."));
  updateSession(WQQuiz::qtFlash);
  slotStatusMsg(i18n("Ready"));
}

void KWordQuizApp::slotQuizMultiple()
{
  slotStatusMsg(i18n("Starting multiple choice session..."));
  updateSession(WQQuiz::qtMultiple);
  slotStatusMsg(i18n("Ready"));
}

void KWordQuizApp::slotQuizQA()
{
  slotStatusMsg(i18n("Starting question & answer session..."));
  updateSession(WQQuiz::qtQA);
  slotStatusMsg(i18n("Ready"));
}

void KWordQuizApp::updateSession(WQQuiz::QuizType qt)
{
  if (m_quiz != 0)
  {
    delete(m_quiz);
    m_quiz = 0;
  }

  switch( m_quizType ){
    case WQQuiz::qtEditor:
      //
      break;
    case WQQuiz::qtFlash:
      if (m_flashView != 0)
      {
        delete(m_flashView);
        m_flashView = 0;
      }
      break;
    case WQQuiz::qtMultiple:
      if (m_multipleView != 0)
      {
        delete(m_multipleView);
        m_multipleView = 0;
      }
      break;
    case WQQuiz::qtQA:
      if (m_qaView != 0)
      {
        delete(m_qaView);
        m_qaView = 0;
      }
      break;
  }

  m_quizType = qt;
  updateActions(qt);

  switch( m_quizType ){
    case WQQuiz::qtEditor:
      m_tableView->show();
      m_tableView -> setFocus();
      break;
    case WQQuiz::qtFlash:
      m_quiz = new WQQuiz(this, doc);
      connect(m_quiz, SIGNAL(checkingAnswer(int )), m_tableView, SLOT(slotCheckedAnswer(int )));
      m_quiz ->setQuizType(WQQuiz::qtFlash);
      m_quiz->setQuizMode(Prefs::mode());
      if (m_quiz -> init())
      {
        m_tableView->hide();
        m_flashView = new FlashView(this);
        connect(quizCheck, SIGNAL(activated()), m_flashView, SLOT(slotFlip()));
        connect(flashKnow, SIGNAL(activated()), m_flashView, SLOT(slotKnow()));
        connect(flashDontKnow, SIGNAL(activated()), m_flashView, SLOT(slotDontKnow()));
        connect(quizRestart, SIGNAL(activated()), m_flashView, SLOT(slotRestart()));
        connect(quizRepeatErrors, SIGNAL(activated()), m_flashView, SLOT(slotRepeat()));
        connect(this, SIGNAL(settingsChanged()), m_flashView, SLOT(slotApplySettings()));

        m_flashView->setQuiz(m_quiz);
        m_flashView->init();
        m_flashView->show();
        m_topLayout->addWidget(m_flashView);
      }
      else
      {
        delete(m_quiz);
        m_quiz = 0;
      }
      break;
    case WQQuiz::qtMultiple:
      m_quiz = new WQQuiz(this, doc);
      connect(m_quiz, SIGNAL(checkingAnswer(int )), m_tableView, SLOT(slotCheckedAnswer(int )));
      m_quiz ->setQuizType(WQQuiz::qtMultiple);
      m_quiz->setQuizMode(Prefs::mode());
      if (m_quiz -> init())
      {
        //m_tableView->saveCurrentSelection(true);
        m_tableView->hide();
        m_multipleView = new MultipleView(this);
        connect(quizCheck, SIGNAL(activated()), m_multipleView, SLOT(slotCheck()));
        connect(quizRestart, SIGNAL(activated()), m_multipleView, SLOT(slotRestart()));
        connect(quizRepeatErrors, SIGNAL(activated()), m_multipleView, SLOT(slotRepeat()));
        connect(this, SIGNAL(settingsChanged()), m_multipleView, SLOT(slotApplySettings()));

        m_multipleView -> setQuiz(m_quiz);
        m_multipleView ->init();
        m_multipleView->show();
        m_topLayout->addWidget(m_multipleView);
      }
      else
      {
        delete(m_quiz);
        m_quiz = 0;
      }
      break;
    case WQQuiz::qtQA:
      m_quiz = new WQQuiz(this, doc);
      connect(m_quiz, SIGNAL(checkingAnswer(int )), m_tableView, SLOT(slotCheckedAnswer(int )));
      m_quiz ->setQuizType(WQQuiz::qtQA);
      m_quiz->setQuizMode(Prefs::mode());
      if (m_quiz -> init())
      {
        //m_tableView->saveCurrentSelection(true);
        m_tableView->hide();
        m_qaView = new QAView(this);
        connect(quizCheck, SIGNAL(activated()), m_qaView, SLOT(slotCheck()));
        connect(qaHint, SIGNAL(activated()), m_qaView, SLOT(slotHint()));
        connect(quizRestart, SIGNAL(activated()), m_qaView, SLOT(slotRestart()));
        connect(quizRepeatErrors, SIGNAL(activated()), m_qaView, SLOT(slotRepeat()));
        connect(this, SIGNAL(settingsChanged()), m_qaView, SLOT(slotApplySettings()));

        m_qaView -> setQuiz(m_quiz);
        m_qaView ->init();
        m_qaView->show();
        m_topLayout->addWidget(m_qaView);
      }
      else
      {
        delete(m_quiz);
        m_quiz = 0;
      }
      break;

  }
}

/** Configure notifications */
void KWordQuizApp::slotConfigureNotifications( )
{
  KNotifyDialog::configure(this, "Notification Configuration Dialog");
}

/** Configure kwordquiz */
void KWordQuizApp::slotConfigure()
{
  if ( KWordQuizPrefs::showDialog( "settings" ) )
    return;

  //KConfigDialog didn't find an instance of this dialog, so lets create it :
  KWordQuizPrefs* dialog = new KWordQuizPrefs( this, "settings",  Prefs::self() );
  connect(dialog, SIGNAL(settingsChanged(const QString &)), this, SLOT(slotApplyPreferences()));
  dialog->show();
}

void KWordQuizApp::slotApplyPreferences()
{
  kDebug() << "Prefs Update" << endl;
  editMarkBlank->setEnabled(Prefs::enableBlanks());
  editUnmarkBlank->setEnabled(Prefs::enableBlanks());
  m_tableView->reset();
  updateSpecialCharIcons();
  emit settingsChanged();
}

void KWordQuizApp::updateSpecialCharIcons( )
{
///@todo port
/*  for (int i = 0; i < 9; i++){
    KAction * act = actionCollection()->action(QString("char_" + QString::number(i + 1)).latin1());
    act->setIcon(charIcon(Prefs::specialCharacters()[i]));
    act->setToolTip(i18n("Inserts the character %1").arg(Prefs::specialCharacters()[i]));
  }*/
}

QString KWordQuizApp::charIcon(const QChar & c)
{
  ///Create a name and path for the icon
  QString s = locateLocal("icon", "char" + QString::number(c.unicode()) + ".png");

  ///No need to redraw if it already exists
  if (KStandardDirs::exists(s))
    return s;

  QRect r(4, 4, 120, 120);

  ///A font to draw the character with
  QFont font("sans");
  font.setPixelSize(100);
  font.setWeight(QFont::Bold);

  ///Create the pixmap
  QPixmap pm(128, 128);
  pm.fill(Qt::white);
  QPainter p(&pm);
  p.setFont(font);
  p.setPen(Qt::blue);
  p.drawText(r, Qt::AlignCenter, (QString) c);

  ///Create transparency mask
  QBitmap bm(128, 128);
  bm.fill(Qt::color0);
  QPainter b(&bm);
  b.setFont(font);
  b.setPen(Qt::color1);
  b.drawText(r, Qt::AlignCenter, (QString) c);

  ///Mask the pixmap
  pm.setMask(bm);

  ///Save the icon to disk
  pm.save(s, "PNG");

  return s;
}

void KWordQuizApp::slotStatusMsg(const QString &text)
{
  //statusBar()->clearMessage();
  statusBar()->showMessage(text);
}

/*!
    \fn KWordQuizApp::updateMode(int m)
 */
void KWordQuizApp::updateMode(int m)
{
  if (m_quiz != 0)
    if (KMessageBox::warningContinueCancel(this, i18n("This will restart your quiz. Do you wish to continue?"), QString(), KStdGuiItem::cont(), "askModeQuiz") != KMessageBox::Continue)
    {
      mode1->setChecked(Prefs::mode() == 1);
      mode2->setChecked(Prefs::mode() == 2);
      mode3->setChecked(Prefs::mode() == 3);
      mode4->setChecked(Prefs::mode() == 4);
      mode5->setChecked(Prefs::mode() == 5);
      return;
    }

  Prefs::setMode(m);
  QString s1 = doc->originalIdentifier();
  QString s2 = doc->identifier(1);

  mode1->setText(i18n("&1 %1 -> %2 In Order", s1, s2));
  mode2->setText(i18n("&2 %1 -> %2 In Order", s2, s1));
  mode3->setText(i18n("&3 %1 -> %2 Randomly", s1, s2));
  mode4->setText(i18n("&4 %1 -> %2 Randomly", s2, s1));
  mode5->setText(i18n("&5 %1 <-> %2 Randomly", s1, s2));

  mode1->setChecked(Prefs::mode() == 1);
  mode2->setChecked(Prefs::mode() == 2);
  mode3->setChecked(Prefs::mode() == 3);
  mode4->setChecked(Prefs::mode() == 4);
  mode5->setChecked(Prefs::mode() == 5);

  QString s;
  mode->setIcon(KIcon("mode" + s.setNum(Prefs::mode())));

  switch (Prefs::mode()){
  case 1:
    m_modeLabel->setText(i18n("%1 -> %2 In Order", s1, s2));
    break;
  case 2:
    m_modeLabel->setText(i18n("%1 -> %2 In Order", s2, s1));
    break;
  case 3:
    m_modeLabel->setText(i18n("%1 -> %2 Randomly", s1, s2));
    break;
  case 4:
    m_modeLabel->setText(i18n("%1 -> %2 Randomly", s2, s1));
    break;
  case 5:
    m_modeLabel->setText(i18n("%1 <-> %2 Randomly", s1, s2));
    break;
  }

  if (m_quiz !=0)
    updateSession(m_quizType);
}

void KWordQuizApp::slotInsertChar( int i )
{
  if (m_qaView != 0)
    m_qaView->slotSpecChar(Prefs::specialCharacters()[i - 1]);
  else
    if (centralWidget() == m_tableView)
      m_tableView->slotSpecChar(Prefs::specialCharacters()[i - 1]);
}
/*
void KWordQuizApp::slotActionHighlighted(KAction * action, bool hl)
{
  //if (!hl)
   // slotStatusMsg(i18n("Ready"));
}
*/
void KWordQuizApp::slotContextMenuRequested(int /*row*/, int /*col*/, const QPoint & pos)
{
  QWidget * w = guiFactory()->container("editor_popup", this);
  KMenu *popup = static_cast<KMenu *>(w);

  popup->exec(pos);
}

void KWordQuizApp::updateActions( WQQuiz::QuizType qt )
{
  bool fEdit = (qt == WQQuiz::qtEditor);

  fileSave->setEnabled(fEdit);
  fileSaveAs->setEnabled(fEdit);
  filePrint->setEnabled(fEdit);
  //editFind->setEnabled(fEdit);
  editClear->setEnabled(fEdit);
  editInsert->setEnabled(fEdit);
  editDelete->setEnabled(fEdit);
  editMarkBlank->setEnabled(fEdit && Prefs::enableBlanks());
  editUnmarkBlank->setEnabled(fEdit && Prefs::enableBlanks());
  vocabLanguages->setEnabled(fEdit);
  vocabFont->setEnabled(fEdit);
  //vocabKeyboard->setEnabled(fEdit);
  vocabRC->setEnabled(fEdit);
  vocabSort->setEnabled(fEdit);
  vocabShuffle->setEnabled(fEdit);

  quizEditor->setEnabled(qt != WQQuiz::qtEditor);
  quizFlash->setEnabled(qt != WQQuiz::qtFlash);
  quizMultiple->setEnabled(qt != WQQuiz::qtMultiple);
  quizQA->setEnabled(qt != WQQuiz::qtQA);

  quizCheck->setEnabled(qt != WQQuiz::qtEditor);
  quizRestart->setEnabled(qt != WQQuiz::qtEditor);
  quizRepeatErrors->setEnabled(false);

  flashKnow->setEnabled(qt == WQQuiz::qtFlash);
  flashDontKnow->setEnabled(qt == WQQuiz::qtFlash);

  qaHint->setEnabled(qt == WQQuiz::qtQA);

  toolBar("quizToolBar")->setHidden(qt == WQQuiz::qtEditor);

}

void KWordQuizApp::slotLeitnerSystem()
{
  if( doc->leitnerSystemActive() )
  {
    doc->setLeitnerSystemActive(false);
    vocabLeitner->setText(i18n("Enable Leitner system"));
    vocabConfigLeitner->setEnabled( false );
  }
  else
  {
    doc->setLeitnerSystemActive(true);
    vocabLeitner->setText(i18n("Disable Leitner system"));
    vocabConfigLeitner->setEnabled( true );
  }
}

void KWordQuizApp::slotConfigLeitner()
{
  PrefLeitner* config = new PrefLeitner(this, doc->leitnerSystem());
  if (config->exec() == QDialog::Accepted)
    doc->setLeitnerSystem(config->system());

  delete config;
}

#include "kwordquiz.moc"
