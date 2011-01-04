/***************************************************************************
                          kwordquiz.cpp  -  description
                             -------------------
    begin                : Wed Jul 24 20:12:30 PDT 2002
    copyright            : (C) 2002-2005 by Peter Hedlund
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
//
// include files for QT
#include <tqpainter.h>
#include <tqbitmap.h>
#include <tqcheckbox.h>

// include files for KDE
#include <kmessagebox.h>
#include <kfiledialog.h>
#include <kstatusbar.h>
#include <klocale.h>
#include <kedittoolbar.h>
#include <kstandarddirs.h> //locate
#include <kfontdialog.h>
#include <kpopupmenu.h>
#include <knotifydialog.h>
#include <kiconloader.h>
//#include <keduvocdata.h>
#include <kdebug.h>

// application specific includes
#include "kwordquiz.h"
#include "kwordquizdoc.h"
#include "dlglanguage.h"
#include "kwordquizprefs.h"
#include "qaview.h"
#include "flashview.h"
#include "multipleview.h"
#include "wqprintdialogpage.h"
#include "prefs.h"
#include "kwqnewstuff.h"

#define ID_STATUS_MSG 1
#define ID_STATUS_MSG_MODE 2
#define ID_STATUS_MSG_SCORE 3

#define ID_MODE_1 1

#define ID_MENU_QUIZ 1001

KWordQuizApp::KWordQuizApp(TQWidget* , const char* name):KMainWindow(0, name)
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
  m_newStuff = 0;

  slotQuizEditor();
  slotUndoChange(i18n("Cannot &Undo"), false);
  updateMode(Prefs::mode());

  m_prefDialog = 0;

  editMarkBlank->setEnabled(Prefs::enableBlanks());
  editUnmarkBlank->setEnabled(Prefs::enableBlanks());

  if (Prefs::firstRun())
  {
    fileOpenRecent->addURL( locate("data", "kwordquiz/examples/example.kvtml"));
    fileOpenRecent->addURL( locate("data", "kwordquiz/examples/french_verbs.kvtml"));
    fileOpenRecent->addURL( locate("data", "kwordquiz/examples/fill_in_the_blank.kvtml"));
    fileOpenRecent->addURL( locate("data", "kwordquiz/examples/us_states_and_capitals.kvtml"));
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

  fileNew = KStdAction::openNew(this, TQT_SLOT(slotFileNew()), actionCollection());
  fileNew->setWhatsThis(i18n("Creates a new blank vocabulary document"));
  fileNew->setToolTip(fileNew->whatsThis());

  fileOpen = KStdAction::open(this, TQT_SLOT(slotFileOpen()), actionCollection());
  fileOpen->setWhatsThis(i18n("Opens an existing vocabulary document"));
  fileOpen->setToolTip(fileOpen->whatsThis());

  fileOpenRecent = KStdAction::openRecent(this, TQT_SLOT(slotFileOpenRecent(const KURL&)), actionCollection());

  fileGHNS = new KAction(i18n("&Get New Vocabularies..."), "knewstuff", "CTRL+G", this, TQT_SLOT(slotFileGHNS()), actionCollection(), "file_ghns");
  fileGHNS->setWhatsThis(i18n("Downloads new vocabularies"));
  fileGHNS->setToolTip(fileGHNS->whatsThis());

  fileSave = KStdAction::save(this, TQT_SLOT(slotFileSave()), actionCollection());
  fileSave->setWhatsThis(i18n("Saves the active vocabulary document"));
  fileSave->setToolTip(fileSave->whatsThis());

  fileSaveAs = KStdAction::saveAs(this, TQT_SLOT(slotFileSaveAs()), actionCollection());
  fileSaveAs->setWhatsThis(i18n("Saves the active vocabulary document with a different name"));
  fileSaveAs->setToolTip(fileSaveAs->whatsThis());

  fileClose = KStdAction::close(this, TQT_SLOT(slotFileClose()), actionCollection());
  fileClose->setWhatsThis(i18n("Closes the active vocabulary document"));
  fileClose->setToolTip(fileClose->whatsThis());

  filePrint = KStdAction::print(this, TQT_SLOT(slotFilePrint()), actionCollection());
  filePrint->setWhatsThis(i18n("Prints the active vocabulary document"));
  filePrint->setToolTip(filePrint->whatsThis());

  fileQuit = KStdAction::quit(this, TQT_SLOT(slotFileQuit()), actionCollection());
  fileQuit->setWhatsThis(i18n("Quits KWordQuiz"));
  fileQuit->setToolTip(fileQuit->whatsThis());

  editUndo = KStdAction::undo(this, TQT_SLOT(slotEditUndo()), actionCollection());
  editUndo->setWhatsThis(i18n("Undoes the last command"));
  editUndo->setToolTip(editUndo->whatsThis());

  editCut = KStdAction::cut(this, TQT_SLOT(slotEditCut()), actionCollection());
  editCut->setWhatsThis(i18n("Cuts the text from the selected cells and places it on the clipboard"));
  editCut->setToolTip(editCut->whatsThis());

  editCopy = KStdAction::copy(this, TQT_SLOT(slotEditCopy()), actionCollection());
  editCopy->setWhatsThis(i18n("Copies the text from the selected cells and places it on the clipboard"));
  editCopy->setToolTip(editCopy->whatsThis());

  editPaste = KStdAction::paste(this, TQT_SLOT(slotEditPaste()), actionCollection());
  editPaste->setWhatsThis(i18n("Pastes previously cut or copied text from the clipboard into the selected cells"));
  editPaste->setToolTip(editPaste->whatsThis());

  editClear = KStdAction::clear(this, TQT_SLOT(slotEditClear()), actionCollection());
  editClear->setWhatsThis(i18n("Clears the content of the selected cells"));
  editClear->setToolTip(editClear->whatsThis());

  editInsert = new KAction(i18n("&Insert Row"), "insert_table_row", "CTRL+I", this, TQT_SLOT(slotEditInsert()), actionCollection(),"edit_insert");
  editInsert->setWhatsThis(i18n("Inserts a new row above the current row"));
  editInsert->setToolTip(editInsert->whatsThis());

  editDelete = new KAction(i18n("&Delete Row"), "delete_table_row", "CTRL+K", this, TQT_SLOT(slotEditDelete()), actionCollection(),"edit_delete");
  editDelete->setWhatsThis(i18n("Deletes the selected row(s)"));
  editDelete->setToolTip(editDelete->whatsThis());

  editMarkBlank = new KAction(i18n("&Mark as Blank"), "markasblank", "CTRL+M", this, TQT_SLOT(slotEditMarkBlank()), actionCollection(),"edit_mark_blank");
  editMarkBlank->setWhatsThis(i18n("Marks the current or selected word as a blank for Fill-in-the-blank"));
  editMarkBlank->setToolTip(editMarkBlank->whatsThis());

  editUnmarkBlank = new KAction(i18n("&Unmark Blanks"), "unmarkasblank", 0, this, TQT_SLOT(slotEditUnmarkBlank()), actionCollection(),"edit_unmark_blank");
  editUnmarkBlank->setWhatsThis(i18n("Removes blanks from the current or selected word"));
  editUnmarkBlank->setToolTip(editUnmarkBlank->whatsThis());

  //@todo implement editFind = KStdAction::find(this, TQT_SLOT(slotEditFind()), actionCollection());

  vocabLanguages = new KAction(i18n("&Column Titles..."), "languages", "CTRL+L", this, TQT_SLOT(slotVocabLanguages()), actionCollection(),"vocab_languages");
  vocabLanguages->setWhatsThis(i18n("Defines the column titles for the active vocabulary"));
  vocabLanguages->setToolTip(vocabLanguages->whatsThis());

  vocabFont = new KAction(i18n("&Font..."), "fonts", 0, this, TQT_SLOT(slotVocabFont()), actionCollection(),"vocab_font");
  vocabFont->setWhatsThis(i18n("Defines the font used by the editor"));
  vocabFont->setToolTip(vocabFont->whatsThis());

  //@todo implement vocabKeyboard = new KAction(i18n("&Keyboard..."), "kxkb", 0, this, TQT_SLOT(slotVocabKeyboard()), actionCollection(),"vocab_keyboard");

  vocabRC = new KAction(i18n("&Rows/Columns..."), "rowcol", 0, this, TQT_SLOT(slotVocabRC()), actionCollection(),"vocab_rc");
  vocabRC->setWhatsThis(i18n("Defines the number of rows, row heights, and column widths for the active vocabulary"));
  vocabRC->setToolTip(vocabRC->whatsThis());

  vocabSort = new KAction(i18n("&Sort..."), "sort_incr", 0, this, TQT_SLOT(slotVocabSort()), actionCollection(),"vocab_sort");
  vocabSort->setWhatsThis(i18n("Sorts the vocabulary in ascending or descending order based on the left or right column"));
  vocabSort->setToolTip(vocabSort->whatsThis());

  vocabShuffle = new KAction(i18n("Sh&uffle"), "shuffle", 0, this, TQT_SLOT(slotVocabShuffle()), actionCollection(),"vocab_shuffle");
  vocabShuffle->setWhatsThis(i18n("Shuffles the entries of the active vocabulary"));
  vocabShuffle->setToolTip(vocabShuffle->whatsThis());

  mode = new KToolBarPopupAction(i18n("Change Mode"), "mode1", 0, this, TQT_SLOT(slotMode0()), actionCollection(),"mode_0");
  mode->setWhatsThis(i18n("Changes the mode used in quiz sessions"));
  mode->setToolTip(mode->whatsThis());

  KPopupMenu *popup = mode->popupMenu();
  popup->clear();
  popup->insertItem(KGlobal::iconLoader()->loadIconSet("mode1", KIcon::Toolbar), "", this, TQT_SLOT(slotMode1()), 0, 0);
  popup->insertItem(KGlobal::iconLoader()->loadIconSet("mode2", KIcon::Toolbar), "", this, TQT_SLOT(slotMode2()), 0, 1);
  popup->insertItem(KGlobal::iconLoader()->loadIconSet("mode3", KIcon::Toolbar), "", this, TQT_SLOT(slotMode3()), 0, 2);
  popup->insertItem(KGlobal::iconLoader()->loadIconSet("mode4", KIcon::Toolbar), "", this, TQT_SLOT(slotMode4()), 0, 3);
  popup->insertItem(KGlobal::iconLoader()->loadIconSet("mode5", KIcon::Toolbar), "", this, TQT_SLOT(slotMode5()), 0, 4);

  mode1 = new KToggleAction("", "mode1", 0, this, TQT_SLOT(slotMode1()), actionCollection(),"mode_1");
  mode2 = new KToggleAction("", "mode2", 0, this, TQT_SLOT(slotMode2()), actionCollection(),"mode_2");
  mode3 = new KToggleAction("", "mode3", 0, this, TQT_SLOT(slotMode3()), actionCollection(),"mode_3");
  mode4 = new KToggleAction("", "mode4", 0, this, TQT_SLOT(slotMode4()), actionCollection(),"mode_4");
  mode5 = new KToggleAction("", "mode5", 0, this, TQT_SLOT(slotMode5()), actionCollection(),"mode_5");
  mode1->setWhatsThis(i18n("Selects this mode"));
  mode2->setWhatsThis(i18n("Selects this mode"));
  mode3->setWhatsThis(i18n("Selects this mode"));
  mode4->setWhatsThis(i18n("Selects this mode"));
  mode5->setWhatsThis(i18n("Selects this mode"));
  mode1->setToolTip(mode1->whatsThis());
  mode2->setToolTip(mode2->whatsThis());
  mode3->setToolTip(mode3->whatsThis());
  mode4->setToolTip(mode4->whatsThis());
  mode5->setToolTip(mode5->whatsThis());

  quizEditor = new KAction(i18n("&Editor"), "editor", "F6", this, TQT_SLOT(slotQuizEditor()), actionCollection(),"quiz_editor");
  quizEditor->setWhatsThis(i18n("Activates the vocabulary editor"));
  quizEditor->setToolTip(quizEditor->whatsThis());

  quizFlash = new KAction(i18n("&Flashcard"), "flash", "F7", this, TQT_SLOT(slotQuizFlash()), actionCollection(),"quiz_flash");
  quizFlash->setWhatsThis(i18n("Starts a flashcard session using the active vocabulary"));
  quizFlash->setToolTip(quizFlash->whatsThis());

  quizMultiple = new KAction(i18n("&Multiple Choice"), "multiple", "F8", this, TQT_SLOT(slotQuizMultiple()), actionCollection(),"quiz_multiple");
  quizMultiple->setWhatsThis(i18n("Starts a multiple choice session using the active vocabulary"));
  quizMultiple->setToolTip(quizMultiple->whatsThis());

  quizQA = new KAction(i18n("&Question && Answer"), "qa", "F9", this, TQT_SLOT(slotQuizQA()), actionCollection(),"quiz_qa");
  quizQA->setWhatsThis(i18n("Starts a question and answer session using the active vocabulary"));
  quizQA->setToolTip(quizQA->whatsThis());

  quizCheck = new KAction(i18n("&Check"), "check", "Return", this, 0, actionCollection(),"quiz_check");
  quizCheck->setWhatsThis(i18n("Checks your answer to this question"));
  quizCheck->setToolTip(quizCheck->whatsThis());

  flashKnow = new KAction(i18n("I &Know"), "know", "K", this, 0, actionCollection(),"flash_know");
  flashKnow->setWhatsThis(i18n("Counts this card as correct and shows the next card"));
  flashKnow->setToolTip(flashKnow->whatsThis());

  flashDontKnow = new KAction(i18n("I &Do Not Know"), "dontknow", "D", this, 0, actionCollection(),"flash_dont_know");
  flashDontKnow->setWhatsThis(i18n("Counts this card as incorrect and shows the next card"));
  flashDontKnow->setToolTip(flashDontKnow->whatsThis());

  qaHint = new KAction(i18n("&Hint"), "hint", "CTRL+H", this, 0, actionCollection(),"qa_hint");
  qaHint->setWhatsThis(i18n("Gets the next correct letter of the answer"));
  qaHint->setToolTip(qaHint->whatsThis());

  quizRestart = new KAction(i18n("&Restart"), "restart.png", "CTRL+R", this, 0, actionCollection(), "quiz_restart");
  quizRestart->setWhatsThis(i18n("Restarts the quiz session from the beginning"));
  quizRestart->setToolTip(quizRestart->whatsThis());

  quizRepeatErrors = new KAction(i18n("Repeat &Errors"), "repeat", "CTRL+E", this, 0, actionCollection(),"quiz_repeat_errors");
  quizRepeatErrors->setWhatsThis(i18n("Repeats all incorrectly answered questions"));
  quizRepeatErrors->setToolTip(quizRepeatErrors->whatsThis());

  configNotifications = KStdAction::configureNotifications(this, TQT_SLOT(slotConfigureNotifications()), actionCollection());
  configNotifications->setWhatsThis(i18n("Configures sound and other notifications for certain events"));
  configNotifications->setToolTip(configNotifications->whatsThis());

  configApp = KStdAction::preferences(this, TQT_SLOT( slotConfigure()), actionCollection());
  configApp->setWhatsThis(i18n("Specifies preferences for the vocabulary editor and quiz sessions"));
  configApp->setToolTip(configApp->whatsThis());

  charMapper = new TQSignalMapper(this);
  connect(charMapper, TQT_SIGNAL(mapped(int)), this, TQT_SLOT(slotInsertChar(int)));

  specialChar1 = new KAction(i18n("Special Character 1"), "", "CTRL+1", charMapper, TQT_SLOT(map()), actionCollection(), "char_1") ;
  specialChar2 = new KAction(i18n("Special Character 2"), 0, "CTRL+2", charMapper, TQT_SLOT(map()), actionCollection(), "char_2") ;
  specialChar3 = new KAction(i18n("Special Character 3"), 0, "CTRL+3", charMapper, TQT_SLOT(map()), actionCollection(), "char_3") ;
  specialChar4 = new KAction(i18n("Special Character 4"), 0, "CTRL+4", charMapper, TQT_SLOT(map()), actionCollection(), "char_4") ;
  specialChar5 = new KAction(i18n("Special Character 5"), 0, "CTRL+5", charMapper, TQT_SLOT(map()), actionCollection(), "char_5") ;
  specialChar6 = new KAction(i18n("Special Character 6"), 0, "CTRL+6", charMapper, TQT_SLOT(map()), actionCollection(), "char_6") ;
  specialChar7 = new KAction(i18n("Special Character 7"), 0, "CTRL+7", charMapper, TQT_SLOT(map()), actionCollection(), "char_7") ;
  specialChar8 = new KAction(i18n("Special Character 8"), 0, "CTRL+8", charMapper, TQT_SLOT(map()), actionCollection(), "char_8") ;
  specialChar9 = new KAction(i18n("Special Character 9"), 0, "CTRL+9", charMapper, TQT_SLOT(map()), actionCollection(), "char_9") ;

  charMapper->setMapping(specialChar1, 1);
  charMapper->setMapping(specialChar2, 2);
  charMapper->setMapping(specialChar3, 3);
  charMapper->setMapping(specialChar4, 4);
  charMapper->setMapping(specialChar5, 5);
  charMapper->setMapping(specialChar6, 6);
  charMapper->setMapping(specialChar7, 7);
  charMapper->setMapping(specialChar8, 8);
  charMapper->setMapping(specialChar9, 9);

  actionCollection()->setHighlightingEnabled(true);
  connect(actionCollection(), TQT_SIGNAL(actiontqStatusText(const TQString &)), this, TQT_SLOT(slotqStatusMsg(const TQString &)));
  connect(actionCollection(), TQT_SIGNAL(actionHighlighted(KAction *, bool)), this, TQT_SLOT(slotActionHighlighted(KAction *, bool)));
  updateSpecialCharIcons();

  if (!initialGeometrySet())
    resize( TQSize(650, 500).expandedTo(tqminimumSizeHint()));
  setupGUI(ToolBar | Keys | StatusBar | Create);
  setAutoSaveSettings();

   configToolbar = actionCollection()->action("options_configure_toolbars");
   configToolbar->setWhatsThis(i18n("Toggles display of the toolbars"));
   configToolbar->setToolTip(configToolbar->whatsThis());
}

void KWordQuizApp::initStatusBar()
{
  statusBar()->insertFixedItem("", ID_STATUS_MSG_MODE, true);
  statusBar()->setItemFixed(ID_STATUS_MSG_MODE, 250);
  statusBar()->setItemAlignment(ID_STATUS_MSG_MODE, AlignLeft|AlignVCenter);
}

void KWordQuizApp::initDocument()
{
  doc = new KWordQuizDoc(this);
  doc->newDocument();
}

void KWordQuizApp::initView()
{
  m_editView = new KWordQuizView(this);
  doc->addView(m_editView);
  setCentralWidget(m_editView);
  setCaption(doc->URL().fileName(),false);
  m_editView->setFont(Prefs::editorFont());
  connect(m_editView, TQT_SIGNAL(undoChange(const TQString&, bool )), this, TQT_SLOT(slotUndoChange(const TQString&, bool)));
  connect(m_editView, TQT_SIGNAL(contextMenuRequested(int, int, const TQPoint &)), this, TQT_SLOT(slotContextMenuRequested(int, int, const TQPoint& )));
}

void KWordQuizApp::openURL(const KURL& url)
{
  if(!url.isEmpty()) {
    if (m_dirWatch->tqcontains(url.path()))
    {
      KMainWindow* w;
      if(memberList)
      {
        for(w=memberList->first(); w!=0; w=memberList->next())
        {
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
      if (doc->URL().fileName() == i18n("Untitled")  && m_editView->gridIsEmpty()){
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

void KWordQuizApp::openDocumentFile(const KURL& url)
{
  slotqStatusMsg(i18n("Opening file..."));
  if (!url.isEmpty()) {
    doc->openDocument( url);
    m_dirWatch->addFile(url.path());
    setCaption(doc->URL().fileName(), false);
    fileOpenRecent->addURL( url );
    updateMode(Prefs::mode());
  }
  slotqStatusMsg(i18n("Ready"));
}


KWordQuizDoc *KWordQuizApp::getDocument() const
{
  return doc;
}

void KWordQuizApp::saveOptions()
{
  fileOpenRecent->saveEntries(kapp->config(), "Recent Files");
  Prefs::writeConfig();
}


void KWordQuizApp::readOptions()
{
  //Prefs::readConfig();
  fileOpenRecent->loadEntries(kapp->config(), "Recent Files");
}

void KWordQuizApp::saveProperties(KConfig *_cfg)
{
  if(doc->URL().fileName()!=i18n("Untitled") && !doc->isModified())
  {
    // saving to tempfile not necessary

  }
  else
  {
    KURL url=doc->URL();
    _cfg->writeEntry("filename", url.url());
    _cfg->writeEntry("modified", doc->isModified());
    TQString tempname = kapp->tempSaveName(url.url());
    TQString tempurl= KURL::encode_string(tempname);
    KURL _url(tempurl);
    doc->saveDocument(_url);
  }
}


void KWordQuizApp::readProperties(KConfig* _cfg)
{
  TQString filename = _cfg->readEntry("filename", "");
  KURL url(filename);
  bool modified = _cfg->readBoolEntry("modified", false);
  if(modified)
  {
    bool canRecover;
    TQString tempname = kapp->checkRecoverFile(filename, canRecover);
    KURL _url(tempname);

    if(canRecover)
    {
      doc->openDocument(_url);
      doc->setModified();
      setCaption(_url.fileName(),true);
      TQFile::remove(tempname);
    }
  }
  else
  {
    if(!filename.isEmpty())
    {
      doc->openDocument(url);
      setCaption(url.fileName(),false);
    }
  }
}

bool KWordQuizApp::queryClose()
{
  bool f = doc->saveModified();
  if (f)
    if (m_dirWatch->tqcontains(doc->URL().path()))
      m_dirWatch->removeFile(doc->URL().path());
  return f;
}

bool KWordQuizApp::queryExit()
{
  saveOptions();
  return true;
}

/////////////////////////////////////////////////////////////////////
// TQT_SLOT IMPLEMENTATION
/////////////////////////////////////////////////////////////////////

void KWordQuizApp::slotFileNew()
{
  slotqStatusMsg(i18n("Opening a new document window..."));
  if (doc->URL().fileName() == i18n("Untitled")  && m_editView->gridIsEmpty()){
    // neither saved nor has content, as good as new
  }
  else
  {
    KWordQuizApp *new_window= new KWordQuizApp();
    new_window->show();
  }
  slotqStatusMsg(i18n("Ready"));
}

void KWordQuizApp::slotFileOpen()
{
  slotqStatusMsg(i18n("Opening file..."));

  TQCheckBox * cb = new TQCheckBox(i18n("&Join selected files into one list"), 0, 0);
  cb -> setChecked(false);

  TQString filter = i18n("*.kvtml *.wql *.xml.gz *.csv|All Supported Documents\n*.kvtml|KDE Vocabulary Document\n*.wql|KWordQuiz Document\n*.xml.gz|Pauker Lesson\n*.csv|Comma-Separated Values");
  KFileDialog *fd = new KFileDialog(TQString::null, filter, this, 0, true, cb);
  fd -> setOperationMode(KFileDialog::Opening);
  fd -> setMode(KFile::Files | KFile::ExistingOnly);
  fd -> setCaption(i18n("Open Vocabulary Document"));

  if (fd->exec() == TQDialog::Accepted)
  {
    KURL::List l = fd -> selectedURLs();
    bool append = ((cb -> isChecked()) && (l.count() > 1));

    if (append)
    {
      KWordQuizApp * w;
      if (doc->URL().fileName() == i18n("Untitled")  && m_editView->gridIsEmpty()){
        // neither saved nor has content, as good as new
        w = this;
      }
      else
      {
        w = new KWordQuizApp();
        w->show();
      }

      KURL::List::iterator it;
      int i = 0;
      for(it = l.begin(); it != l.end(); ++it)
      {
        w->getDocument()->openDocument(*it, true, i);
        i++;
      }
    }
    else
    {
      KURL::List::iterator it;
      for(it = l.begin(); it != l.end(); ++it)
      {
        openURL(*it);
      }
    }
  }

  delete (fd); //deletes cb also

  slotqStatusMsg(i18n("Ready"));
}

void KWordQuizApp::slotFileOpenRecent(const KURL& url)
{
  slotqStatusMsg(i18n("Opening file..."));
  fileOpenRecent->setCurrentItem(-1);
  openURL(url);
  slotqStatusMsg(i18n("Ready"));
}

void KWordQuizApp::slotFileSave()
{
  slotqStatusMsg(i18n("Saving file..."));
  if (doc->URL().fileName() == i18n("Untitled") )
  {
    slotFileSaveAs();
  }
  else
  {
    doc->saveDocument(doc->URL());
  }
  slotqStatusMsg(i18n("Ready"));
}

void KWordQuizApp::slotFileGHNS()
{
  if (!m_newStuff)
    m_newStuff = new KWQNewStuff(this);
  m_newStuff->download();
}

void KWordQuizApp::slotFileSaveAs()
{
  slotqStatusMsg(i18n("Saving file with a new filename..."));
  saveAsFileName();
  slotqStatusMsg(i18n("Ready"));
}

bool KWordQuizApp::saveAsFileName( )
{

  bool success = false;

  TQString filter = i18n("*.kvtml|KDE Vocabulary Document\n*.wql|KWordQuiz Document\n*.csv|Comma-Separated Values\n*.html|Hypertext Markup Language");
  KFileDialog *fd = new KFileDialog(TQString::null, filter, this, 0, true);
  fd -> setOperationMode(KFileDialog::Saving);
  fd -> setCaption(i18n("Save Vocabulary Document As"));

  if (fd->exec() == TQDialog::Accepted)
  {
    KURL url = fd -> selectedURL();
    if(!url.isEmpty()){

      //@todo check that a valid extension was really given
      if (!url.fileName().tqcontains('.'))
      {
        if  (fd->currentFilter() == "*.wql")
          url = KURL(url.path() + ".wql");
        else if (fd->currentFilter() == "*.csv")
          url = KURL(url.path() + ".csv");
        else if (fd->currentFilter() == "*.html")
          url = KURL(url.path() + ".html");
        else
          url = KURL(url.path() + ".kvtml");
      }

      TQFileInfo fileinfo(url.path());
      if (fileinfo.exists() && KMessageBox::warningContinueCancel(0,
          i18n("<qt>The file<br><b>%1</b><br>already exists. Do you want to overwrite it?</qt>")
              .arg(url.path()),TQString::null,i18n("Overwrite")) == KMessageBox::Cancel)
      {
      // do nothing
      }
      else
      {
        if (m_dirWatch ->tqcontains(doc->URL().path()))
          m_dirWatch ->removeFile(doc->URL().path());
        doc->saveDocument(url);
        m_dirWatch->addFile(url.path());
        fileOpenRecent->addURL(url);
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
  slotqStatusMsg(i18n("Closing file..."));

  if (memberList->count() > 1)
    close();
  else
    if (queryClose())
    {
      if (m_quiz != 0)
      {
        delete(m_quiz);
        m_quiz = 0;
      }
      doc->newDocument();
      setCaption(doc->URL().fileName(), doc->isModified());
      delete (m_editView);
      initView();
      slotQuizEditor();
      slotUndoChange(i18n("Cannot &Undo"), false);
      updateMode(Prefs::mode());
      m_editView ->setFocus();
    }

  slotqStatusMsg(i18n("Ready"));
}

void KWordQuizApp::slotFilePrint()
{
  slotqStatusMsg(i18n("Printing..."));
  WQPrintDialogPage * p = new WQPrintDialogPage(this);
  KPrinter printer;
  printer.addDialogPage(p);
  printer.setFullPage(true);
  if (printer.setup(this))
  {
    m_editView->print(&printer);
  }

  slotqStatusMsg(i18n("Ready"));
}

void KWordQuizApp::slotFileQuit()
{
  slotqStatusMsg(i18n("Exiting..."));
  saveOptions();
  // close the first window, the list makes the next one the first again.
  // This ensures that queryClose() is called on each window to ask for closing
  KMainWindow* w;
  if(memberList)
  {
    for(w=memberList->first(); w!=0; w=memberList->next())
    {
      // only close the window if the closeEvent is accepted. If the user presses Cancel on the saveModified() dialog,
      // the window and the application stay open.
      if(!w->close())
        break;
    }
  }
}

void KWordQuizApp::slotUndoChange( const TQString & text, bool enabled )
{
  editUndo->setText(text);
  editUndo->setEnabled(enabled);
}

void KWordQuizApp::slotEditUndo()
{
  slotqStatusMsg(i18n("Undoing previous command..."));
  m_editView->doEditUndo();
  slotqStatusMsg(i18n("Ready"));
}

void KWordQuizApp::slotEditCut()
{
  slotqStatusMsg(i18n("Cutting selection..."));
  m_editView->doEditCut();
  slotqStatusMsg(i18n("Ready"));
}

void KWordQuizApp::slotEditCopy()
{
  slotqStatusMsg(i18n("Copying selection to clipboard..."));
  m_editView->doEditCopy();
  slotqStatusMsg(i18n("Ready"));
}

void KWordQuizApp::slotEditPaste()
{
  slotqStatusMsg(i18n("Inserting clipboard contents..."));
  m_editView->doEditPaste();
  slotqStatusMsg(i18n("Ready"));
}

void KWordQuizApp::slotEditClear()
{
  slotqStatusMsg(i18n("Clearing the selected cells..."));
  m_editView->doEditClear();
  slotqStatusMsg(i18n("Ready"));
}

void KWordQuizApp::slotEditInsert()
{
  slotqStatusMsg(i18n("Inserting rows..."));
  m_editView->doEditInsert();
  slotqStatusMsg(i18n("Ready"));
}

void KWordQuizApp::slotEditDelete()
{
  slotqStatusMsg(i18n("Deleting selected rows..."));
  m_editView->doEditDelete();
  slotqStatusMsg(i18n("Ready"));
}

void KWordQuizApp::slotEditMarkBlank()
{
  slotqStatusMsg(i18n("Marking selected text as a blank..."));
  m_editView->doEditMarkBlank();
  slotqStatusMsg(i18n("Ready"));
}

void KWordQuizApp::slotEditUnmarkBlank()
{
  slotqStatusMsg(i18n("Removing blank markings..."));
  m_editView->doEditUnmarkBlank();
  slotqStatusMsg(i18n("Ready"));
}

void KWordQuizApp::slotEditFind()
{
  slotqStatusMsg(i18n("Searching for indicated text..."));
  KMessageBox::sorry(0, i18n("Not implemented yet"));
  slotqStatusMsg(i18n("Ready"));
}

void KWordQuizApp::slotVocabLanguages()
{
  slotqStatusMsg(i18n("Setting the column titles of the vocabulary..."));
  DlgLanguage* dlg;
  dlg = new DlgLanguage(this, "dlg_lang", true);
  dlg->setLanguage(1, m_editView -> horizontalHeader()->label(0));
  dlg->setLanguage(2, m_editView -> horizontalHeader()->label(1));
  dlg->disableResize();
  if (dlg->exec() == KDialogBase::Accepted)
  {
    m_editView -> horizontalHeader()->setLabel(0, dlg->Language(1));
    m_editView -> horizontalHeader()->setLabel(1, dlg->Language(2));
    updateMode(Prefs::mode());
  }
  slotqStatusMsg(i18n("Ready"));
}

void KWordQuizApp::slotVocabFont()
{
  slotqStatusMsg(i18n("Setting the font of the vocabulary..."));
  KFontDialog* dlg;
  dlg = new KFontDialog(this, "dlg_font", false, true);
  dlg->setFont(m_editView -> font());
  if (dlg->exec() == KFontDialog::Accepted)
  {
    m_editView ->setFont(dlg->font());
    Prefs::setEditorFont(dlg->font());
    doc->setModified(true);
  }
  slotqStatusMsg(i18n("Ready"));
}

void KWordQuizApp::slotVocabKeyboard()
{
  slotqStatusMsg(i18n("Changing the keyboard tqlayout..."));
  KMessageBox::sorry(0, i18n("Not implemented yet"));
  slotqStatusMsg(i18n("Ready"));
}

void KWordQuizApp::slotVocabChar()
{
  slotqStatusMsg(i18n("Inserting special character..."));
  m_editView->doVocabSpecChar();
  slotqStatusMsg(i18n("Ready"));
}

void KWordQuizApp::slotVocabRC()
{
  slotqStatusMsg(i18n("Changing row and column properties..."));
  m_editView->doVocabRC();
  slotqStatusMsg(i18n("Ready"));
}

void KWordQuizApp::slotVocabSort()
{
  slotqStatusMsg(i18n("Sorting the vocabulary..."));
  m_editView->doVocabSort();
  slotqStatusMsg(i18n("Ready"));
}

void KWordQuizApp::slotVocabShuffle()
{
  slotqStatusMsg(i18n("Randomizing the vocabulary..."));
  m_editView->doVocabShuffle();
  slotqStatusMsg(i18n("Ready"));
}

void KWordQuizApp::slotMode0()
{
  slotqStatusMsg(i18n("Updating mode..."));
  if (Prefs::mode() < 5) {
    updateMode(Prefs::mode() + 1);
  }
  else
  {
  updateMode(1);
  }
  slotqStatusMsg(i18n("Ready"));
}

void KWordQuizApp::slotMode1()
{
  slotqStatusMsg(i18n("Updating mode..."));
  updateMode(1);
  slotqStatusMsg(i18n("Ready"));
}

void KWordQuizApp::slotMode2()
{
  slotqStatusMsg(i18n("Updating mode..."));
  updateMode(2);
  slotqStatusMsg(i18n("Ready"));
}

void KWordQuizApp::slotMode3()
{
  slotqStatusMsg(i18n("Updating mode..."));
  updateMode(3);
  slotqStatusMsg(i18n("Ready"));
}

void KWordQuizApp::slotMode4()
{
  slotqStatusMsg(i18n("Updating mode..."));
  updateMode(4);
  slotqStatusMsg(i18n("Ready"));
}

void KWordQuizApp::slotMode5()
{
  slotqStatusMsg(i18n("Updating mode..."));
  updateMode(5);
  slotqStatusMsg(i18n("Ready"));
}

void KWordQuizApp::slotQuizEditor()
{
  slotqStatusMsg(i18n("Starting editor session..."));
  updateSession(WQQuiz::qtEditor);
  slotqStatusMsg(i18n("Ready"));
}

void KWordQuizApp::slotQuizFlash()
{
  slotqStatusMsg(i18n("Starting flashcard session..."));
  updateSession(WQQuiz::qtFlash);
  slotqStatusMsg(i18n("Ready"));
}

void KWordQuizApp::slotQuizMultiple()
{
  slotqStatusMsg(i18n("Starting multiple choice session..."));
  updateSession(WQQuiz::qtMultiple);
  slotqStatusMsg(i18n("Ready"));
}

void KWordQuizApp::slotQuizQA()
{
  slotqStatusMsg(i18n("Starting question & answer session..."));
  updateSession(WQQuiz::qtQA);
  slotqStatusMsg(i18n("Ready"));
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
      m_editView->show();
      setCentralWidget(m_editView);
      m_editView -> setFocus();
      break;
    case WQQuiz::qtFlash:
      m_quiz = new WQQuiz(m_editView);
      connect(m_quiz, TQT_SIGNAL(checkingAnswer(int )), m_editView, TQT_SLOT(slotCheckedAnswer(int )));
      m_quiz ->setQuizType(WQQuiz::qtFlash);
      m_quiz->setQuizMode(Prefs::mode());
      if (m_quiz -> init())
      {
        m_editView->saveCurrentSelection(true);
        m_editView->hide();
        m_flashView = new FlashView(this);
        connect(quizCheck, TQT_SIGNAL(activated()), m_flashView, TQT_SLOT(slotFlip()));
        connect(flashKnow, TQT_SIGNAL(activated()), m_flashView, TQT_SLOT(slotKnow()));
        connect(flashDontKnow, TQT_SIGNAL(activated()), m_flashView, TQT_SLOT(slotDontKnow()));
        connect(quizRestart, TQT_SIGNAL(activated()), m_flashView, TQT_SLOT(slotRestart()));
        connect(quizRepeatErrors, TQT_SIGNAL(activated()), m_flashView, TQT_SLOT(slotRepeat()));
        connect(this, TQT_SIGNAL(settingsChanged()), m_flashView, TQT_SLOT(slotApplySettings()));

        setCentralWidget(m_flashView);
        m_flashView -> setQuiz(m_quiz);
        m_flashView ->init();
        m_flashView->show();
      }
      else
      {
        delete(m_quiz);
        m_quiz = 0;
      }
      break;
    case WQQuiz::qtMultiple:
      m_quiz = new WQQuiz(m_editView);
      connect(m_quiz, TQT_SIGNAL(checkingAnswer(int )), m_editView, TQT_SLOT(slotCheckedAnswer(int )));
      m_quiz ->setQuizType(WQQuiz::qtMultiple);
      m_quiz->setQuizMode(Prefs::mode());
      if (m_quiz -> init())
      {
        m_editView->saveCurrentSelection(true);
        m_editView->hide();
        m_multipleView = new MultipleView(this);
        connect(quizCheck, TQT_SIGNAL(activated()), m_multipleView, TQT_SLOT(slotCheck()));
        connect(quizRestart, TQT_SIGNAL(activated()), m_multipleView, TQT_SLOT(slotRestart()));
        connect(quizRepeatErrors, TQT_SIGNAL(activated()), m_multipleView, TQT_SLOT(slotRepeat()));
        connect(this, TQT_SIGNAL(settingsChanged()), m_multipleView, TQT_SLOT(slotApplySettings()));

        setCentralWidget(m_multipleView);

        m_multipleView -> setQuiz(m_quiz);
        m_multipleView ->init();
        m_multipleView->show();
      }
      else
      {
        delete(m_quiz);
        m_quiz = 0;
      }
      break;
    case WQQuiz::qtQA:
      m_quiz = new WQQuiz(m_editView);
      connect(m_quiz, TQT_SIGNAL(checkingAnswer(int )), m_editView, TQT_SLOT(slotCheckedAnswer(int )));
      m_quiz ->setQuizType(WQQuiz::qtQA);
      m_quiz->setQuizMode(Prefs::mode());
      if (m_quiz -> init())
      {
        m_editView->saveCurrentSelection(true);
        m_editView->hide();
        m_qaView = new QAView(this);
        connect(quizCheck, TQT_SIGNAL(activated()), m_qaView, TQT_SLOT(slotCheck()));
        connect(qaHint, TQT_SIGNAL(activated()), m_qaView, TQT_SLOT(slotHint()));
        connect(quizRestart, TQT_SIGNAL(activated()), m_qaView, TQT_SLOT(slotRestart()));
        connect(quizRepeatErrors, TQT_SIGNAL(activated()), m_qaView, TQT_SLOT(slotRepeat()));
        connect(this, TQT_SIGNAL(settingsChanged()), m_qaView, TQT_SLOT(slotApplySettings()));

        setCentralWidget(m_qaView);

        m_qaView -> setQuiz(m_quiz);
        m_qaView ->init();
        m_qaView->show();
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
  connect(dialog, TQT_SIGNAL(settingsChanged()), this, TQT_SLOT(slotApplyPreferences()));
  dialog->show();
}

void KWordQuizApp::slotApplyPreferences()
{
  kdDebug() << "Prefs Update" << endl;
  editMarkBlank->setEnabled(Prefs::enableBlanks());
  editUnmarkBlank->setEnabled(Prefs::enableBlanks());
  m_editView->viewport()->tqrepaint(true);
  updateSpecialCharIcons();
  emit settingsChanged();
}

void KWordQuizApp::updateSpecialCharIcons( )
{
  for (int i = 0; i < 9; i++){
    KAction * act = actionCollection()->action(TQString("char_" + TQString::number(i + 1)).latin1());
    act->setIcon(charIcon(Prefs::specialCharacters()[i]));
    act->setToolTip(i18n("Inserts the character %1").arg(Prefs::specialCharacters()[i]));
  }
}

TQString KWordQuizApp::charIcon(const TQChar & c)
{
  ///Create a name and path for the icon
  TQString s = locateLocal("icon", "char" + TQString::number(c.tqunicode()) + ".png");

  ///No need to redraw if it already exists
  if (KStandardDirs::exists(s))
    return s;

  TQRect r(4, 4, 120, 120);

  ///A font to draw the character with
  TQFont font("sans");
  font.setPixelSize(100);
  font.setWeight(TQFont::Bold);

  ///Create the pixmap
  TQPixmap pm(128, 128);
  pm.fill(Qt::white);
  TQPainter p(&pm);
  p.setFont(font);
  p.setPen(Qt::blue);
  p.drawText(r, Qt::AlignCenter, (TQString) c);

  ///Create transparency tqmask
  TQBitmap bm(128, 128);
  bm.fill(Qt::color0);
  TQPainter b(&bm);
  b.setFont(font);
  b.setPen(Qt::color1);
  b.drawText(r, Qt::AlignCenter, (TQString) c);

  ///Mask the pixmap
  pm.setMask(bm);

  ///Save the icon to disk
  pm.save(s, "PNG");

  return s;
}

void KWordQuizApp::slotqStatusMsg(const TQString &text)
{
  statusBar()->clear();
  statusBar()->message(text);
}

/*!
    \fn KWordQuizApp::updateMode(int m)
 */
