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

#include <kactioncollection.h>
#include <kapplication.h>
#include <kactionmenu.h>
#include <kmessagebox.h>
#include <kfiledialog.h>
#include <kstatusbar.h>
#include <klocale.h>
#include <kedittoolbar.h>
#include <kstandarddirs.h>
#include <kfontdialog.h>
#include <kmenu.h>
#include <knotifyconfigwidget.h>
#include <kiconloader.h>
#include <kdebug.h>
#include <kxmlguifactory.h>
#include <kglobal.h>
#include <krecentfilesaction.h>
#include <kstandardaction.h>
#include <ktoggleaction.h>
#include <ktoolbar.h>
#include <kicon.h>
#include <knewstuff2/engine.h>
#include <knewstuff2/ui/knewstuffaction.h>

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
#include "version.h"
#include "prefleitner.h"

KWordQuizApp * KWordQuizApp::m_self = 0;

KWordQuizApp::KWordQuizApp(QWidget*):KMainWindow(0)
{
  m_self = this;

  initStatusBar();
  initActions();
  initDocument();
  initModel();

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

  QAction *a = actionCollection()->action(QString("mode_%1").arg(QString::number(Prefs::mode())));
  slotModeActionGroupTriggered(a);

  m_prefDialog = 0;

  editMarkBlank->setEnabled(Prefs::enableBlanks());
  editUnmarkBlank->setEnabled(Prefs::enableBlanks());

  if (Prefs::firstRun())
  {
    fileOpenRecent->addUrl(KUrl::fromPath(KStandardDirs::locate("data", "kwordquiz/examples/example.kvtml")));
    fileOpenRecent->addUrl(KUrl::fromPath(KStandardDirs::locate("data", "kwordquiz/examples/french_verbs.kvtml")));
    fileOpenRecent->addUrl(KUrl::fromPath(KStandardDirs::locate("data", "kwordquiz/examples/fill_in_the_blank.kvtml")));
    fileOpenRecent->addUrl(KUrl::fromPath(KStandardDirs::locate("data", "kwordquiz/examples/us_states_and_capitals.kvtml")));
    Prefs::setFirstRun(false);
  }
  m_doc->setModified(false);
}

KWordQuizApp::~KWordQuizApp()
{

}

