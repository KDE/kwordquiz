/***************************************************************************
                                kwordquiz.cpp
                             -------------------
    begin         : Wed Jul 24 20:12:30 PDT 2002
    copyright     : (C) 2002-2010 Peter Hedlund <peter.hedlund@kdemail.net>

 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "kwordquiz.h"

#include <QBitmap>
#include <QCheckBox>
#include <QDBusInterface>
#include <QDBusReply>
#include <QDebug>
#include <QFileDialog>
#include <QFontDialog>
#include <QIcon>
#include <QMenu>
#include <QMimeDatabase>
#include <QMimeType>
#include <QPainter>
#include <QPointer>
#include <QStandardPaths>
#include <QStatusBar>
#include <QTemporaryFile>

#include <KActionCollection>
#include <KActionMenu>
#include <KConfigGroup>
#include <KLocalizedString>
#include <KMessageBox>
#include <knewstuff_version.h>
#if KNEWSTUFF_VERSION < QT_VERSION_CHECK(5, 78, 0)
#include <KNS3/DownloadDialog>
#else
#include <KNS3/QtQuickDialogWrapper>
#endif
#include <KNotifyConfigWidget>
#include <KPageWidget>
#include <KProcess>
#include <KToggleAction>
#include <KToolBar>
#include <KUndoActions>
#include <KWindowSystem>
#include <KXMLGUIFactory>

#include "keduvocdocument.h"
#include "keduvoclesson.h"
#include "keduvocexpression.h"
#include "kwqtablemodel.h"
#include "kwqsortfiltermodel.h"
#include "kwqtableview.h"
#include "kwqcommands.h"
#include "dlglanguage.h"
#include "kwordquizprefs.h"
#include "qaview.h"
#include "flashview.h"
#include "multipleview.h"
#include "wqprintdialogpage.h"
#include "prefs.h"
#include "kwordquiz_version.h"

KWordQuizApp::KWordQuizApp(QWidget*):KXmlGuiWindow(0)
{
  m_quiz = 0;
  m_prefDialog = 0;

  initStatusBar();
  initActions();
  initDocument();
  initModel();

  readOptions();

  initView();

  m_dirWatch = KDirWatch::self();

  slotModeActionGroupTriggered(m_modeActionGroup->actions().at(Prefs::mode() - 1));

  editMarkBlank->setEnabled(Prefs::enableBlanks());
  editUnmarkBlank->setEnabled(Prefs::enableBlanks());

  if (Prefs::firstRun())
  {
    fileOpenRecent->addUrl(QUrl::fromLocalFile(QStandardPaths::locate(QStandardPaths::GenericDataLocation, QStringLiteral("kwordquiz/examples/example.kvtml"))));
    fileOpenRecent->addUrl(QUrl::fromLocalFile(QStandardPaths::locate(QStandardPaths::GenericDataLocation, QStringLiteral("kwordquiz/examples/french_verbs.kvtml"))));
    fileOpenRecent->addUrl(QUrl::fromLocalFile(QStandardPaths::locate(QStandardPaths::GenericDataLocation, QStringLiteral("kwordquiz/examples/fill_in_the_blank.kvtml"))));
    fileOpenRecent->addUrl(QUrl::fromLocalFile(QStandardPaths::locate(QStandardPaths::GenericDataLocation, QStringLiteral("kwordquiz/examples/us_states_and_capitals.kvtml"))));
    Prefs::setFirstRun(false);
  }
}

KWordQuizApp::~KWordQuizApp()
{
  disconnect(m_undoStack, &QUndoStack::undoTextChanged, this, &KWordQuizApp::slotUndoTextChanged);
  disconnect(m_undoStack, &QUndoStack::redoTextChanged, this, &KWordQuizApp::slotRedoTextChanged);
}

void KWordQuizApp::initActions()
{
  QAction* configToolbar;
  QAction* configNotifications;
  QAction* configApp;

  QAction * a;

  fileNew = KStandardAction::openNew(this, SLOT(slotFileNew()), actionCollection());
  fileNew->setWhatsThis(i18n("Creates a new blank vocabulary document"));
  fileNew->setToolTip(fileNew->whatsThis());
  fileNew->setStatusTip(fileNew->whatsThis());

  fileOpen = KStandardAction::open(this, SLOT(slotFileOpen()), actionCollection());
  fileOpen->setWhatsThis(i18n("Opens an existing vocabulary document"));
  fileOpen->setToolTip(fileOpen->whatsThis());
  fileOpen->setStatusTip(fileOpen->whatsThis());

  fileOpenRecent = KStandardAction::openRecent(this, SLOT(slotFileOpenRecent(QUrl)), actionCollection());

  fileGHNS = new QAction(QIcon::fromTheme(QStringLiteral("get-hot-new-stuff")), i18n("Download New Vocabularies..."), this);
  connect(fileGHNS, &QAction::triggered, this, &KWordQuizApp::slotFileGHNS);
  actionCollection()->addAction(QStringLiteral("file_ghns"), fileGHNS);
  actionCollection()->setDefaultShortcut(fileGHNS, QKeySequence(Qt::CTRL | Qt::Key_G));
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

  filePrintPreview = KStandardAction::printPreview(this, SLOT(slotFilePrintPreview()), actionCollection());
  filePrintPreview->setWhatsThis(i18n("Shows a preview of the active vocabulary document"));
  filePrintPreview->setToolTip(filePrintPreview->whatsThis());
  filePrintPreview->setStatusTip(filePrintPreview->whatsThis());

  fileQuit = KStandardAction::quit(this, SLOT(slotFileQuit()), actionCollection());
  fileQuit->setWhatsThis(i18n("Quits KWordQuiz"));
  fileQuit->setToolTip(fileQuit->whatsThis());
  fileQuit->setStatusTip(fileQuit->whatsThis());

  m_undoStack = new QUndoStack(this);
  editUndo = KUndoActions::createUndoAction(m_undoStack, actionCollection());
  editRedo = KUndoActions::createRedoAction(m_undoStack, actionCollection());
  connect(m_undoStack, &QUndoStack::cleanChanged, this, &KWordQuizApp::slotCleanChanged);
  connect(m_undoStack, &QUndoStack::undoTextChanged, this, &KWordQuizApp::slotUndoTextChanged);
  connect(m_undoStack, &QUndoStack::redoTextChanged, this, &KWordQuizApp::slotRedoTextChanged);

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
  actionCollection()->setDefaultShortcut(editClear, Qt::Key_Delete);
  editClear->setWhatsThis(i18n("Clears the content of the selected cells"));
  editClear->setToolTip(editClear->whatsThis());
  editClear->setStatusTip(editClear->whatsThis());

  editInsert = actionCollection()->addAction(QStringLiteral("edit_insert"));
  editInsert->setIcon(QIcon::fromTheme(QStringLiteral("insert-table-row")));
  editInsert->setText(i18n("&Insert Row"));
  actionCollection()->setDefaultShortcut(editInsert, QKeySequence(Qt::CTRL | Qt::Key_I));
  editInsert->setWhatsThis(i18n("Inserts a new row above the current row"));
  editInsert->setToolTip(editInsert->whatsThis());
  editInsert->setStatusTip(editInsert->whatsThis());
  connect(editInsert, &QAction::triggered, this, &KWordQuizApp::slotEditInsert);

  editDelete = actionCollection()->addAction(QStringLiteral("edit_delete"));
  editDelete->setIcon(QIcon::fromTheme(QStringLiteral("delete-table-row")));
  editDelete->setText(i18n("&Delete Row"));
  actionCollection()->setDefaultShortcut(editDelete, QKeySequence(Qt::CTRL | Qt::Key_K));
  editDelete->setWhatsThis(i18n("Deletes the selected row(s)"));
  editDelete->setToolTip(editDelete->whatsThis());
  editDelete->setStatusTip(editDelete->whatsThis());
  connect(editDelete, &QAction::triggered, this, &KWordQuizApp::slotEditDelete);

  editMarkBlank = actionCollection()->addAction(QStringLiteral("edit_mark_blank"));
  editMarkBlank->setIcon(QIcon::fromTheme(QStringLiteral("markasblank")));
  editMarkBlank->setText(i18n("&Mark as Blank"));
  actionCollection()->setDefaultShortcut(editMarkBlank, QKeySequence(Qt::CTRL | Qt::Key_M));
  editMarkBlank->setWhatsThis(i18n("Marks the current or selected word as a blank for Fill-in-the-blank"));
  editMarkBlank->setToolTip(editMarkBlank->whatsThis());
  editMarkBlank->setStatusTip(editMarkBlank->whatsThis());
  connect(editMarkBlank, &QAction::triggered, this, &KWordQuizApp::slotEditMarkBlank);

  editUnmarkBlank = actionCollection()->addAction(QStringLiteral("edit_unmark_blank"));
  editUnmarkBlank->setIcon(QIcon::fromTheme(QStringLiteral("unmarkasblank")));
  editUnmarkBlank->setText(i18n("&Unmark Blanks"));
  editUnmarkBlank->setWhatsThis(i18n("Removes blanks from the current or selected word"));
  editUnmarkBlank->setToolTip(editUnmarkBlank->whatsThis());
  editUnmarkBlank->setStatusTip(editUnmarkBlank->whatsThis());
  connect(editUnmarkBlank, &QAction::triggered, this, &KWordQuizApp::slotEditUnmarkBlank);

  vocabLanguages = actionCollection()->addAction(QStringLiteral("vocab_languages"));
  vocabLanguages->setIcon(QIcon::fromTheme(QStringLiteral("languages")));
  vocabLanguages->setText(i18n("&Column Settings..."));
  actionCollection()->setDefaultShortcut(vocabLanguages, QKeySequence(Qt::CTRL | Qt::Key_L));
  vocabLanguages->setWhatsThis(i18n("Defines the column settings for the active vocabulary"));
  vocabLanguages->setToolTip(vocabLanguages->whatsThis());
  vocabLanguages->setStatusTip(vocabLanguages->whatsThis());
  connect(vocabLanguages, &QAction::triggered, this, &KWordQuizApp::slotVocabLanguages);

  vocabFont = actionCollection()->addAction(QStringLiteral("vocab_font"));
  vocabFont->setIcon(QIcon::fromTheme(QStringLiteral("preferences-desktop-font")));
  vocabFont->setText(i18n("&Font..."));
  vocabFont->setWhatsThis(i18n("Defines the font used by the editor"));
  vocabFont->setToolTip(vocabFont->whatsThis());
  vocabFont->setStatusTip(vocabFont->whatsThis());
  connect(vocabFont, &QAction::triggered, this, &KWordQuizApp::slotVocabFont);

  a = actionCollection()->addAction(QStringLiteral("vocab_image"));
  a->setIcon(QIcon::fromTheme(QStringLiteral("image-x-generic")));
  a->setText(i18n("Link &Image..."));
  a->setWhatsThis(i18n("Links an image with the current entry"));
  a->setToolTip(a->whatsThis());
  a->setStatusTip(a->whatsThis());
  connect(a, &QAction::triggered, this, &KWordQuizApp::slotVocabImage);

  a = actionCollection()->addAction(QStringLiteral("vocab_sound"));
  a->setIcon(QIcon::fromTheme(QStringLiteral("audio-x-generic")));
  a->setText(i18n("Link &Sound..."));
  a->setWhatsThis(i18n("Links a sound with the current entry"));
  a->setToolTip(a->whatsThis());
  a->setStatusTip(a->whatsThis());
  connect(a, &QAction::triggered, this, &KWordQuizApp::slotVocabSound);

  vocabAdjustRows = actionCollection()->addAction(QStringLiteral("vocab_adjust_rows"));
  //vocabAdjustRows->setIcon(QIcon::fromTheme());
  vocabAdjustRows->setText(i18n("&Adjust Row Heights"));
  vocabAdjustRows->setWhatsThis(i18n("Automatically adjusts the height of selected rows"));
  vocabAdjustRows->setToolTip(vocabAdjustRows->whatsThis());
  vocabAdjustRows->setStatusTip(vocabAdjustRows->whatsThis());
  connect(vocabAdjustRows, &QAction::triggered, this, &KWordQuizApp::slotVocabAdjustRows);

  vocabShuffle = actionCollection()->addAction(QStringLiteral("vocab_shuffle"));
  vocabShuffle->setIcon(QIcon::fromTheme(QStringLiteral("shuffle")));
  vocabShuffle->setText(i18n("Sh&uffle"));
  vocabShuffle->setWhatsThis(i18n("Shuffles the entries of the active vocabulary"));
  vocabShuffle->setToolTip(vocabShuffle->whatsThis());
  vocabShuffle->setStatusTip(vocabShuffle->whatsThis());
  connect(vocabShuffle, &QAction::triggered, this, &KWordQuizApp::slotVocabShuffle);

  vocabLayouts = actionCollection()->addAction(QStringLiteral("vocab_layouts"));
  vocabLayouts->setText(i18n("&Keyboard Layout"));
  vocabLayouts->setWhatsThis(i18n("Shows available keyboard layouts"));
  vocabLayouts->setToolTip(vocabLayouts->whatsThis());
  vocabLayouts->setStatusTip(vocabLayouts->whatsThis());
  QMenu *m = new QMenu(this);
  vocabLayouts->setMenu(m);

  m_layoutActionGroup = new QActionGroup(this);
  connect(m_layoutActionGroup, &QActionGroup::triggered, this, &KWordQuizApp::slotLayoutActionGroupTriggered);

  m_modeActionMenu = actionCollection()->add<KActionMenu>(QStringLiteral("mode_0"));
  m_modeActionMenu->setIcon(QIcon::fromTheme(QStringLiteral("mode1")));
  m_modeActionMenu->setText(i18n("Change Mode"));
  m_modeActionMenu->setIconText(i18n("Mode"));
  m_modeActionMenu->setWhatsThis(i18n("Changes the mode used in quiz sessions"));
  m_modeActionMenu->setToolTip(m_modeActionMenu->whatsThis());
  m_modeActionMenu->setStatusTip(m_modeActionMenu->whatsThis());
  connect(m_modeActionMenu, &QAction::triggered, this, &KWordQuizApp::slotModeChange);

  m_modeActionGroup = new QActionGroup(this);
  connect(m_modeActionGroup, &QActionGroup::triggered, this, &KWordQuizApp::slotModeActionGroupTriggered);

  for (int i = 1; i <=5; ++i) {
    a = actionCollection()->addAction(QStringLiteral("mode_%1").arg(QString::number(i)));
    a->setData(i);
    a->setCheckable(true);
    a->setIcon(QIcon::fromTheme(QStringLiteral("mode%1").arg(QString::number(i))));
    a->setWhatsThis(i18n("Selects this mode"));
    a->setToolTip(a->whatsThis());
    a->setStatusTip(a->whatsThis());
    m_modeActionMenu->addAction(a);
    m_modeActionGroup->addAction(a);
  }

  quizEditor = actionCollection()->addAction(QStringLiteral("quiz_editor"));
  quizEditor->setIcon(QIcon::fromTheme(QStringLiteral("editor")));
  quizEditor->setText(i18nc("@item:inlistbox activate vocabulary editor", "&Editor"));
  actionCollection()->setDefaultShortcut(quizEditor, QKeySequence(Qt::Key_F6));
  quizEditor->setWhatsThis(i18n("Activates the vocabulary editor"));
  quizEditor->setToolTip(quizEditor->whatsThis());
  quizEditor->setStatusTip(quizEditor->whatsThis());
  connect(quizEditor, &QAction::triggered, this, &KWordQuizApp::slotQuizEditor);

  quizFlash = actionCollection()->addAction(QStringLiteral("quiz_flash"));
  quizFlash->setIcon(QIcon::fromTheme(QStringLiteral("flash")));
  quizFlash->setText(i18n("&Flashcard"));
  actionCollection()->setDefaultShortcut(quizFlash, QKeySequence(Qt::Key_F7));
  quizFlash->setWhatsThis(i18n("Starts a flashcard session using the active vocabulary"));
  quizFlash->setToolTip(quizFlash->whatsThis());
  quizFlash->setStatusTip(quizFlash->whatsThis());
  connect(quizFlash, &QAction::triggered, this, &KWordQuizApp::slotQuizFlash);

  quizMultiple = actionCollection()->addAction(QStringLiteral("quiz_multiple"));
  quizMultiple->setIcon(QIcon::fromTheme(QStringLiteral("multiple")));
  quizMultiple->setText(i18n("&Multiple Choice"));
  actionCollection()->setDefaultShortcut(quizMultiple, QKeySequence(Qt::Key_F8));
  quizMultiple->setWhatsThis(i18n("Starts a multiple choice session using the active vocabulary"));
  quizMultiple->setToolTip(quizMultiple->whatsThis());
  quizMultiple->setStatusTip(quizMultiple->whatsThis());
  connect(quizMultiple, &QAction::triggered, this, &KWordQuizApp::slotQuizMultiple);

  quizQA = actionCollection()->addAction(QStringLiteral("quiz_qa"));
  quizQA->setIcon(QIcon::fromTheme(QStringLiteral("qa")));
  quizQA->setText(i18n("&Question and Answer"));
  quizQA->setIconText(i18n("Q&&A"));
  actionCollection()->setDefaultShortcut(quizQA, QKeySequence(Qt::Key_F9));
  quizQA->setWhatsThis(i18n("Starts a question and answer session using the active vocabulary"));
  quizQA->setToolTip(quizQA->whatsThis());
  quizQA->setStatusTip(quizQA->whatsThis());
  connect(quizQA, &QAction::triggered, this, &KWordQuizApp::slotQuizQA);

  quizCheck = actionCollection()->addAction(QStringLiteral("quiz_check"));
  quizCheck->setIcon(QIcon::fromTheme(QStringLiteral("answer-correct")));
  quizCheck->setText(i18n("&Check"));
  actionCollection()->setDefaultShortcut(quizCheck, QKeySequence(Qt::Key_Return));
  quizCheck->setWhatsThis(i18n("Checks your answer to this question"));
  quizCheck->setToolTip(quizCheck->whatsThis());
  quizCheck->setStatusTip(quizCheck->whatsThis());

  quizOpt1 = actionCollection()->addAction(QStringLiteral("quiz_Opt1"));
  quizOpt1->setText(i18n("Choose Option &1"));
  actionCollection()->setDefaultShortcut(quizOpt1, QKeySequence(Qt::Key_1));

  quizOpt2 = actionCollection()->addAction(QStringLiteral("quiz_Opt2"));
  quizOpt2->setText(i18n("Choose Option &2"));
  actionCollection()->setDefaultShortcut(quizOpt2, QKeySequence(Qt::Key_2));

  quizOpt3 = actionCollection()->addAction(QStringLiteral("quiz_Opt3"));
  quizOpt3->setText(i18n("Choose Option &3"));
  actionCollection()->setDefaultShortcut(quizOpt3, QKeySequence(Qt::Key_3));

  flashKnow = actionCollection()->addAction(QStringLiteral("flash_know"));
  flashKnow->setIcon(QIcon::fromTheme(QStringLiteral("know")));
  flashKnow->setText(i18n("I &Know"));
  actionCollection()->setDefaultShortcut(flashKnow, QKeySequence(Qt::Key_K));
  flashKnow->setWhatsThis(i18n("Counts this card as correct and shows the next card"));
  flashKnow->setToolTip(flashKnow->whatsThis());
  flashKnow->setStatusTip(flashKnow->whatsThis());

  flashDontKnow = actionCollection()->addAction(QStringLiteral("flash_dont_know"));
  flashDontKnow->setIcon(QIcon::fromTheme(QStringLiteral("dontknow")));
  flashDontKnow->setText(i18n("I &Do Not Know"));
  actionCollection()->setDefaultShortcut(flashDontKnow, QKeySequence(Qt::Key_D));
  flashDontKnow->setWhatsThis(i18n("Counts this card as incorrect and shows the next card"));
  flashDontKnow->setToolTip(flashDontKnow->whatsThis());
  flashDontKnow->setStatusTip(flashDontKnow->whatsThis());

  qaHint = actionCollection()->addAction(QStringLiteral("qa_hint"));
  qaHint->setIcon(QIcon::fromTheme(QStringLiteral("hint")));
  qaHint->setText(i18n("&Hint"));
  actionCollection()->setDefaultShortcut(qaHint, QKeySequence(Qt::CTRL | Qt::Key_H));
  qaHint->setWhatsThis(i18n("Gets the next correct letter of the answer"));
  qaHint->setToolTip(qaHint->whatsThis());
  qaHint->setStatusTip(qaHint->whatsThis());

  qaMarkLastCorrect = actionCollection()->addAction(QStringLiteral("qa_mark_last_correct"));
  qaMarkLastCorrect->setIcon(QIcon::fromTheme(QStringLiteral("answer-correct")));
  qaMarkLastCorrect->setText(i18n("Mark Last Correct"));
  actionCollection()->setDefaultShortcut(qaMarkLastCorrect, QKeySequence(Qt::SHIFT | Qt::Key_Return));
  qaMarkLastCorrect->setWhatsThis(i18n("Marks last answer as correct"));
  qaMarkLastCorrect->setToolTip(qaMarkLastCorrect->whatsThis());
  qaMarkLastCorrect->setStatusTip(qaMarkLastCorrect->whatsThis());

  quizRestart = actionCollection()->addAction(QStringLiteral("quiz_restart"));
  quizRestart->setIcon(QIcon::fromTheme(QStringLiteral("start-over")));
  quizRestart->setText(i18n("&Restart"));
  actionCollection()->setDefaultShortcut(quizRestart, QKeySequence(Qt::CTRL | Qt::Key_R));
  quizRestart->setWhatsThis(i18n("Restarts the quiz session from the beginning"));
  quizRestart->setToolTip(quizRestart->whatsThis());
  quizRestart->setStatusTip(quizRestart->whatsThis());

  a = actionCollection()->addAction(QStringLiteral("quiz_audio_play"));
  a->setIcon(QIcon::fromTheme(QStringLiteral("media-playback-start")));
  a->setText(i18n("&Play Audio"));
  actionCollection()->setDefaultShortcut(a, QKeySequence(Qt::CTRL | Qt::Key_B));
  a->setWhatsThis(i18n("Play associated audio"));
  a->setToolTip(a->whatsThis());
  a->setStatusTip(a->whatsThis());

  quizRepeatErrors = actionCollection()->addAction(QStringLiteral("quiz_repeat_errors"));
  quizRepeatErrors->setIcon(QIcon::fromTheme(QStringLiteral("repeat")));
  quizRepeatErrors->setText(i18n("Repeat &Errors"));
  actionCollection()->setDefaultShortcut(quizRepeatErrors, QKeySequence(Qt::CTRL | Qt::Key_E));
  quizRepeatErrors->setWhatsThis(i18n("Repeats all incorrectly answered questions"));
  quizRepeatErrors->setToolTip(quizRepeatErrors->whatsThis());
  quizRepeatErrors->setStatusTip(quizRepeatErrors->whatsThis());

  quizExportErrors = actionCollection()->addAction(QStringLiteral("quiz_export_errors"));
  quizExportErrors->setIcon(KStandardGuiItem::saveAs().icon());
  quizExportErrors->setText(i18n("Export Errors &As..."));
  quizExportErrors->setWhatsThis(i18n("Exports all errors as a new vocabulary document"));
  quizExportErrors->setToolTip(quizExportErrors->whatsThis());
  quizExportErrors->setStatusTip(quizExportErrors->whatsThis());
  connect(quizExportErrors, &QAction::triggered, this, &KWordQuizApp::slotCreateErrorListDocument);

  configShowSearchBar = actionCollection()->add<KToggleAction>(QStringLiteral("config_show_search"));
  configShowSearchBar->setText(i18n("Show Se&arch"));
  connect(configShowSearchBar, &QAction::triggered, this, &KWordQuizApp::slotConfigShowSearch);
  configShowSearchBar->setWhatsThis(i18n("Toggle display of the search bar"));
  configShowSearchBar->setToolTip(configShowSearchBar->whatsThis());
  configShowSearchBar->setStatusTip(configShowSearchBar->whatsThis());

  configNotifications = KStandardAction::configureNotifications(this, SLOT(slotConfigureNotifications()), actionCollection());
  configNotifications->setWhatsThis(i18n("Configures sound and other notifications for certain events"));
  configNotifications->setToolTip(configNotifications->whatsThis());
  configNotifications->setStatusTip(configNotifications->whatsThis());

  configApp = KStandardAction::preferences(this, SLOT(slotConfigure()), actionCollection());
  configApp->setWhatsThis(i18n("Specifies preferences for the vocabulary editor and quiz sessions"));
  configApp->setToolTip(configApp->whatsThis());
  configApp->setStatusTip(configApp->whatsThis());

  for (int i = 1; i <=9; ++i) {
    a = actionCollection()->addAction(QStringLiteral("char_%1").arg(QString::number(i)));
    a->setText(i18n("Special Character <numid>%1</numid>", i));
    actionCollection()->setDefaultShortcut(a, QKeySequence(QStringLiteral("Ctrl+%1").arg(QString::number(i))));
    connect(a, &QAction::triggered, this, [this, i] () { slotInsertChar(i); });
  }

  updateSpecialCharIcons();

  resize( QSize(650, 450).expandedTo(minimumSizeHint()));
  setupGUI(ToolBar | Keys | StatusBar | Create);
  setAutoSaveSettings();

  configToolbar = actionCollection()->action(QStringLiteral("options_configure_toolbars"));
  configToolbar->setWhatsThis(i18n("Toggles display of the toolbars"));
  configToolbar->setToolTip(configToolbar->whatsThis());
  configToolbar->setStatusTip(configToolbar->whatsThis());
}

void KWordQuizApp::initStatusBar()
{
  m_statusLabel = new QLabel(statusBar());
  statusBar()->insertPermanentWidget(0, m_statusLabel);
  //statusBar()->setItemFixed(1, 250);
}

void KWordQuizApp::initDocument()
{
  m_doc = new KEduVocDocument(this);
  m_doc->setGenerator(QStringLiteral("kwordquiz %1").arg(KWORDQUIZ_VERSION_STRING));
  ///@todo New kvtml documents have a locale setting which should contain the language they are written in.
  /* it would be nice to support that somehow. get hot new stuff for KHangman and KAnagram already uses this. */
  m_doc->appendIdentifier();
  m_doc->identifier(0).setName(i18n("Column 1"));
  // m_doc->identifier(0).setLocale("en");
  m_doc->appendIdentifier();
  m_doc->identifier(1).setName(i18n("Column 2"));
  // m_doc->identifier(1).setLocale("en");

  for (int i=0; i<20; i++)
  {
    m_doc->lesson()->appendEntry(new KEduVocExpression());
  }
}