void KWordQuizApp::updateMode(int m)
{
  if (m_quiz != 0)
    if (KMessageBox::warningContinueCancel(this, i18n("This will restart your quiz. Do you wish to continue?"), TQString::null, KStdGuiItem::cont(), "askModeQuiz") != KMessageBox::Continue)
    {
      mode1->setChecked(Prefs::mode() == 1);
      mode2->setChecked(Prefs::mode() == 2);
      mode3->setChecked(Prefs::mode() == 3);
      mode4->setChecked(Prefs::mode() == 4);
      mode5->setChecked(Prefs::mode() == 5);
      return;
    }

  Prefs::setMode(m);
  TQString s1 = m_editView -> horizontalHeader()->label(0);
  TQString s2 = m_editView -> horizontalHeader()->label(1);

  mode1->setText(i18n("&1 %1 -> %2 In Order").arg(s1).arg(s2));
  mode2->setText(i18n("&2 %1 -> %2 In Order").arg(s2).arg(s1));
  mode3->setText(i18n("&3 %1 -> %2 Randomly").arg(s1).arg(s2));
  mode4->setText(i18n("&4 %1 -> %2 Randomly").arg(s2).arg(s1));
  mode5->setText(i18n("&5 %1 <-> %2 Randomly").arg(s1).arg(s2));

  mode1->setChecked(Prefs::mode() == 1);
  mode2->setChecked(Prefs::mode() == 2);
  mode3->setChecked(Prefs::mode() == 3);
  mode4->setChecked(Prefs::mode() == 4);
  mode5->setChecked(Prefs::mode() == 5);

  KPopupMenu *popup = mode->popupMenu();
  popup->setItemChecked(0, Prefs::mode() == 1);
  popup->setItemChecked(1, Prefs::mode() == 2);
  popup->setItemChecked(2, Prefs::mode() == 3);
  popup->setItemChecked(3, Prefs::mode() == 4);
  popup->setItemChecked(4, Prefs::mode() == 5);

  popup->changeItem(0, i18n("&1 %1 -> %2 In Order").arg(s1).arg(s2));
  popup->changeItem(1, i18n("&2 %1 -> %2 In Order").arg(s2).arg(s1));
  popup->changeItem(2, i18n("&3 %1 -> %2 Randomly").arg(s1).arg(s2));
  popup->changeItem(3, i18n("&4 %1 -> %2 Randomly").arg(s2).arg(s1));
  popup->changeItem(4, i18n("&5 %1 <-> %2 Randomly").arg(s1).arg(s2));

  TQString s;
  mode->setIcon("mode" + s.setNum(Prefs::mode()));

  switch( Prefs::mode() ){
  case 1:
    statusBar()->changeItem(i18n("%1 -> %2 In Order").arg(s1).arg(s2), ID_STATUS_MSG_MODE);
    break;
  case 2:
    statusBar()->changeItem(i18n("%1 -> %2 In Order").arg(s2).arg(s1), ID_STATUS_MSG_MODE);
    break;
  case 3:
    statusBar()->changeItem(i18n("%1 -> %2 Randomly").arg(s1).arg(s2), ID_STATUS_MSG_MODE);
    break;
  case 4:
    statusBar()->changeItem(i18n("%1 -> %2 Randomly").arg(s2).arg(s1), ID_STATUS_MSG_MODE);
    break;
  case 5:
    statusBar()->changeItem(i18n("%1 <-> %2 Randomly").arg(s1).arg(s2), ID_STATUS_MSG_MODE);
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
    if (centralWidget() == m_editView)
      m_editView->slotSpecChar(Prefs::specialCharacters()[i - 1]);
}

void KWordQuizApp::slotActionHighlighted( KAction * action, bool hl)
{
  if (!hl)
    slotqStatusMsg(i18n("Ready"));
}

void KWordQuizApp::slotContextMenuRequested(int row, int col, const TQPoint & pos)
{
  TQWidget *w = factory()->container("editor_popup", this);
  TQPopupMenu *popup = static_cast<TQPopupMenu *>(w);
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

#include "kwordquiz.moc"