void KWordQuizApp::initActions()
{
  QAction* configToolbar;
  QAction* configNotifications;
  QAction* configApp;

  fileNew = KStandardAction::openNew(this, SLOT(slotFileNew()), actionCollection());
  fileNew->setWhatsThis(i18n("Creates a new blank vocabulary document"));
  fileNew->setToolTip(fileNew->whatsThis());
  fileNew->setStatusTip(fileNew->whatsThis());

  fileOpen = KStandardAction::open(this, SLOT(slotFileOpen()), actionCollection());
  fileOpen->setWhatsThis(i18n("Opens an existing vocabulary document"));
  fileOpen->setToolTip(fileOpen->whatsThis());
  fileOpen->setStatusTip(fileOpen->whatsThis());

  fileOpenRecent = KStandardAction::openRecent(this, SLOT(slotFileOpenRecent(const KUrl&)), actionCollection());

  fileGHNS = KNS::standardAction(i18n("Vocabularies..."), this, SLOT(slotFileGHNS()), actionCollection(), "file_ghns");
  fileGHNS->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_G));
  fileGHNS->setWhatsThis(i18n("Downloads new vocabularies"));
  fileGHNS->setToolTip(fileGHNS->whatsThis());
  fileGHNS->setStatusTip(fileGHNS->whatsThis());

  fileSave = KStandardAction::save(this, SLOT(slotFileSave()), actionCollection());
  fileSave->setWhatsThis(i18n("Saves the active vocabulary document"));
  fileSave->setToolTip(fileSave->whatsThis());
  fileSave->setStatusTip(fileSave->whatsThis());

  fileSaveAs = KStandardAction::saveAs(this, SLOT(slotFileSaveAs()), actionCollection());
  fileSaveAs->setWhatsThis(i18n("Saves the active vocabulary document with a different name"));
  fileSaveAs->setToolTip(fileSaveAs->whatsThis());
  fileSaveAs->setStatusTip(fileSaveAs->whatsThis());

  fileClose = KStandardAction::close(this, SLOT(slotFileClose()), actionCollection());
  fileClose->setWhatsThis(i18n("Closes the active vocabulary document"));
  fileClose->setStatusTip(fileClose->whatsThis());

  filePrint = KStandardAction::print(this, SLOT(slotFilePrint()), actionCollection());
  filePrint->setWhatsThis(i18n("Prints the active vocabulary document"));
  filePrint->setToolTip(filePrint->whatsThis());
  filePrint->setStatusTip(filePrint->whatsThis());

  fileQuit = KStandardAction::quit(this, SLOT(slotFileQuit()), actionCollection());
  fileQuit->setWhatsThis(i18n("Quits KWordQuiz"));
  fileQuit->setToolTip(fileQuit->whatsThis());
  fileQuit->setStatusTip(fileQuit->whatsThis());

  editUndo = KStandardAction::undo(this, SLOT(slotEditUndo()), actionCollection());
  editUndo->setWhatsThis(i18n("Undoes the last command"));
  editUndo->setToolTip(editUndo->whatsThis());
  editUndo->setStatusTip(editUndo->whatsThis());

  editCut = KStandardAction::cut(this, SLOT(slotEditCut()), actionCollection());
  editCut->setWhatsThis(i18n("Cuts the text from the selected cells and places it on the clipboard"));
  editCut->setToolTip(editCut->whatsThis());
  editCut->setStatusTip(editCut->whatsThis());

  editCopy = KStandardAction::copy(this, SLOT(slotEditCopy()), actionCollection());
  editCopy->setWhatsThis(i18n("Copies the text from the selected cells and places it on the clipboard"));
  editCopy->setToolTip(editCopy->whatsThis());
  editCopy->setStatusTip(editCopy->whatsThis());

  editPaste = KStandardAction::paste(this, SLOT(slotEditPaste()), actionCollection());
  editPaste->setWhatsThis(i18n("Pastes previously cut or copied text from the clipboard into the selected cells"));
  editPaste->setToolTip(editPaste->whatsThis());
  editPaste->setStatusTip(editPaste->whatsThis());

  editClear = KStandardAction::clear(this, SLOT(slotEditClear()), actionCollection());
  editClear->setWhatsThis(i18n("Clears the content of the selected cells"));
  editClear->setToolTip(editClear->whatsThis());
  editClear->setStatusTip(editClear->whatsThis());

  editInsert = actionCollection()->addAction("edit_insert");
  editInsert->setIcon(KIcon("insert_table_row"));
  editInsert->setText(i18n("&Insert Row"));
  editInsert->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_I));
  editInsert->setWhatsThis(i18n("Inserts a new row above the current row"));
  editInsert->setToolTip(editInsert->whatsThis());
  editInsert->setStatusTip(editInsert->whatsThis());
  connect(editInsert, SIGNAL(triggered(bool)), this, SLOT(slotEditInsert()));

  editDelete = actionCollection()->addAction("edit_delete");
  editDelete->setIcon(KIcon("delete_table_row"));
  editDelete->setText(i18n("&Delete Row"));
  editDelete->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_K));
  editDelete->setWhatsThis(i18n("Deletes the selected row(s)"));
  editDelete->setToolTip(editDelete->whatsThis());
  editDelete->setStatusTip(editDelete->whatsThis());
  connect(editDelete, SIGNAL(triggered(bool)), this, SLOT(slotEditDelete()));

  editMarkBlank = actionCollection()->addAction("edit_mark_blank");
  editMarkBlank->setIcon(KIcon("markasblank"));
  editMarkBlank->setText(i18n("&Mark as Blank"));
  editMarkBlank->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_M));
  editMarkBlank->setWhatsThis(i18n("Marks the current or selected word as a blank for Fill-in-the-blank"));
  editMarkBlank->setToolTip(editMarkBlank->whatsThis());
  editMarkBlank->setStatusTip(editMarkBlank->whatsThis());
  connect(editMarkBlank, SIGNAL(triggered(bool)), this, SLOT(slotEditMarkBlank()));

  editUnmarkBlank = actionCollection()->addAction("edit_unmark_blank");
  editUnmarkBlank->setIcon(KIcon("unmarkasblank"));
  editUnmarkBlank->setText(i18n("&Unmark Blanks"));
  editUnmarkBlank->setWhatsThis(i18n("Removes blanks from the current or selected word"));
  editUnmarkBlank->setToolTip(editUnmarkBlank->whatsThis());
  editUnmarkBlank->setStatusTip(editUnmarkBlank->whatsThis());
  connect(editUnmarkBlank, SIGNAL(triggered(bool)), this, SLOT(slotEditUnmarkBlank()));

  vocabLanguages = actionCollection()->addAction("vocab_languages");
  vocabLanguages->setIcon(KIcon("languages"));
  vocabLanguages->setText(i18n("&Column Titles..."));
  vocabLanguages->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_L));
  vocabLanguages->setWhatsThis(i18n("Defines the column titles for the active vocabulary"));
  vocabLanguages->setToolTip(vocabLanguages->whatsThis());
  vocabLanguages->setStatusTip(vocabLanguages->whatsThis());
  connect(vocabLanguages, SIGNAL(triggered(bool)), this, SLOT(slotVocabLanguages()));

  vocabFont = actionCollection()->addAction("vocab_font");
  vocabFont->setIcon(KIcon("fonts"));
  vocabFont->setText(i18n("&Font..."));
  vocabFont->setWhatsThis(i18n("Defines the font used by the editor"));
  vocabFont->setToolTip(vocabFont->whatsThis());
  vocabFont->setStatusTip(vocabFont->whatsThis());
  connect(vocabFont, SIGNAL(triggered(bool)), this, SLOT(slotVocabFont()));

  //@todo implement vocabKeyboard = new KAction(i18n("&Keyboard..."), "kxkb", 0, this, SLOT(slotVocabKeyboard()), actionCollection(),"vocab_keyboard");

  vocabRC = actionCollection()->addAction("vocab_rc");
  vocabRC->setIcon(KIcon("rowcol"));
  vocabRC->setText(i18n("&Rows/Columns..."));
  vocabRC->setWhatsThis(i18n("Defines the number of rows, row heights, and column widths for the active vocabulary"));
  vocabRC->setToolTip(vocabRC->whatsThis());
  vocabRC->setStatusTip(vocabRC->whatsThis());
  connect(vocabRC, SIGNAL(triggered(bool)), this, SLOT(slotVocabRC()));

  vocabSort = actionCollection()->addAction("vocab_sort");
  vocabSort->setIcon(KIcon("sort_incr"));
  vocabSort->setText(i18n("&Sort..."));
  vocabSort->setWhatsThis(i18n("Sorts the vocabulary in ascending or descending order based on the left or right column"));
  vocabSort->setToolTip(vocabSort->whatsThis());
  vocabSort->setStatusTip(vocabSort->whatsThis());
  connect(vocabSort, SIGNAL(triggered(bool)), this, SLOT(slotVocabSort()));

  vocabShuffle = actionCollection()->addAction("vocab_shuffle");
  vocabShuffle->setIcon(KIcon("shuffle"));
  vocabShuffle->setText(i18n("Sh&uffle"));
  vocabShuffle->setWhatsThis(i18n("Shuffles the entries of the active vocabulary"));
  vocabShuffle->setToolTip(vocabShuffle->whatsThis());
  vocabShuffle->setStatusTip(vocabShuffle->whatsThis());
  connect(vocabShuffle, SIGNAL(triggered(bool)), this, SLOT(slotVocabShuffle()));

  vocabLeitner = actionCollection()->addAction("vocab_leitner");
  vocabLeitner->setIcon(KIcon("leitner"));
  vocabLeitner->setText(i18n("Enable Leitner system"));
  vocabLeitner->setWhatsThis(i18n("Enables the Leitner system for the active vocabulary"));
  vocabLeitner->setToolTip(vocabLeitner->whatsThis());
  vocabLeitner->setStatusTip(vocabLeitner->whatsThis());
  connect(vocabLeitner, SIGNAL(triggered(bool)), this, SLOT(slotLeitnerSystem()));

  vocabConfigLeitner = actionCollection()->addAction("vocab_leitner_config");
  vocabConfigLeitner->setIcon(KIcon("config_leitner"));
  vocabConfigLeitner->setText(i18n("Configure Leitner system"));
  vocabConfigLeitner->setWhatsThis(i18n("Configure the Leitner system used for the active vocabulary"));
  vocabConfigLeitner->setToolTip(vocabConfigLeitner->whatsThis());
  vocabConfigLeitner->setStatusTip(vocabConfigLeitner->whatsThis());
  vocabConfigLeitner->setEnabled(false);
  connect(vocabConfigLeitner, SIGNAL(triggered(bool)), this, SLOT(slotConfigLeitner()));

  mode = actionCollection()->add<KActionMenu>("mode_0");
  mode->setIcon(KIcon("mode1"));
  mode->setText(i18n("Change Mode"));
  mode->setWhatsThis(i18n("Changes the mode used in quiz sessions"));
  mode->setToolTip(mode->whatsThis());
  mode->setStatusTip(mode->whatsThis());
  connect(mode, SIGNAL(triggered(bool)), this, SLOT(slotModeChange()));

  m_modeActionGroup = new QActionGroup(this);
  connect(m_modeActionGroup, SIGNAL(triggered(QAction *)), this, SLOT(slotModeActionGroupTriggered(QAction *)));

  mode1 = actionCollection()->addAction("mode_1");
  mode1->setData(1);
  mode1->setCheckable(true);
  mode1->setIcon(KIcon("mode1"));
  mode1->setText("");
  mode1->setWhatsThis(i18n("Selects this mode"));
  mode1->setToolTip(mode1->whatsThis());
  mode1->setStatusTip(mode1->whatsThis());
  mode->addAction(mode1);
  m_modeActionGroup->addAction(mode1);

  mode2 = actionCollection()->addAction("mode_2");
  mode2->setData(2);
  mode2->setCheckable(true);
  mode2->setIcon(KIcon("mode2"));
  mode2->setText("");
  mode2->setWhatsThis(i18n("Selects this mode"));
  mode2->setToolTip(mode2->whatsThis());
  mode2->setStatusTip(mode2->whatsThis());
  mode->addAction(mode2);
  m_modeActionGroup->addAction(mode2);

  mode3 = actionCollection()->addAction("mode_3");
  mode3->setData(3);
  mode3->setCheckable(true);
  mode3->setIcon(KIcon("mode3"));
  mode3->setText("");
  mode3->setWhatsThis(i18n("Selects this mode"));
  mode3->setToolTip(mode3->whatsThis());
  mode3->setStatusTip(mode3->whatsThis());
  mode->addAction(mode3);
  m_modeActionGroup->addAction(mode3);

  mode4 = actionCollection()->addAction("mode_4");
  mode4->setData(4);
  mode4->setCheckable(true);
  mode4->setIcon(KIcon("mode4"));
  mode4->setText("");
  mode4->setWhatsThis(i18n("Selects this mode"));
  mode4->setToolTip(mode4->whatsThis());
  mode4->setStatusTip(mode4->whatsThis());
  mode->addAction(mode4);
  m_modeActionGroup->addAction(mode4);

  mode5 = actionCollection()->addAction("mode_5");
  mode5->setData(5);
  mode5->setCheckable(true);
  mode5->setIcon(KIcon("mode5"));
  mode5->setText("");
  mode5->setWhatsThis(i18n("Selects this mode"));
  mode5->setToolTip(mode5->whatsThis());
  mode5->setStatusTip(mode5->whatsThis());
  mode->addAction(mode5);
  m_modeActionGroup->addAction(mode5);

  quizEditor = actionCollection()->addAction("quiz_editor");
  quizEditor->setIcon(KIcon("editor"));
  quizEditor->setText(i18n("&Editor"));
  quizEditor->setShortcut(QKeySequence(Qt::Key_F6));
  quizEditor->setWhatsThis(i18n("Activates the vocabulary editor"));
  quizEditor->setToolTip(quizEditor->whatsThis());
  quizEditor->setStatusTip(quizEditor->whatsThis());
  connect(quizEditor, SIGNAL(triggered(bool)), this, SLOT(slotQuizEditor()));

  quizFlash = actionCollection()->addAction("quiz_flash");
  quizFlash->setIcon(KIcon("flash"));
  quizFlash->setText(i18n("&Flashcard"));
  quizFlash->setShortcut(QKeySequence(Qt::Key_F7));
  quizFlash->setWhatsThis(i18n("Starts a flashcard session using the active vocabulary"));
  quizFlash->setToolTip(quizFlash->whatsThis());
  quizFlash->setStatusTip(quizFlash->whatsThis());
  connect(quizFlash, SIGNAL(triggered(bool)), this, SLOT(slotQuizFlash()));

  quizMultiple = actionCollection()->addAction("quiz_multiple");
  quizMultiple->setIcon(KIcon("multiple"));
  quizMultiple->setText(i18n("&Multiple Choice"));
  quizMultiple->setShortcut(QKeySequence(Qt::Key_F8));
  quizMultiple->setWhatsThis(i18n("Starts a multiple choice session using the active vocabulary"));
  quizMultiple->setToolTip(quizMultiple->whatsThis());
  quizMultiple->setStatusTip(quizMultiple->whatsThis());
  connect(quizMultiple, SIGNAL(triggered(bool)), this, SLOT(slotQuizMultiple()));

  quizQA = actionCollection()->addAction("quiz_qa");
  quizQA->setIcon(KIcon("qa"));
  quizQA->setText(i18n("&Question && Answer"));
  quizQA->setShortcut(QKeySequence(Qt::Key_F9));
  quizQA->setWhatsThis(i18n("Starts a question and answer session using the active vocabulary"));
  quizQA->setToolTip(quizQA->whatsThis());
  quizQA->setStatusTip(quizQA->whatsThis());
  connect(quizQA, SIGNAL(triggered(bool)), this, SLOT(slotQuizQA()));

  quizCheck = actionCollection()->addAction("quiz_check");
  quizCheck->setIcon(KIcon("check"));
  quizCheck->setText(i18n("&Check"));
  quizCheck->setShortcut(QKeySequence(Qt::Key_Return));
  quizCheck->setWhatsThis(i18n("Checks your answer to this question"));
  quizCheck->setToolTip(quizCheck->whatsThis());
  quizCheck->setStatusTip(quizCheck->whatsThis());

  flashKnow = actionCollection()->addAction("flash_know");
  flashKnow->setIcon(KIcon("know"));
  flashKnow->setText(i18n("I &Know"));
  flashKnow->setShortcut(QKeySequence(Qt::Key_K));
  flashKnow->setWhatsThis(i18n("Counts this card as correct and shows the next card"));
  flashKnow->setToolTip(flashKnow->whatsThis());
  flashKnow->setStatusTip(flashKnow->whatsThis());

  flashDontKnow = actionCollection()->addAction("flash_dont_know");
  flashDontKnow->setIcon(KIcon("dontknow"));
  flashDontKnow->setText(i18n("I &Do Not Know"));
  flashDontKnow->setShortcut(QKeySequence(Qt::Key_D));
  flashDontKnow->setWhatsThis(i18n("Counts this card as incorrect and shows the next card"));
  flashDontKnow->setToolTip(flashDontKnow->whatsThis());
  flashDontKnow->setStatusTip(flashDontKnow->whatsThis());

  qaHint = actionCollection()->addAction("qa_hint");
  qaHint->setIcon(KIcon("hint"));
  qaHint->setText(i18n("&Hint"));
  qaHint->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_H));
  qaHint->setWhatsThis(i18n("Gets the next correct letter of the answer"));
  qaHint->setToolTip(qaHint->whatsThis());
  qaHint->setStatusTip(qaHint->whatsThis());

  quizRestart = actionCollection()->addAction("quiz_restart");
  quizRestart->setIcon(KIcon("restart"));
  quizRestart->setText(i18n("&Restart"));
  quizRestart->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_R));
  quizRestart->setWhatsThis(i18n("Restarts the quiz session from the beginning"));
  quizRestart->setToolTip(quizRestart->whatsThis());
  quizRestart->setStatusTip(quizRestart->whatsThis());

  quizRepeatErrors = actionCollection()->addAction("quiz_repeat_errors");
  quizRepeatErrors->setIcon(KIcon("repeat"));
  quizRepeatErrors->setText(i18n("Repeat &Errors"));
  quizRepeatErrors->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_E));
  quizRepeatErrors->setWhatsThis(i18n("Repeats all incorrectly answered questions"));
  quizRepeatErrors->setToolTip(quizRepeatErrors->whatsThis());
  quizRepeatErrors->setStatusTip(quizRepeatErrors->whatsThis());

  configShowSearchBar = actionCollection()->add<KToggleAction>("config_show_search");
  configShowSearchBar->setText(i18n("Show Se&arch"));
  connect(configShowSearchBar, SIGNAL(triggered(bool)), this, SLOT(slotConfigShowSearch()));
  configShowSearchBar->setWhatsThis(i18n("Toggle display of the search bar"));
  configShowSearchBar->setToolTip(configShowSearchBar->whatsThis());
  configShowSearchBar->setStatusTip(configShowSearchBar->whatsThis());

  configNotifications = KStandardAction::configureNotifications(this, SLOT(slotConfigureNotifications()), actionCollection());
  configNotifications->setWhatsThis(i18n("Configures sound and other notifications for certain events"));
  configNotifications->setToolTip(configNotifications->whatsThis());
  configNotifications->setStatusTip(configNotifications->whatsThis());

  configApp = KStandardAction::preferences(this, SLOT( slotConfigure()), actionCollection());
  configApp->setWhatsThis(i18n("Specifies preferences for the vocabulary editor and quiz sessions"));
  configApp->setToolTip(configApp->whatsThis());
  configApp->setToolTip(configApp->whatsThis());

  charMapper = new QSignalMapper(this);
  connect(charMapper, SIGNAL(mapped(int)), this, SLOT(slotInsertChar(int)));

  specialChar1 = actionCollection()->addAction("char_1") ;
  specialChar1->setText(i18n("Special Character 1"));
  specialChar1->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_1));
  specialChar1->setWhatsThis(i18n("Insert this character"));
  specialChar1->setToolTip(specialChar1->whatsThis());
  specialChar1->setStatusTip(specialChar1->whatsThis());
  connect(specialChar1, SIGNAL(triggered(bool)), charMapper, SLOT(map()));

  specialChar2 = actionCollection()->addAction("char_2") ;
  specialChar2->setText(i18n("Special Character 2"));
  specialChar2->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_2));
  specialChar2->setWhatsThis(i18n("Insert this character"));
  specialChar2->setToolTip(specialChar2->whatsThis());
  specialChar2->setStatusTip(specialChar2->whatsThis());
  connect(specialChar2, SIGNAL(triggered(bool)), charMapper, SLOT(map()));

  specialChar3 = actionCollection()->addAction("char_3") ;
  specialChar3->setText(i18n("Special Character 3"));
  specialChar3->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_3));
  specialChar3->setWhatsThis(i18n("Insert this character"));
  specialChar3->setToolTip(specialChar3->whatsThis());
  specialChar3->setStatusTip(specialChar3->whatsThis());
  connect(specialChar3, SIGNAL(triggered(bool)), charMapper, SLOT(map()));

  specialChar4 = actionCollection()->addAction("char_4") ;
  specialChar4->setText(i18n("Special Character 4"));
  specialChar4->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_4));
  specialChar4->setWhatsThis(i18n("Insert this character"));
  specialChar4->setToolTip(specialChar4->whatsThis());
  specialChar4->setStatusTip(specialChar4->whatsThis());
  connect(specialChar4, SIGNAL(triggered(bool)), charMapper, SLOT(map()));

  specialChar5 = actionCollection()->addAction("char_5") ;
  specialChar5->setText(i18n("Special Character 5"));
  specialChar5->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_5));
  specialChar5->setWhatsThis(i18n("Insert this character"));
  specialChar5->setToolTip(specialChar5->whatsThis());
  specialChar5->setStatusTip(specialChar5->whatsThis());
  connect(specialChar5, SIGNAL(triggered(bool)), charMapper, SLOT(map()));

  specialChar6 = actionCollection()->addAction("char_6") ;
  specialChar6->setText(i18n("Special Character 6"));
  specialChar6->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_6));
  specialChar6->setWhatsThis(i18n("Insert this character"));
  specialChar6->setToolTip(specialChar6->whatsThis());
  specialChar6->setStatusTip(specialChar6->whatsThis());
  connect(specialChar6, SIGNAL(triggered(bool)), charMapper, SLOT(map()));

  specialChar7 = actionCollection()->addAction("char_7") ;
  specialChar7->setText(i18n("Special Character 7"));
  specialChar7->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_7));
  specialChar7->setWhatsThis(i18n("Insert this character"));
  specialChar7->setToolTip(specialChar7->whatsThis());
  specialChar7->setStatusTip(specialChar7->whatsThis());
  connect(specialChar7, SIGNAL(triggered(bool)), charMapper, SLOT(map()));

  specialChar8 = actionCollection()->addAction("char_8") ;
  specialChar8->setText(i18n("Special Character 8"));
  specialChar8->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_8));
  specialChar8->setWhatsThis(i18n("Insert this character"));
  specialChar8->setToolTip(specialChar8->whatsThis());
  specialChar8->setStatusTip(specialChar8->whatsThis());
  connect(specialChar8, SIGNAL(triggered(bool)), charMapper, SLOT(map()));

  specialChar9 = actionCollection()->addAction("char_9") ;
  specialChar9->setText(i18n("Special Character 9"));
  specialChar9->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_9));
  specialChar9->setWhatsThis(i18n("Insert this character"));
  specialChar9->setToolTip(specialChar9->whatsThis());
  specialChar9->setStatusTip(specialChar9->whatsThis());
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

  updateSpecialCharIcons();

  if (!initialGeometrySet())
    resize( QSize(650, 500).expandedTo(minimumSizeHint()));
  setupGUI(ToolBar | Keys | StatusBar | Create);
  setAutoSaveSettings();

  configToolbar = actionCollection()->action("options_configure_toolbars");
  configToolbar->setWhatsThis(i18n("Toggles display of the toolbars"));
  configToolbar->setToolTip(configToolbar->whatsThis());
  configToolbar->setStatusTip(configToolbar->whatsThis());
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
  m_doc = new KEduVocDocument(this);
  m_doc->appendIdentifier(i18n("Column 1"));
  m_doc->appendIdentifier(i18n("Column 2"));
  for (int i=0; i<20; i++)
  {
    m_doc->appendEntry(new KEduVocExpression());
  }
  m_doc->setModified(false);
}

