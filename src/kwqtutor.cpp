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

#include <QAction>
#include <QFileDialog>
#include <QMenu>
#include <QPointer>
#include <QTimer>

#include <KConfigGroup>
#include <KLocalizedString>
#include <KMessageBox>
#include <KStandardAction>

#include "keduvoclesson.h"
#include "keduvocexpression.h"
#include "kwqtutorprefs.h"
#include "prefs.h"

KWQTutor::KWQTutor(const QUrl &fileToOpen, QWidget *parent) : KStatusNotifierItem(parent)
{
  KMessageBox::information(0, i18n("<qt>KWordQuiz Tutor displays flashcards on your screen in a way that allows "
                           "you to set a certain time interval at which flashcards will pop up.<br /><br />"
                           "The flashcards pop up in a non-intrusive way allowing you to carry on working "
                           "without stealing the focus from other programs you might be working with.<br /><br />"
                           "Be sure to right-click KWordQuiz Tutor icon on the panel and select Preferences. "
                           "There you can assign keyboard shortcuts.</qt>"),
                           i18n("Tutor"), QStringLiteral("firstTutorRun"));

  m_isRunning = false;

  setIconByName(QStringLiteral("kwordquiz"));
  connect (this, SIGNAL(quitSelected()),this, SLOT(quit()));

  QAction * a;
  QMenu * menu = new QMenu();

  m_tutorStartAction = new QAction(menu);
  m_tutorStartAction->setText(i18n("Start Exercise"));
  m_tutorStartAction->setIcon(QIcon::fromTheme(QStringLiteral("media-playback-start")));
  connect (m_tutorStartAction, &QAction::triggered, this,  &KWQTutor::startStopPressed);
  addAction(QStringLiteral("tutor_start"), m_tutorStartAction);
  menu->addAction(m_tutorStartAction);

  a = KStandardAction::open(this, SLOT(loadFile()), this);
  menu->addAction(a);

  a = KStandardAction::preferences(this, SLOT(showPrefMenu()), this);
  a->setText(i18n("Configure KWordQuiz Tutor..."));
  menu->addAction(a);

  m_globalCollection = new KActionCollection(this);
  a = m_globalCollection->addAction(QStringLiteral("kwq_close_flash_card"));
  a->setText(i18n("Close Flash Card"));
  //a->setGlobalShortcut(KShortcut(Qt::CTRL | Qt::ALT | Qt::Key_J));
  connect (a, &QAction::triggered, this, &KWQTutor::closeFlashcard);

  a = m_globalCollection->addAction(QStringLiteral("kwq_flip_flash_card"));
  a->setText(i18n("Flip Flash Card"));
  //a->setGlobalShortcut(KShortcut(Qt::CTRL | Qt::ALT | Qt::Key_H));
  connect (a, &QAction::triggered, this, &KWQTutor::flipFlashcard);

  m_timer = new QTimer;
  connect (m_timer, &QTimer::timeout, this, &KWQTutor::updateTimer);

  m_randomSequence = new KRandomSequence();
  m_flashcard = new KWQTutorFlashCard();

  m_tutorDoc = new KEduVocDocument();
  if (fileToOpen.isEmpty())
    m_tutorDoc->open(QUrl(Prefs::lastVocabFile()));
  else {
    m_tutorDoc->open(fileToOpen);
    Prefs::setLastVocabFile(fileToOpen.toLocalFile());
  }
  setContextMenu(menu);

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
  qDebug() << interval;
  m_timer->start(1000 * 60 * interval);
  if (!m_flashcard->isVisible())
    showFlashcard(m_randomSequence->getLong(m_tutorDoc->lesson()->entryCount(KEduVocLesson::Recursive) - 1));
}

void KWQTutor::startStopPressed()
{
  if (!m_isRunning && m_tutorDoc->lesson()->entryCount(KEduVocLesson::Recursive))  //make sure the file's not empty
  {
    m_isRunning = true;
    m_tutorStartAction->setText(i18n("Stop Exercise"));
    m_tutorStartAction->setIcon(QIcon::fromTheme(QStringLiteral("media-playback-stop")));
    updateTimer();
    return;
  }
  if (m_isRunning)
  {
    m_timer->stop();
    m_tutorStartAction->setText(i18n("Start Exercise"));
    m_tutorStartAction->setIcon(QIcon::fromTheme(QStringLiteral("media-playback-start")));
    m_isRunning = false;
    m_flashcard->setVisible(false);
    Prefs::setTutorFlashCardGeometry(m_flashcard->geometry());
  }
}

void KWQTutor::loadFile()
{
  QString filter = KEduVocDocument::pattern(KEduVocDocument::Reading);
  QPointer<QFileDialog> fd = new QFileDialog(nullptr, i18n("Open Vocabulary Document"), QString(), filter);
  fd->setFileMode(QFileDialog::ExistingFiles);
  if (fd->exec() == QDialog::Accepted)
  {
    m_tutorDoc->open(fd->selectedUrls().at(0));
    Prefs::setLastVocabFile(fd->selectedUrls().at(0).toLocalFile());
    if (Prefs::startExerciseAsSoonAsFileIsLoaded())
      startStopPressed();
  }
  delete fd;
}

void KWQTutor::showPrefMenu()
{
  if (KWQTutorPrefs::showDialog(QStringLiteral("settings")))
    return;

  //KConfigDialog didn't find an instance of this dialog, so lets create it :
  KWQTutorPrefs* dialog = new KWQTutorPrefs(0, QStringLiteral("settings"),  Prefs::self(), m_globalCollection);
  connect(dialog, &KConfigDialog::settingsChanged, this, &KWQTutor::slotApplyPreferences);
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
  Prefs::self()->save();
}
