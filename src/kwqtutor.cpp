/***************************************************************************
                                 kwqtutor.cpp
                             -------------------

    copyright            : (C) 2010 by Peter Hedlund
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

#include "kwqtutor.h"

#include <QtCore/QPointer>
#include <QtCore/QTimer>
#include <QtGui/QMenu>

#include <KAction>
#include <KMessageBox>
#include <KStandardAction>
#include <KFileDialog>
#include <KLocale>

#include "keduvocdocument.h"
#include "keduvoclesson.h"
#include "keduvocexpression.h"
#include "kwqtutorflashcard.h"
#include "kwqtutorprefs.h"
#include "prefs.h"

KWQTutor::KWQTutor(KUrl fileToOpen, QWidget *parent) : KSystemTrayIcon(parent)
{
  KMessageBox::information(0, i18n("<qt>KWordQuiz Tutor displays flashcards on your screen in a way that allows "
                           "you to set a certain time interval at which flashcards will pop up.<br /><br />"
                           "The flashcards pop up in a non-intrusive way allowing you to carry on working "
                           "without stealing the focus from other programs you might be working with.<br /><br />"
                           "Be sure to right-click KWordQuiz Tutor icon on the panel and select Preferences. "
                           "There you can assign keyboard shortcuts.</qt>"),
                           i18n("Tutor"), QString("firstTutorRun"));

  m_isRunning = false;

  setIcon(KIcon(KIconLoader::global()->iconPath("kwordquiz", KIconLoader::Application)));
  connect (this, SIGNAL(quitSelected()),this, SLOT(quit()));

  KAction* a;

  a = actionCollection()->addAction("tutor_start");
  a->setText(i18n("Start Exercise"));
  a->setIcon(KIcon("media-playback-start"));
  connect (a, SIGNAL(triggered()), this,  SLOT (startStopPressed()));
  contextMenu()->addAction(a);

  a = KStandardAction::open(this, SLOT(loadFile()), actionCollection());
  contextMenu()->addAction(a);

  a = KStandardAction::preferences(this, SLOT(showPrefMenu()), actionCollection());
  a->setText(i18n("Configure KWordQuiz Tutor..."));
  contextMenu()->addAction(a);

  m_globalCollection = new KActionCollection(this);
  a = m_globalCollection->addAction("kwq_close_flash_card");
  a->setText(i18n("Close Flash Card"));
  a->setGlobalShortcut(KShortcut(Qt::CTRL + Qt::ALT + Qt::Key_J));
  connect (a, SIGNAL(triggered()), this, SLOT(closeFlashcard()));

  a = m_globalCollection->addAction("kwq_flip_flash_card");
  a->setText(i18n("Flip Flash Card"));
  a->setGlobalShortcut(KShortcut(Qt::CTRL + Qt::ALT + Qt::Key_H));
  connect (a, SIGNAL(triggered()), this, SLOT(flipFlashcard()));

  m_timer = new QTimer;
  connect (m_timer, SIGNAL(timeout()), this, SLOT(updateTimer()));

  m_randomSequence = new KRandomSequence();
  m_flashcard = new KWQTutorFlashCard();

  m_tutorDoc = new KEduVocDocument();
  if (fileToOpen.isEmpty())
    m_tutorDoc->open(KUrl(Prefs::lastVocabFile()));
  else {
    m_tutorDoc->open(fileToOpen);
    Prefs::setLastVocabFile(fileToOpen.pathOrUrl());
  }

  show();

  if (Prefs::startExerciseAsSoonAsFileIsLoaded() && !fileToOpen.isEmpty()) //starting immediately?
    startStopPressed();
}

void KWQTutor::updateTimer()
{
  int interval = 0;
  if (Prefs::tutorTiming() == Prefs::EnumTutorTiming::FixedInterval)
    interval = Prefs::tutorEvery();
  else if (Prefs::tutorTiming() == Prefs::EnumTutorTiming::RandomInterval) {
    do 
      interval = m_randomSequence->getLong(Prefs::tutorMax());
    while (interval < Prefs::tutorMin());
  }
  kDebug() << interval;
  m_timer->start(1000 * 60 * interval);
  if (!m_flashcard->isVisible())
    showFlashcard(m_randomSequence->getLong(m_tutorDoc->lesson()->entryCount(KEduVocLesson::Recursive) - 1));
}

void KWQTutor::startStopPressed()
{
  if (!m_isRunning && m_tutorDoc->lesson()->entryCount(KEduVocLesson::Recursive))  //make sure the file's not empty
  {
    m_isRunning = true;
    actionCollection()->action("tutor_start")->setText(i18n("Stop Exercise"));
    actionCollection()->action("tutor_start")->setIcon(KIcon("media-playback-stop"));
    updateTimer();
    return;
  }
  if (m_isRunning)
  {
    m_timer->stop();
    actionCollection()->action("tutor_start")->setText(i18n("Start Exercise"));
    actionCollection()->action("tutor_start")->setIcon(KIcon("media-playback-start"));
    m_isRunning = false;
    m_flashcard->setVisible(false);
    Prefs::setTutorFlashCardGeometry(m_flashcard->geometry());
  }
}

void KWQTutor::loadFile()
{
  QString filter = KEduVocDocument::pattern(KEduVocDocument::Reading);
  QPointer<KFileDialog> fd = new KFileDialog(KUrl(), filter, 0);
  fd->setOperationMode(KFileDialog::Opening);
  fd->setMode(KFile::File | KFile::ExistingOnly);
  fd->setCaption(i18n("Open Vocabulary Document"));
  if (fd->exec() == QDialog::Accepted)
  {
    m_tutorDoc->open(fd->selectedUrl());
    Prefs::setLastVocabFile(fd->selectedUrl().pathOrUrl());
    if (Prefs::startExerciseAsSoonAsFileIsLoaded())
      startStopPressed();
  }
  delete fd;
}

void KWQTutor::showPrefMenu()
{
  if (KWQTutorPrefs::showDialog("settings"))
    return;

  //KConfigDialog didn't find an instance of this dialog, so lets create it :
  KWQTutorPrefs* dialog = new KWQTutorPrefs(0, "settings",  Prefs::self(), m_globalCollection);
  connect(dialog, SIGNAL(settingsChanged(QString)), this, SLOT(slotApplyPreferences()));
  dialog->show();
}

void KWQTutor::slotApplyPreferences()
{
  m_flashcard->init();
}

void KWQTutor::flipFlashcard()
{
  m_currentColumn == 0 ? m_currentColumn = 1 : m_currentColumn = 0;
  QVariant entry (m_tutorDoc->lesson()->entry(m_currentEntry, KEduVocLesson::Recursive)->translation(m_currentColumn)->text());
  m_flashcard->setText(entry.toString());
}

void KWQTutor::closeFlashcard()
{
  m_flashcard->close();
}

void KWQTutor::showFlashcard(int numberShowing)
{
  m_currentColumn = 0;
  QVariant entry (m_tutorDoc->lesson()->entry(numberShowing, KEduVocLesson::Recursive)->translation(m_currentColumn)->text());
  m_flashcard->setText((entry).toString());
  m_flashcard->setGeometry(Prefs::tutorFlashCardGeometry());
  m_flashcard->show();  
  m_currentEntry = numberShowing;
}

void KWQTutor::quit()
{
  Prefs::self()->writeConfig();
}

#include "kwqtutor.moc"