void KWordQuizApp::initModel()
{
  m_tableModel = new KWQTableModel(this);
  m_tableModel->setDocument(m_doc);
  m_tableModel->setHeaderData(0, Qt::Horizontal, QSize(250, 25), Qt::SizeHintRole);
  m_tableModel->setHeaderData(1, Qt::Horizontal, QSize(250, 25), Qt::SizeHintRole);
}

void KWordQuizApp::initView()
{
  QWidget * mainWidget = new QWidget(this);
  setCentralWidget(mainWidget);
  m_topLayout = new QVBoxLayout(mainWidget);
  m_topLayout->setMargin(0);
  m_topLayout->setSpacing(KDialog::spacingHint());

  m_searchLine = new KLineEdit(this);
  m_searchLine->show();
  m_searchLine->setFocusPolicy(Qt::ClickFocus);
  m_searchLine->setClearButtonShown(true);
  m_searchLine->setClickMessage(i18n("Enter search terms here"));
  connect(m_searchLine, SIGNAL(textChanged(const QString&)), this, SLOT(slotEditFind(const QString&)));

  QLabel *label = new QLabel(i18n("S&earch:"), this);
  label->setBuddy(m_searchLine);
  label->show();

  m_searchWidget = new QWidget(this);
  QHBoxLayout* layout = new QHBoxLayout(m_searchWidget);
  layout->setSpacing(KDialog::spacingHint());
  layout->setMargin(KDialog::marginHint());
  layout->addWidget(label);
  layout->addWidget(m_searchLine);

  m_tableView = new KWQTableView(centralWidget());
  m_tableView->setFrameStyle(QFrame::NoFrame);
  m_topLayout->addWidget(m_searchWidget);
  m_topLayout->addWidget(m_tableView);

  m_tableView->setModel(m_tableModel);
  m_tableView->setColumnWidth(0, qvariant_cast<QSize>(m_tableModel->headerData(0, Qt::Horizontal, Qt::SizeHintRole)).width());
  m_tableView->setColumnWidth(1, qvariant_cast<QSize>(m_tableModel->headerData(1, Qt::Horizontal, Qt::SizeHintRole)).width());
  setCaption(m_doc->url().fileName(),false);
  connect(m_tableView, SIGNAL(undoChange(const QString&, bool )), this, SLOT(slotUndoChange(const QString&, bool)));
  connect(m_tableModel, SIGNAL(modelReset()), m_tableView, SLOT(slotModelReset()));

  m_searchWidget->setVisible(Prefs::showSearch());
  configShowSearchBar->setChecked(Prefs::showSearch());

  m_doc->setModified(false);
}