void KWordQuizApp::initModel()
{
  m_tableModel = new KWQTableModel(this);
  m_tableModel->setDocument(m_doc);
  m_tableModel->setHeaderData(0, Qt::Horizontal, QSize(250, 25), Qt::SizeHintRole);
  m_tableModel->setHeaderData(1, Qt::Horizontal, QSize(250, 25), Qt::SizeHintRole);
  m_sortFilterModel = new KWQSortFilterModel(this);
  m_sortFilterModel->setTableModel(m_tableModel);
}

void KWordQuizApp::initView()
{
  m_pageWidget = new KPageWidget(this);
  m_pageWidget->setFaceType( KPageView::List );
  setCentralWidget(m_pageWidget);
  connect(m_pageWidget, SIGNAL(currentPageChanged(KPageWidgetItem*,KPageWidgetItem*)), this, SLOT(slotCurrentPageChanged(KPageWidgetItem*,KPageWidgetItem*)));

  QVBoxLayout *editorLayout = new QVBoxLayout();
  editorLayout->setContentsMargins(0, 0, 0, 0);

  m_searchLine = new FilterProxySearchLine(this);
  m_searchLine->setFocusPolicy(Qt::ClickFocus);
  m_searchLine->setPlaceholderText(i18n("Enter search terms here"));
  m_searchLine->setFilterProxyModel(m_sortFilterModel);

  m_tableView = new KWQTableView(m_undoStack, this);
  editorLayout->addWidget(m_searchLine);
  editorLayout->addWidget(m_tableView);
  m_tableView->setFilterModel(m_sortFilterModel);
  m_tableView->setColumnWidth(0, qvariant_cast<QSize>(m_tableModel->headerData(0, Qt::Horizontal, Qt::SizeHintRole)).width());
  m_tableView->setColumnWidth(1, qvariant_cast<QSize>(m_tableModel->headerData(1, Qt::Horizontal, Qt::SizeHintRole)).width());
  setWindowTitle(m_doc->url().fileName() + "[*]");
  setWindowModified(false);
  m_tableView->setContextMenuPolicy(Qt::CustomContextMenu);
  connect(m_tableView, &QWidget::customContextMenuRequested, this, &KWordQuizApp::slotTableContextMenuRequested);
  connect(m_tableModel, &QAbstractItemModel::modelReset, m_tableView, &KWQTableView::slotModelReset);

  m_searchLine->setVisible(Prefs::showSearch());
  configShowSearchBar->setChecked(Prefs::showSearch());

  m_editorView = new QWidget(this);
  m_flashView = new FlashView(this, actionCollection());
  m_multipleView = new MultipleView(this, actionCollection());
  m_qaView = new QAView(this, actionCollection());

  m_editorPage = m_pageWidget->addPage(m_editorView, i18nc("@item:inlistbox vocabulary editor", "Editor"));
  m_editorPage->setIcon(QIcon::fromTheme(QStringLiteral("editor")));
  m_editorPage->setHeader(QLatin1String(""));
  m_editorPage->widget()->setLayout(editorLayout);

  m_flashPage = m_pageWidget->addPage(m_flashView, i18n("Flashcard"));
  m_flashPage->setIcon(QIcon::fromTheme(QStringLiteral("flash")));
  m_flashPage->setHeader(QLatin1String(""));

  m_multiplePage = m_pageWidget->addPage(m_multipleView, i18n("Multiple Choice"));
  m_multiplePage->setIcon(QIcon::fromTheme(QStringLiteral("multiple")));
  m_multiplePage->setHeader(QLatin1String(""));

  m_qaPage = m_pageWidget->addPage(m_qaView, i18n("Question & Answer"));
  m_qaPage->setIcon(QIcon::fromTheme(QStringLiteral("qa")));
  m_qaPage->setHeader(QLatin1String(""));

  m_pageWidget->addAction(quizEditor);
  m_pageWidget->addAction(quizFlash);
  m_pageWidget->addAction(quizMultiple);
  m_pageWidget->addAction(quizQA);

  const QList<QObject*> listOfChildren = m_pageWidget->children();

  foreach (QObject *object, listOfChildren) {
    if(!object->isWidgetType())
      continue;

    QWidget *childWidget = static_cast<QWidget *>(object);

    if (childWidget->inherits("QListView"))
      childWidget->setFocusPolicy(Qt::NoFocus);
  }

  m_pageWidget->setCurrentPage(m_editorPage);
  m_tableView->setFocus();
  updateActions();
}