void KWordQuizApp::openUrl(const KUrl& url)
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
          if(a->m_doc->url().path() == url.path())
          {
            if (w->isMinimized())
              w->showNormal();
            w->activateWindow();
            w->raise();
            break;
          }
        }
      }
    }
    else
    {
      if (m_tableModel->isEmpty()){
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
    m_doc->open(url);
    m_tableModel->reset();
    m_dirWatch->addFile(url.path());
    setCaption(m_doc->url().fileName(), false);
    fileOpenRecent->addUrl(url);
    QAction *a = actionCollection()->action(QString("mode_%1").arg(QString::number(Prefs::mode())));
    slotModeActionGroupTriggered(a);
    m_doc->setModified(false);
  }
  slotStatusMsg(i18n("Ready"));
}

KEduVocDocument *KWordQuizApp::document() const
{
  return m_doc;
}

void KWordQuizApp::saveOptions()
{
  fileOpenRecent->saveEntries(KGlobal::config()->group( "Recent Files") );
  Prefs::writeConfig();
}

void KWordQuizApp::readOptions()
{
  fileOpenRecent->loadEntries(KGlobal::config()->group( "Recent Files") );
}

void KWordQuizApp::saveProperties(KConfigGroup &_cfg)
{
  if(m_doc->url().fileName()!=i18n("Untitled") && !m_doc->isModified())
  {
    // saving to tempfile not necessary
  }
  else
  {
    KUrl url = m_doc->url();
    _cfg.writeEntry("filename", url.url());
    _cfg.writeEntry("modified", m_doc->isModified());
    QString tempname = kapp->tempSaveName(url.url());
    QString tempurl = KUrl::toPercentEncoding(tempname);
    KUrl _url(tempurl);
    m_doc->saveAs(_url, KEduVocDocument::automatic, QString("kwordquiz %1").arg(KWQ_VERSION));
  }
}

void KWordQuizApp::readProperties(const KConfigGroup &_cfg)
{
  QString filename = _cfg.readEntry("filename", "");
  KUrl url(filename);
  bool modified = _cfg.readEntry("modified", false);
  if(modified)
  {
    bool canRecover;
    QString tempname = kapp->checkRecoverFile(filename, canRecover);
    KUrl _url(tempname);

    if(canRecover)
    {
      m_doc->open(_url);
      m_doc->setModified();
      setCaption(_url.fileName(),true);
      QFile::remove(tempname);
    }
  }
  else
  {
    if(!filename.isEmpty())
    {
      m_doc->open(url);
      setCaption(url.fileName(),false);
    }
  }
}

bool KWordQuizApp::queryClose()
{
  bool completed=true;
  if(m_doc->isModified())
  {
    int want_save = KMessageBox::warningYesNoCancel(this, i18n("The current document has been modified.\nDo you want to save it?"));
    switch(want_save)
    {
      case KMessageBox::Cancel:
        completed = false;
        break;

      case KMessageBox::Yes:
        if (m_doc->url().fileName() == i18n("Untitled"))
        {
          completed = saveAsFileName();
        }
        else
        {
          completed = m_doc->saveAs(m_doc->url(), KEduVocDocument::automatic, QString("kwordquiz %1").arg(KWQ_VERSION));
        }

        break;

      case KMessageBox::No:
        m_doc->setModified(false);
        completed = true;
        break;

      default:
        completed = false;
        break;
    }
  }

  if (completed)
    if (m_dirWatch->contains(m_doc->url().path()))
      m_dirWatch->removeFile(m_doc->url().path());
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

  for (int r = 0; r < m_tableModel->rowCount(QModelIndex()); ++r)
  {
    QString s = m_tableModel->data(m_tableModel->index(r, 0, QModelIndex()), Qt::DisplayRole).toString();
    if (s.length() > 0)
      for (int i = 0; i <= s.length(); ++i)
        if (s[i] == delim_start || s[i] == delim_end)
          if (!m_tableView->checkForBlank(s, blanks))
            errorCount++;
    s = m_tableModel->data(m_tableModel->index(r, 1, QModelIndex()), Qt::DisplayRole).toString();
    if (s.length() > 0)
      for (int i = 0; i <= s.length(); ++i)
        if (s[i] == delim_start || s[i] == delim_end)
          if (!m_tableView->checkForBlank(s, blanks))
            errorCount++;
    }
  return (errorCount == 0);
}