void KWordQuizApp::openUrl(const QUrl &url)
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
          if(a->document()->url().path() == url.path())
          {
            if (w->isMinimized())
              KWindowSystem::unminimizeWindow(w->winId());
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

void KWordQuizApp::openDocumentFile(const QUrl &url)
{
  slotStatusMsg(i18n("Opening file..."));
  if (!url.isEmpty()) {
    m_tableModel->beginResetModel();
    int result = m_doc->open(url);
    if (result == KEduVocDocument::NoError) {
      while (m_doc->identifierCount() < 2) { //if we opened a TAB-less CSV, there
        m_doc->appendIdentifier(); //may be 0 or 1 identifiers, we need at least 2
      }
      m_tableModel->endResetModel();

      m_dirWatch->addFile(url.path());
      setWindowTitle(m_doc->url().fileName() + "[*]");
      setWindowModified(false);
      fileOpenRecent->addUrl(url);
      slotModeActionGroupTriggered(m_modeActionGroup->checkedAction());
      m_undoStack->clear();
      switch (Prefs::startSession()) {
        case Prefs::EnumStartSession::Flashcard:
          slotQuizFlash();
          break;
        case Prefs::EnumStartSession::MultipleChoice:
          slotQuizMultiple();
          break;
        case Prefs::EnumStartSession::QA:
          slotQuizQA();
          break;
        default:
          slotQuizEditor();
          break;
      }
    }
    else {
      KMessageBox::error(this, KEduVocDocument::errorDescription(result));
      m_tableModel->endResetModel();
    }
  }
  slotStatusMsg(i18nc("@info:status ready", "Ready"));
}

KEduVocDocument *KWordQuizApp::document() const
{
  return m_doc;
}

void KWordQuizApp::saveOptions()
{
  fileOpenRecent->saveEntries(KSharedConfig::openConfig()->group( "Recent Files") );
  Prefs::self()->save();
}

void KWordQuizApp::readOptions()
{
  fileOpenRecent->loadEntries(KSharedConfig::openConfig()->group( "Recent Files") );
}

void KWordQuizApp::saveProperties(KConfigGroup &_cfg)
{
  if(!m_doc->url().fileName().isEmpty() && !m_doc->isModified())
  {
    // saving to tempfile not necessary
  }
  else
  {
    QUrl url = m_doc->url();
    _cfg.writeEntry("filename", url.url());
    _cfg.writeEntry("modified", m_doc->isModified());
    QString tempname = QTemporaryFile().fileName();
    QString tempurl = QUrl::toPercentEncoding(tempname);
    QUrl _url(tempurl);
    m_doc->saveAs(_url, KEduVocDocument::Automatic);
  }
}

void KWordQuizApp::readProperties(const KConfigGroup &_cfg)
{
  QString filename = _cfg.readEntry("filename", "");
  QUrl url(filename);
  bool modified = _cfg.readEntry("modified", false);
  if(modified)
  {
    QTemporaryFile tmpfile(filename);
    if (tmpfile.open()) {
        QUrl _url(QUrl::fromLocalFile(tmpfile.fileName()));
        m_doc->open(_url);
        m_doc->setModified();
        setWindowTitle(_url.fileName() + "[*]");
        setWindowModified(true);
    }
  }
  else
  {
    if(!filename.isEmpty())
    {
      m_doc->open(url);
      setWindowTitle(url.fileName() + "[*]");
      setWindowModified(false);
    }
  }
}

bool KWordQuizApp::queryClose()
{
  bool completed=true;
  if(m_doc->isModified())
  {
    int want_save = KMessageBox::warningYesNoCancel(this, i18n("The current document has been modified.\nDo you want to save it?"), QString(), KStandardGuiItem::save(), KStandardGuiItem::discard());
    switch(want_save)
    {
      case KMessageBox::Cancel:
        completed = false;
        break;

      case KMessageBox::Yes:
        if (m_doc->url().fileName().isEmpty())
        {
          completed = saveDocAsFileName(m_doc);
        }
        else
        {
          completed = (m_doc->saveAs(m_doc->url(), KEduVocDocument::Automatic) == KEduVocDocument::NoError);
        }

        break;

      case KMessageBox::No:
        completed = true;
        break;

      default:
        completed = false;
        break;
    }
  }

  if (completed)
    if (m_dirWatch->contains(m_doc->url().toLocalFile()))
      m_dirWatch->removeFile(m_doc->url().toLocalFile());
  saveOptions();
  // Close the current document to get rid of its lock file
  m_doc->close();
  return completed;
}


void KWordQuizApp::slotFileNew()
{
  slotStatusMsg(i18n("Opening a new document window..."));
  if (!m_tableModel->isEmpty()){
    KWordQuizApp *new_window= new KWordQuizApp();
    new_window->show();
  }
  slotStatusMsg(i18nc("@info:status ready", "Ready"));
}

void KWordQuizApp::slotFileOpen()
{
  slotStatusMsg(i18n("Opening file..."));

  QCheckBox * cb = new QCheckBox(i18n("&Merge selected files with the current document"), 0);
  cb -> setChecked(false);
  cb -> setEnabled(false);

  QString filter = KEduVocDocument::pattern(KEduVocDocument::Reading);
  QPointer<QFileDialog> fd = new QFileDialog(this, i18n("Open Vocabulary Document"), QString(), filter);
  fd->setFileMode(QFileDialog::ExistingFiles);

  if (fd->exec() == QDialog::Accepted)
  {
    QList<QUrl> l = fd->selectedUrls();
    bool append = ((cb->isChecked()) && (l.count() >= 1));

    if (append)
    {
      QList<QUrl>::iterator it;
      for(it = l.begin(); it != l.end(); ++it)
      {
        KEduVocDocument *new_doc = new KEduVocDocument(this);
        new_doc->open(*it);

        m_doc->merge(new_doc, false);
        delete (new_doc);
      }
      //m_tableModel->reset();
    }
    else
    {
      QList<QUrl>::iterator it;
      for(it = l.begin(); it != l.end(); ++it)
      {
        openUrl(*it);
      }
    }
  }

  delete (fd); //deletes cb also

  slotStatusMsg(i18nc("@info:status ready", "Ready"));
}

void KWordQuizApp::slotFileOpenRecent(const QUrl &url)
{
  slotStatusMsg(i18n("Opening file..."));
  fileOpenRecent->setCurrentItem(-1);
  openUrl(url);
  slotStatusMsg(i18nc("@info:status ready", "Ready"));
}


void KWordQuizApp::slotFileGHNS()
{
#if KNEWSTUFF_VERSION < QT_VERSION_CHECK(5, 78, 0)
  QPointer<KNS3::DownloadDialog> getHotNewStuffDialog = new KNS3::DownloadDialog(QStringLiteral("kwordquiz.knsrc"), this);
  getHotNewStuffDialog->exec();
  KNS3::Entry::List entries = getHotNewStuffDialog->changedEntries();
#else
  KNS3::QtQuickDialogWrapper dialog(QStringLiteral("kwordquiz.knsrc"));
  const QList<KNSCore::EntryInternal> entries = dialog.exec();
#endif
  QMimeDatabase db;

  // list of changed entries
  foreach(const auto &entry, entries) {
    // check mime type and if kvtml, open it
    foreach(const QString &file, entry.installedFiles()) {
      QMimeType mimeType = db.mimeTypeForFile(file);
      if (mimeType.name() == QLatin1String("application/x-kvtml")) {
	    KProcess::startDetached(QStringLiteral("kwordquiz"), QStringList() << file);
      }
    }
  }
}


void KWordQuizApp::slotFileSave()
{
  bool success = false;
  slotStatusMsg(i18n("Saving file..."));
  if (m_doc->url().fileName().isEmpty()) {
    success = saveDocAsFileName(m_doc);
  }
  else {
    int saveStatus = m_doc->saveAs(m_doc->url(), KEduVocDocument::Automatic);
    if (saveStatus == KEduVocDocument::NoError)
      success = true;
    else
      success = saveDocAsFileName(m_doc);
  }
  if (success)
    m_undoStack->setClean(); //emits cleanChanged()
  slotStatusMsg(i18nc("@info:status ready", "Ready"));
}


void KWordQuizApp::slotFileSaveAs()
{
  slotStatusMsg(i18n("Saving file with a new filename..."));
  bool success = saveDocAsFileName(m_doc);
  if (success) {
    m_undoStack->setClean(); //emits cleanChanged()
    setWindowTitle(m_doc->url().fileName() + "[*]"); //if clean to begin with
    setWindowModified(m_doc->isModified());
  }
  slotStatusMsg(i18nc("@info:status ready", "Ready"));
}


bool KWordQuizApp::saveDocAsFileName(KEduVocDocument *document)
{
  bool success = false;
  int result = KEduVocDocument::Unknown;

  QString filter = KEduVocDocument::pattern(KEduVocDocument::Writing);
  filter.append(QStringLiteral(";;"));
  filter.append(i18n("HTML Document") + QStringLiteral(" (*.html)"));

  QPointer<QFileDialog> fd = new QFileDialog(this, i18n("Save Vocabulary Document As"), QString(), filter);
  fd->setAcceptMode(QFileDialog::AcceptSave);

  if (fd->exec() == QDialog::Accepted) {
    QUrl url = fd->selectedUrls().at(0);
    if (!url.isEmpty()) {
      QString chosenFilter = fd->selectedNameFilter();
      if (!url.fileName().contains('.')) {
        qDebug() << "Selected name filter is " << chosenFilter;
        if (chosenFilter == QLatin1String("*.csv"))
          url = QUrl(url.path().append(".csv"));
        else if (chosenFilter == QLatin1String("*.kvtml"))
          url = QUrl(url.path().append(".kvtml"));
        else if (chosenFilter == QLatin1String("*.html"))
          url = QUrl(url.path().append(".html"));
      }

      if (chosenFilter == QLatin1String("*.html")) {
        if (m_tableView->doHtmlExport(url))
          result = KEduVocDocument::NoError;
        else
          result = KEduVocDocument::FileCannotWrite;
        success = false; //export only, do not consider really saved
      }
      else {
        if (m_dirWatch->contains(document->url().toLocalFile()))
          m_dirWatch->removeFile(document->url().toLocalFile());
        result = document->saveAs(url, KEduVocDocument::Automatic);
        if (result == KEduVocDocument::NoError) {
          m_dirWatch->addFile(url.path());
          fileOpenRecent->addUrl(url);
          success = true;
        }
      }

      if (result != KEduVocDocument::NoError) {
        KMessageBox::error(this, KEduVocDocument::errorDescription(result));
        success = false;
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
      setWindowTitle(m_doc->url().fileName() + "[*]");
      setWindowModified(m_doc->isModified());
      m_tableModel->setDocument(m_doc);
      slotQuizEditor();
      slotModeActionGroupTriggered(m_modeActionGroup->actions().at(Prefs::mode() - 1));
      m_tableView ->selectionModel()->setCurrentIndex(m_sortFilterModel->index(0, 0), QItemSelectionModel::SelectCurrent);
      m_undoStack->clear();
    }

  slotStatusMsg(i18nc("@info:status ready", "Ready"));
}

void KWordQuizApp::slotFilePrint()
{
  slotStatusMsg(i18n("Printing..."));
  m_tableView->doPrint();
  slotStatusMsg(i18nc("@info:status ready", "Ready"));
}

void KWordQuizApp::slotFilePrintPreview() {
  slotStatusMsg(i18n("Showing Preview..."));
  m_tableView->doPrintPreview();
  slotStatusMsg(i18nc("@info:status ready", "Ready"));
}


void KWordQuizApp::slotFileQuit()
{
  slotStatusMsg(i18nc("@info:status quitting", "Quitting..."));
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
      // Close the corresponding document to remove its lock file
      static_cast<KWordQuizApp *>(w)->document()->close();
    }
  }
}

void KWordQuizApp::slotEditCut()
{
  slotStatusMsg(i18n("Cutting selection..."));
  m_tableView->doEditCut();
  slotStatusMsg(i18nc("@info:status ready", "Ready"));
}

void KWordQuizApp::slotEditCopy()
{
  slotStatusMsg(i18n("Copying selection to clipboard..."));
  m_tableView->doEditCopy();
  slotStatusMsg(i18nc("@info:status ready", "Ready"));
}

void KWordQuizApp::slotEditPaste()
{
  slotStatusMsg(i18n("Inserting clipboard contents..."));
  m_tableView->doEditPaste();
  slotStatusMsg(i18nc("@info:status ready", "Ready"));
}

void KWordQuizApp::slotEditClear()
{
  slotStatusMsg(i18n("Clearing the selected cells..."));
  m_tableView->doEditClear();
  slotStatusMsg(i18nc("@info:status ready", "Ready"));
}

void KWordQuizApp::slotEditInsert()
{
  slotStatusMsg(i18n("Inserting rows..."));
  m_tableView->doEditInsert();
  slotStatusMsg(i18nc("@info:status ready", "Ready"));
}

void KWordQuizApp::slotEditDelete()
{
  slotStatusMsg(i18n("Deleting selected rows..."));
  m_tableView->doEditDelete();
  slotStatusMsg(i18nc("@info:status ready", "Ready"));
}

void KWordQuizApp::slotEditMarkBlank()
{
  slotStatusMsg(i18n("Marking selected text as a blank..."));
  m_tableView->doEditMarkBlank();
  slotStatusMsg(i18nc("@info:status ready", "Ready"));
}

void KWordQuizApp::slotEditUnmarkBlank()
{
  slotStatusMsg(i18n("Removing blank markings..."));
  m_tableView->doEditUnmarkBlank();
  slotStatusMsg(i18nc("@info:status ready", "Ready"));
}

void KWordQuizApp::slotVocabLanguages()
{
  slotStatusMsg(i18n("Setting the column titles of the vocabulary..."));
  DlgLanguage* dlg;
  dlg = new DlgLanguage(m_tableModel, this);

  if (dlg->exec() == QDialog::Accepted) {
    KWQCommandIdentifiers *kwqc = new KWQCommandIdentifiers(m_tableView, dlg->columnData());
    m_undoStack->push(kwqc);
  }
  slotStatusMsg(i18nc("@info:status ready", "Ready"));
}

void KWordQuizApp::slotVocabFont()
{
  slotStatusMsg(i18n("Setting the font of the vocabulary..."));
  QPointer<QFontDialog> dlg = new QFontDialog(this);
  dlg->setObjectName(QStringLiteral("dlg_font"));
  dlg->setFont(Prefs::editorFont());
  if (dlg->exec() == QDialog::Accepted)
  {
    KWQCommandFont *kwqc = new KWQCommandFont(m_tableView, Prefs::editorFont(), dlg->font());
    m_undoStack->push(kwqc);
  }
  delete dlg;
  slotStatusMsg(i18nc("@info:status ready", "Ready"));
}


void KWordQuizApp::slotVocabImage()
{
  slotStatusMsg(i18n("Linking an image with the current entry..."));
  m_tableView->doVocabImage();
  slotStatusMsg(i18nc("@info:status ready", "Ready"));
}


void KWordQuizApp::slotVocabSound()
{
  slotStatusMsg(i18n("Linking a sound with the current entry..."));
  m_tableView->doVocabSound();
  slotStatusMsg(i18nc("@info:status ready", "Ready"));
}


void KWordQuizApp::slotVocabAdjustRows()
{
  slotStatusMsg(i18n("Adjusting row heights..."));
  foreach(const QModelIndex &index, m_tableView->selectionModel()->selectedIndexes())
    m_tableView->adjustRow(index.row());
  slotStatusMsg(i18nc("@info:status ready", "Ready"));
}


void KWordQuizApp::slotVocabShuffle()
{
  slotStatusMsg(i18n("Randomizing the vocabulary..."));
  m_tableView->doVocabShuffle();
  slotStatusMsg(i18nc("@info:status ready", "Ready"));
}

void KWordQuizApp::slotModeChange()
{
  QString newMode;
  slotStatusMsg(i18n("Updating mode..."));
  if (Prefs::mode() < 5)
    newMode = QStringLiteral("mode_%1").arg(QString::number(Prefs::mode() + 1));
  else
    newMode = QStringLiteral("mode_1");

  QAction *a = actionCollection()->action(newMode);
  a->activate(QAction::Trigger);
  slotStatusMsg(i18nc("@info:status ready", "Ready"));
}


void KWordQuizApp::slotQuizEditor()
{
  slotStatusMsg(i18n("Starting editor session..."));
  m_pageWidget->setCurrentPage(m_editorPage);
  slotStatusMsg(i18nc("@info:status ready", "Ready"));
}

void KWordQuizApp::slotQuizFlash()
{
  slotStatusMsg(i18n("Starting flashcard session..."));
  m_pageWidget->setCurrentPage(m_flashPage);
  slotStatusMsg(i18nc("@info:status ready", "Ready"));
}

void KWordQuizApp::slotQuizMultiple()
{
  slotStatusMsg(i18n("Starting multiple choice session..."));
  m_pageWidget->setCurrentPage(m_multiplePage);
  slotStatusMsg(i18nc("@info:status ready", "Ready"));
}

void KWordQuizApp::slotQuizQA()
{
  slotStatusMsg(i18n("Starting question & answer session..."));
  m_pageWidget->setCurrentPage(m_qaPage);
  slotStatusMsg(i18nc("@info:status ready", "Ready"));
}

void KWordQuizApp::slotCurrentPageChanged(KPageWidgetItem *current, KPageWidgetItem *before)
{
  Q_UNUSED(before);
  delete(m_quiz);
  m_quiz = 0;
  disconnect(quizCheck, 0, 0, 0);
  disconnect(flashKnow, 0, 0, 0);
  disconnect(flashDontKnow, 0, 0, 0);
  disconnect(quizRestart, 0, 0, 0);
  disconnect(actionCollection()->action(QStringLiteral("quiz_audio_play")), 0, 0, 0);
  disconnect(quizRepeatErrors, 0, 0, 0);
  disconnect(qaHint, 0, 0, 0);
  disconnect(qaMarkLastCorrect, 0, 0, 0);

  if (current == m_editorPage) {
    m_tableView->setFocus();
    m_searchLine->setVisible(Prefs::showSearch());
  }

  else  if (current == m_flashPage) {
    if (Prefs::enableBlanks()) {
      if (!m_tableModel->checkSyntax())
        m_pageWidget->setCurrentPage(m_editorPage);
    }
    m_quiz = new KWQQuizModel(this);
    m_quiz->setFilterModel(m_sortFilterModel);
    connect(m_quiz, &KWQQuizModel::checkingAnswer, m_tableView, &KWQTableView::slotCheckedAnswer);
    m_quiz->setQuizType(Prefs::EnumStartSession::Flashcard);
    m_quiz->setQuizMode(Prefs::mode());
    if (m_quiz->init())
    {
      connect(quizCheck, SIGNAL(triggered(bool)), m_flashView, SLOT(slotCheck()));
      connect(flashKnow, &QAction::triggered, m_flashView, &FlashView::slotKnow);
      connect(flashDontKnow, &QAction::triggered, m_flashView, &FlashView::slotDontKnow);
      connect(actionCollection()->action(QStringLiteral("quiz_audio_play")), &QAction::triggered, m_flashView, &KWQQuizView::slotAudioPlay);
      connect(quizRestart, &QAction::triggered, m_flashView, &KWQQuizView::slotRestart);
      connect(quizRepeatErrors, &QAction::triggered, m_flashView, &KWQQuizView::slotRepeat);
      connect(this, SIGNAL(settingsChanged()), m_flashView, SLOT(slotApplySettings()));

      m_flashView->setQuiz(m_quiz);
      m_flashView->init();
      m_flashView->stackedWidget->setCurrentIndex(0);
    }
    else
    {
      delete(m_quiz);
      m_quiz = 0;
      m_flashView->stackedWidget->setCurrentIndex(1);
    }
  }

  else if (current == m_multiplePage) {
    if (Prefs::enableBlanks()) {
      if (!m_tableModel->checkSyntax())
        m_pageWidget->setCurrentPage(m_editorPage);
    }
    m_quiz = new KWQQuizModel(this);
    m_quiz->setFilterModel(m_sortFilterModel);
    connect(m_quiz, &KWQQuizModel::checkingAnswer, m_tableView, &KWQTableView::slotCheckedAnswer);
    m_quiz->setQuizType(Prefs::EnumStartSession::MultipleChoice);
    m_quiz->setQuizMode(Prefs::mode());
    if (m_quiz->init())
    {
      connect(quizCheck, SIGNAL(triggered(bool)), m_multipleView, SLOT(slotCheck()));
      connect(quizRestart, &QAction::triggered, m_multipleView, &KWQQuizView::slotRestart);
      connect(quizRepeatErrors, &QAction::triggered, m_multipleView, &KWQQuizView::slotRepeat);
      connect(this, SIGNAL(settingsChanged()), m_multipleView, SLOT(slotApplySettings()));

      m_multipleView->setQuiz(m_quiz);
      m_multipleView->init();
      m_multipleView->stackedWidget->setCurrentIndex(0);
    }
    else
    {
      delete(m_quiz);
      m_quiz = 0;
      m_multipleView->stackedWidget->setCurrentIndex(1);
    }
  }

  else if (current == m_qaPage) {
    if (Prefs::enableBlanks()) {
      if (!m_tableModel->checkSyntax())
        m_pageWidget->setCurrentPage(m_editorPage);
    }
    m_quiz = new KWQQuizModel(this);
    m_quiz->setFilterModel(m_sortFilterModel);
    connect(m_quiz, &KWQQuizModel::checkingAnswer, m_tableView, &KWQTableView::slotCheckedAnswer);
    m_quiz->setQuizType(Prefs::EnumStartSession::QA);
    m_quiz->setQuizMode(Prefs::mode());
    if (m_quiz->init())
    {
      connect(quizCheck, SIGNAL(triggered(bool)), m_qaView, SLOT(slotCheck()));
      connect(qaHint, &QAction::triggered, m_qaView, &QAView::slotHint);
      connect(qaMarkLastCorrect, &QAction::triggered, m_qaView, &QAView::slotMarkLastCorrect);
      connect(quizRestart, &QAction::triggered, m_qaView, &KWQQuizView::slotRestart);
      connect(actionCollection()->action(QStringLiteral("quiz_audio_play")), &QAction::triggered, m_qaView, &KWQQuizView::slotAudioPlay);
      connect(quizRepeatErrors, &QAction::triggered, m_qaView, &KWQQuizView::slotRepeat);
      connect(this, SIGNAL(settingsChanged()), m_qaView, SLOT(slotApplySettings()));

      m_qaView->setQuiz(m_quiz);
      m_qaView->init();
      m_qaView->stackedWidget->setCurrentIndex(0);
    }
    else
    {
      delete(m_quiz);
      m_quiz = 0;
      m_qaView->stackedWidget->setCurrentIndex(1);
    }
  }

  updateActions();
}

/** Configure notifications */
void KWordQuizApp::slotConfigureNotifications( )
{
  KNotifyConfigWidget::configure(this);
}

/** Configure kwordquiz */
void KWordQuizApp::slotConfigure()
{
  if (KWordQuizPrefs::showDialog(QStringLiteral("settings")))
    return;

  //KConfigDialog didn't find an instance of this dialog, so lets create it :
  KWordQuizPrefs* dialog = new KWordQuizPrefs(this, QStringLiteral("settings"),  Prefs::self(), actionCollection());
  connect(dialog, &KConfigDialog::settingsChanged, this, &KWordQuizApp::slotApplyPreferences);
  dialog->show();
}

void KWordQuizApp::slotApplyPreferences()
{
  editMarkBlank->setEnabled(Prefs::enableBlanks());
  editUnmarkBlank->setEnabled(Prefs::enableBlanks());
  m_tableView->reset();
  updateSpecialCharIcons();
  emit settingsChanged();
}

void KWordQuizApp::updateSpecialCharIcons()
{
  for (int i = 0; i < 9; i++){
    QAction * a = actionCollection()->action(QString("char_" + QString::number(i + 1)));
    a->setIcon(charIcon(Prefs::specialCharacters()[i]));
    a->setIconText(i18n("Insert %1", Prefs::specialCharacters()[i]));
    a->setWhatsThis(i18n("Inserts the character %1", Prefs::specialCharacters()[i]));
    a->setToolTip(a->whatsThis());
    a->setStatusTip(a->whatsThis());
  }
}

QIcon KWordQuizApp::charIcon(const QChar & c)
{
  QRect r(4, 4, 120, 120);

  ///A font to draw the character with
  QFont font(QStringLiteral("sans"));
  font.setPixelSize(100);
  font.setWeight(QFont::Bold);

  ///Create the pixmap
  QPixmap pm(128, 128);
  pm.fill(Qt::white);
  QPainter p;
  p.begin(&pm);
  p.setFont(font);
  p.setPen(Qt::blue);
  p.drawText(r, Qt::AlignCenter, (QString) c);
  p.end();

  ///Create transparency mask
  QBitmap bm(128, 128);
  bm.fill(Qt::color0);
  QPainter b;
  b.begin(&bm);
  b.setFont(font);
  b.setPen(Qt::color1);
  b.drawText(r, Qt::AlignCenter, (QString) c);
  b.end();

  ///Mask the pixmap
  pm.setMask(bm);

  return QIcon(pm);
}

void KWordQuizApp::slotStatusMsg(const QString &text)
{
  //statusBar()->clearMessage();
  statusBar()->showMessage(text);
}


void KWordQuizApp::slotModeActionGroupTriggered(QAction *act)
{
  if (m_quiz != 0)
    if (KMessageBox::warningContinueCancel(this, i18n("This will restart your quiz. Do you wish to continue?"), QString(), KStandardGuiItem::cont(), KStandardGuiItem::cancel(), QStringLiteral("askModeQuiz"))
      != KMessageBox::Continue)
      {
        m_modeActionGroup->actions()[0]->setChecked(Prefs::mode() == 1);
        m_modeActionGroup->actions()[1]->setChecked(Prefs::mode() == 2);
        m_modeActionGroup->actions()[2]->setChecked(Prefs::mode() == 3);
        m_modeActionGroup->actions()[3]->setChecked(Prefs::mode() == 4);
        m_modeActionGroup->actions()[4]->setChecked(Prefs::mode() == 5);
        return;
      }

  act->setChecked(true);

  QString s1 = m_tableModel->headerData(0, Qt::Horizontal, Qt::DisplayRole).toString();
  QString s2 = m_tableModel->headerData(1, Qt::Horizontal, Qt::DisplayRole).toString();

  m_modeActionGroup->actions()[0]->setText(i18n("&1 %1 -> %2 In Order", s1, s2));
  m_modeActionGroup->actions()[1]->setText(i18n("&2 %1 -> %2 In Order", s2, s1));
  m_modeActionGroup->actions()[2]->setText(i18n("&3 %1 -> %2 Randomly", s1, s2));
  m_modeActionGroup->actions()[3]->setText(i18n("&4 %1 -> %2 Randomly", s2, s1));
  m_modeActionGroup->actions()[4]->setText(i18n("&5 %1 <-> %2 Randomly", s1, s2));

  Prefs::setMode(act->data().toInt());
  m_modeActionMenu->setIcon(QIcon::fromTheme("mode" + QString::number(Prefs::mode())));

  switch (Prefs::mode()){
  case 1:
    m_statusLabel->setText(i18n("%1 -> %2 In Order", s1, s2));
    break;
  case 2:
    m_statusLabel->setText(i18n("%1 -> %2 In Order", s2, s1));
    break;
  case 3:
    m_statusLabel->setText(i18n("%1 -> %2 Randomly", s1, s2));
    break;
  case 4:
    m_statusLabel->setText(i18n("%1 -> %2 Randomly", s2, s1));
    break;
  case 5:
    m_statusLabel->setText(i18n("%1 &lt;-&gt; %2 Randomly", s1, s2));
    break;
  }

  if (m_quiz !=0)
    slotCurrentPageChanged(m_pageWidget->currentPage(), m_pageWidget->currentPage());
}


void KWordQuizApp::slotInsertChar( int i )
{
  if (m_pageWidget->currentPage() == m_qaPage)
    m_qaView->slotSpecChar(Prefs::specialCharacters()[i - 1]);
  else
    if (m_pageWidget->currentPage() == m_editorPage) {
      m_tableView->slotSpecChar(Prefs::specialCharacters()[i - 1]);
    }
}


void KWordQuizApp::updateActions()
{
  bool fEdit = (m_pageWidget->currentPage() == m_editorPage);
  bool fQuiz = !fEdit && (m_quiz != 0);

  fileSave->setEnabled(fEdit);
  fileSaveAs->setEnabled(fEdit);
  filePrint->setEnabled(fEdit);
  filePrintPreview->setEnabled(fEdit);

  editCopy->setEnabled(fEdit);
  editCut->setEnabled(fEdit);
  editPaste->setEnabled(fEdit);
  editClear->setEnabled(fEdit);
  editInsert->setEnabled(fEdit);
  editDelete->setEnabled(fEdit);
  editMarkBlank->setEnabled(fEdit && Prefs::enableBlanks());
  editUnmarkBlank->setEnabled(fEdit && Prefs::enableBlanks());

  vocabLanguages->setEnabled(fEdit);
  vocabFont->setEnabled(fEdit);
  actionCollection()->action(QStringLiteral("vocab_image"))->setEnabled(fEdit);
  actionCollection()->action(QStringLiteral("vocab_sound"))->setEnabled(fEdit);
  vocabAdjustRows->setEnabled(fEdit);
  vocabShuffle->setEnabled(fEdit);

  quizCheck->setEnabled(fQuiz);
  quizRestart->setEnabled(fQuiz);
  quizRepeatErrors->setEnabled(false);
  quizExportErrors->setEnabled(false);

  flashKnow->setEnabled((m_pageWidget->currentPage() == m_flashPage) && fQuiz);
  flashDontKnow->setEnabled((m_pageWidget->currentPage() == m_flashPage) && fQuiz);

  qaHint->setEnabled((m_pageWidget->currentPage() == m_qaPage) && fQuiz);
  qaMarkLastCorrect->setVisible((m_pageWidget->currentPage() == m_qaPage) && fQuiz);
  actionCollection()->action(QStringLiteral("quiz_audio_play"))->setEnabled(false);

  quizOpt1->setEnabled((m_pageWidget->currentPage() == m_multiplePage) && fQuiz);
  quizOpt2->setEnabled((m_pageWidget->currentPage() == m_multiplePage) && fQuiz);
  quizOpt3->setEnabled((m_pageWidget->currentPage() == m_multiplePage) && fQuiz);

  configShowSearchBar->setEnabled(fEdit);

  addToolBar(Qt::RightToolBarArea, toolBar(QStringLiteral("quizToolBar")));
  toolBar(QStringLiteral("quizToolBar"))->setHidden(fEdit);
}


void KWordQuizApp::slotConfigShowSearch()
{
  if (m_searchLine) {
    m_searchLine->setVisible(m_searchLine->isHidden());
    Prefs::setShowSearch(m_searchLine->isVisible());
  }
}


void KWordQuizApp::slotCleanChanged(bool clean)
{
  m_doc->setModified(!clean);
  setWindowTitle(m_doc->url().fileName() + "[*]");
  setWindowModified(m_doc->isModified());
}


void KWordQuizApp::slotUndoTextChanged(const QString & undoText)
{
  Q_UNUSED(undoText);
  editUndo->setWhatsThis(editUndo->text());
  editUndo->setStatusTip(editUndo->text());

  slotModeActionGroupTriggered(m_modeActionGroup->actions().at(Prefs::mode() - 1));
}


void KWordQuizApp::slotRedoTextChanged(const QString &redoText)
{
  Q_UNUSED(redoText);
  editRedo->setWhatsThis(editRedo->text());
  editRedo->setStatusTip(editRedo->text());

  slotModeActionGroupTriggered(m_modeActionGroup->actions().at(Prefs::mode() - 1));
}

void KWordQuizApp::slotCreateErrorListDocument()
{
  if (m_quiz) {
    KEduVocDocument *errorDoc = new KEduVocDocument(this);
    errorDoc->appendIdentifier();
    errorDoc->identifier(0).setName(m_tableModel->headerData(0, Qt::Horizontal, Qt::DisplayRole).toString());
    errorDoc->appendIdentifier();
    errorDoc->identifier(1).setName(m_tableModel->headerData(1, Qt::Horizontal, Qt::DisplayRole).toString());

    foreach (int item, m_quiz->errorList()) {
      KEduVocExpression *errorExpr = new KEduVocExpression();
      errorDoc->lesson()->appendEntry(errorExpr);
      errorExpr->setTranslation(0, m_quiz->data(m_quiz->index(item, 0), Qt::DisplayRole).toString());
      errorExpr->setTranslation(1, m_quiz->data(m_quiz->index(item, 1), Qt::DisplayRole).toString());
    }
    saveDocAsFileName(errorDoc);
    if (m_dirWatch->contains(errorDoc->url().toLocalFile()))
      m_dirWatch->removeFile(errorDoc->url().toLocalFile());
  }
}

void KWordQuizApp::slotTableContextMenuRequested(const QPoint & pos)
{
    QMenu *popup = static_cast<QMenu*>(guiFactory()->container(QStringLiteral("editor_popup"),this));
    QAction *a;
    int column = m_tableView->currentIndex().column();
    QString currentLayout = QLatin1String("");
    currentLayout = m_tableModel->headerData(column, Qt::Horizontal, KWQTableModel::KeyboardLayoutRole).toString();

    // keyboard layout
    // try to talk to kxbk - get a list of keyboard layouts
    QDBusInterface kxbk(QStringLiteral("org.kde.keyboard"), QStringLiteral("/Layouts"), QStringLiteral("org.kde.KeyboardLayouts"));
    QDBusReply<QStringList> reply = kxbk.call(QStringLiteral("getLayoutsList"));
    if (reply.isValid()) {
        QStringList layouts = reply;
        //layouts.prepend(QString());
        QMenu *m = vocabLayouts->menu();
        m->clear();
        foreach (const QString &s, layouts) {
            a = m->addAction(s);
            a->setData(s);
            a->setCheckable(true);
            a->setActionGroup(m_layoutActionGroup);
            if (s == currentLayout)
              a->setChecked(true);
        }
    } else {
        qDebug() << "kxkb dbus error";
    }

    popup->exec(m_tableView->viewport()->mapToGlobal(pos));
}

void KWordQuizApp::slotLayoutActionGroupTriggered(QAction *act)
{
    int column = m_tableView->currentIndex().column();
    m_tableModel->setHeaderData(column, Qt::Horizontal, act->data().toString(), KWQTableModel::KeyboardLayoutRole);
}