void KWordQuizApp::slotFileNew()
{
  slotStatusMsg(i18n("Opening a new document window..."));
  if (!m_tableModel->isEmpty()){
    KWordQuizApp *new_window= new KWordQuizApp();
    new_window->show();
  }
  slotStatusMsg(i18n("Ready"));
}

void KWordQuizApp::slotFileOpen()
{
  slotStatusMsg(i18n("Opening file..."));

  QCheckBox * cb = new QCheckBox(i18n("&Merge selected files with the current document"), 0);
  cb -> setChecked(false);
  cb -> setEnabled(true);

  QString filter = KEduVocDocument::pattern(KEduVocDocument::Reading);
  KFileDialog *fd = new KFileDialog(QString(), filter, this, cb);
  fd->setOperationMode(KFileDialog::Opening);
  fd->setMode(KFile::Files | KFile::ExistingOnly);
  fd->setCaption(i18n("Open Vocabulary Document"));

  if (fd->exec() == QDialog::Accepted)
  {
    KUrl::List l = fd->selectedUrls();
    bool append = ((cb->isChecked()) && (l.count() >= 1));

    if (append)
    {
      KUrl::List::iterator it;
      for(it = l.begin(); it != l.end(); ++it)
      {
        KEduVocDocument *new_doc = new KEduVocDocument(this);
        new_doc->open(*it);

        m_doc->merge(new_doc, false);
        delete (new_doc);
      }
      m_tableModel->reset();
    }
    else
    {
      KUrl::List::iterator it;
      for(it = l.begin(); it != l.end(); ++it)
      {
        openUrl(*it);
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
  openUrl(url);
  slotStatusMsg(i18n("Ready"));
}

void KWordQuizApp::slotFileSave()
{
  slotStatusMsg(i18n("Saving file..."));
  if (m_doc->url().fileName() == i18n("Untitled") )
  {
    slotFileSaveAs();
  }
  else
  {
    if (!m_doc->saveAs(m_doc->url(), KEduVocDocument::automatic, QString("kwordquiz %1").arg(KWQ_VERSION))) {
      slotFileSaveAs();
    }
  }
  slotStatusMsg(i18n("Ready"));
}

void KWordQuizApp::slotFileGHNS()
{
  KNS::Entry::List entries = KNS::Engine::download();
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

  QString filter = KEduVocDocument::pattern(KEduVocDocument::Writing);
  KFileDialog *fd = new KFileDialog(QString(), filter, this);
  fd -> setOperationMode(KFileDialog::Saving);
  fd -> setCaption(i18n("Save Vocabulary Document As"));

  if (fd->exec() == QDialog::Accepted)
  {
    KUrl url = fd -> selectedUrl();
    if(!url.isEmpty()){
      if (!url.fileName().contains('.'))
      {
        if (fd->currentFilter() == "*.csv")
          url = KUrl(url.path() + ".csv");
        else
          url = KUrl(url.path() + ".kvtml");
      }

      QFileInfo fileinfo(url.path());
      if (fileinfo.exists() && KMessageBox::warningContinueCancel(0,
          i18n("<qt>The file<br><b>%1</b><br>already exists. Do you want to overwrite it?</qt>",
               url.path()),QString(),KStandardGuiItem::overwrite()) == KMessageBox::Cancel)
      {
      // do nothing
      }
      else
      {
        if (m_dirWatch ->contains(m_doc->url().path()))
          m_dirWatch ->removeFile(m_doc->url().path());
        m_doc->saveAs(url, KEduVocDocument::automatic, QString("kwordquiz %1").arg(KWQ_VERSION));
        m_dirWatch->addFile(url.path());
        fileOpenRecent->addUrl(url);
        setCaption(m_doc->url().fileName(), m_doc->isModified());
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
      delete m_doc;
      initDocument();
      setCaption(m_doc->url().fileName(), m_doc->isModified());
      m_tableModel->setDocument(m_doc);
      slotQuizEditor();
      slotUndoChange(i18n("Cannot &Undo"), false);
      QAction *a = actionCollection()->action(QString("mode_%1").arg(QString::number(Prefs::mode())));
      slotModeActionGroupTriggered(a);
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

void KWordQuizApp::slotUndoChange(const QString & text, bool enabled)
{
  editUndo->setText(text);
  editUndo->setEnabled(enabled);
  m_doc->setModified(true);
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

void KWordQuizApp::slotEditFind(const QString & find)
{
  if (find.isEmpty()) {
    for (int i = 0; i < m_tableModel->rowCount(QModelIndex()); i++)
      m_tableView->setRowHidden(i, false);
    return;
  }
  for (int i = 0; i < m_tableModel->rowCount(QModelIndex()); i++)
    m_tableView->setRowHidden(i, true);

  QModelIndexList list = m_tableModel->match(m_tableModel->index(0, 0, QModelIndex()), Qt::DisplayRole, QVariant(find), m_tableModel->rowCount(QModelIndex()),
                                             Qt::MatchContains | Qt::MatchWrap);

  QModelIndexList list2 = m_tableModel->match(m_tableModel->index(0, 1, QModelIndex()), Qt::DisplayRole, QVariant(find), m_tableModel->rowCount(QModelIndex()),
                                              Qt::MatchContains | Qt::MatchWrap);

  foreach(QModelIndex index, list2)
    list.append(index);

  foreach(QModelIndex index, list)
    m_tableView->setRowHidden(index.row(), false);
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
    QAction *a = actionCollection()->action(QString("mode_%1").arg(QString::number(Prefs::mode())));
    slotModeActionGroupTriggered(a);
  }
  slotStatusMsg(i18n("Ready"));
}

void KWordQuizApp::slotVocabFont()
{
  slotStatusMsg(i18n("Setting the font of the vocabulary..."));
  KFontDialog* dlg;
  dlg = new KFontDialog(this);
  dlg->setObjectName("dlg_font");
  dlg->setFont(Prefs::editorFont());
  if (dlg->exec() == KFontDialog::Accepted)
  {
    m_tableView->reset();
    Prefs::setEditorFont(dlg->font());
    m_doc->setModified(true);
  }
  delete dlg;
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
  slotStatusMsg(i18n("Ready"));
}

void KWordQuizApp::slotModeChange()
{
  QString newMode;
  slotStatusMsg(i18n("Updating mode..."));
  if (Prefs::mode() < 5)
    newMode = QString("mode_%1").arg(QString::number(Prefs::mode() + 1));
  else
    newMode = QString("mode_1");

  QAction *a = actionCollection()->action(newMode);
  a->activate(QAction::Trigger);
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

  if ((qt != WQQuiz::qtEditor) && Prefs::enableBlanks()) {
    if (!checkSyntax(true))
      return;
  }

  switch (qt){
    case WQQuiz::qtEditor:
      m_tableView->show();
      m_tableView -> setFocus();
      m_searchWidget->setVisible(Prefs::showSearch());
      m_quizType = qt;
      break;
    case WQQuiz::qtFlash:
      m_quiz = new WQQuiz(this, m_doc);
      connect(m_quiz, SIGNAL(checkingAnswer(int )), m_tableView, SLOT(slotCheckedAnswer(int )));
      m_quiz ->setQuizType(WQQuiz::qtFlash);
      m_quiz->setQuizMode(Prefs::mode());
      if (m_quiz -> init())
      {
        m_tableView->hide();
        m_searchWidget->hide();
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
        m_quizType = qt;
      }
      else
      {
        delete(m_quiz);
        m_quiz = 0;
      }
      break;
    case WQQuiz::qtMultiple:
      m_quiz = new WQQuiz(this, m_doc);
      connect(m_quiz, SIGNAL(checkingAnswer(int )), m_tableView, SLOT(slotCheckedAnswer(int )));
      m_quiz ->setQuizType(WQQuiz::qtMultiple);
      m_quiz->setQuizMode(Prefs::mode());
      if (m_quiz -> init())
      {
        //m_tableView->saveCurrentSelection(true);
        m_tableView->hide();
        m_searchWidget->hide();
        m_multipleView = new MultipleView(this);
        connect(quizCheck, SIGNAL(activated()), m_multipleView, SLOT(slotCheck()));
        connect(quizRestart, SIGNAL(activated()), m_multipleView, SLOT(slotRestart()));
        connect(quizRepeatErrors, SIGNAL(activated()), m_multipleView, SLOT(slotRepeat()));
        connect(this, SIGNAL(settingsChanged()), m_multipleView, SLOT(slotApplySettings()));

        m_multipleView -> setQuiz(m_quiz);
        m_multipleView ->init();
        m_multipleView->show();
        m_topLayout->addWidget(m_multipleView);
        m_quizType = qt;
      }
      else
      {
        delete(m_quiz);
        m_quiz = 0;
      }
      break;
    case WQQuiz::qtQA:
      m_quiz = new WQQuiz(this, m_doc);
      connect(m_quiz, SIGNAL(checkingAnswer(int )), m_tableView, SLOT(slotCheckedAnswer(int )));
      m_quiz ->setQuizType(WQQuiz::qtQA);
      m_quiz->setQuizMode(Prefs::mode());
      if (m_quiz -> init())
      {
        //m_tableView->saveCurrentSelection(true);
        m_tableView->hide();
        m_searchWidget->hide();
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
        m_quizType = qt;
      }
      else
      {
        delete(m_quiz);
        m_quiz = 0;
      }
      break;

  }
  updateActions(m_quizType);
}

/** Configure notifications */
void KWordQuizApp::slotConfigureNotifications( )
{
  KNotifyConfigWidget::configure(this);
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
  for (int i = 0; i < 9; i++){
    QAction * act = actionCollection()->action(QString("char_" + QString::number(i + 1)));
    act->setIcon(KIcon(charIcon(Prefs::specialCharacters()[i])));
    act->setToolTip(i18n("Inserts the character %1", Prefs::specialCharacters()[i]));
  }
}

QString KWordQuizApp::charIcon(const QChar & c)
{
  ///Create a name and path for the icon
  QString s = KStandardDirs::locateLocal("icon", "char" + QString::number(c.unicode()) + ".png");

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


void KWordQuizApp::slotModeActionGroupTriggered(QAction *act)
{
  if (m_quiz != 0)
    if (KMessageBox::warningContinueCancel(this, i18n("This will restart your quiz. Do you wish to continue?"), QString(), KStandardGuiItem::cont(), KStandardGuiItem::cancel(), "askModeQuiz")
      != KMessageBox::Continue)
      {
        mode1->setChecked(Prefs::mode() == 1);
        mode2->setChecked(Prefs::mode() == 2);
        mode3->setChecked(Prefs::mode() == 3);
        mode4->setChecked(Prefs::mode() == 4);
        mode5->setChecked(Prefs::mode() == 5);
        return;
      }

  act->setChecked(true);

  QString s1 = m_tableModel->headerData(0, Qt::Horizontal, Qt::DisplayRole).toString();
  QString s2 = m_tableModel->headerData(1, Qt::Horizontal, Qt::DisplayRole).toString();

  mode1->setText(i18n("&1 %1 -> %2 In Order", s1, s2));
  mode2->setText(i18n("&2 %1 -> %2 In Order", s2, s1));
  mode3->setText(i18n("&3 %1 -> %2 Randomly", s1, s2));
  mode4->setText(i18n("&4 %1 -> %2 Randomly", s2, s1));
  mode5->setText(i18n("&5 %1 <-> %2 Randomly", s1, s2));

  Prefs::setMode(act->data().toInt());
  mode->setIcon(KIcon("mode" + QString::number(Prefs::mode())));

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
    if (m_tableView->isVisible())
      m_tableView->slotSpecChar(Prefs::specialCharacters()[i - 1]);
}


void KWordQuizApp::contextMenuEvent(QContextMenuEvent * event)
{
  QWidget * w = guiFactory()->container("editor_popup", this);
  KMenu *popup = static_cast<KMenu *>(w);

  popup->exec( QPoint( event->globalX(), event->globalY() ) );
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

  //toolBar("quizToolBar")->setHidden(qt == WQQuiz::qtEditor);

}

void KWordQuizApp::slotLeitnerSystem()
{
  if(m_doc->leitnerSystemActive())
  {
    m_doc->setLeitnerSystemActive(false);
    vocabLeitner->setText(i18n("Enable Leitner system"));
    vocabConfigLeitner->setEnabled( false );
  }
  else
  {
    m_doc->setLeitnerSystemActive(true);
    vocabLeitner->setText(i18n("Disable Leitner system"));
    vocabConfigLeitner->setEnabled( true );
  }
}

void KWordQuizApp::slotConfigLeitner()
{
  PrefLeitner* config = new PrefLeitner(m_doc->leitnerSystem(), this);
  if (config->exec() == QDialog::Accepted)
    m_doc->setLeitnerSystem(config->system());

  delete config;
}

void KWordQuizApp::slotConfigShowSearch()
{
  if (m_searchWidget) {
    m_searchWidget->setVisible(m_searchWidget->isHidden());
    Prefs::setShowSearch(m_searchWidget->isVisible());
  }
}

#include "kwordquiz.moc"
