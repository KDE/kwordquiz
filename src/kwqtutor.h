/*
    kwqtutor.h
    -------------------

    SPDX-FileCopyrightText: 2010 Peter Hedlund
    email                : peter.hedlund@kdemail.net

*/

/***************************************************************************
 *                                                                         *
 *   SPDX-License-Identifier: GPL-2.0-or-later
 *                                                                         *
 ***************************************************************************/

#ifndef KWQTUTOR_H
#define KWQTUTOR_H

#include <KRandomSequence>
#include <KActionCollection>
#include <KStatusNotifierItem>

#include "keduvocdocument.h"
#include "kwqtutorflashcard.h"

class KWQTutor : public KStatusNotifierItem
{
  Q_OBJECT

  public:
    /* init systray icon, meny, actions */
    explicit KWQTutor (const QUrl &fileToOpen, QWidget *parent = nullptr);

  public Q_SLOTS:
    /* self-calling function: it sets the time interval,calls itself and call func to display flashcard*/
    void updateTimer();
    /*open keduvocdocument from the menu */
    void loadFile();
    /* show the Preferences menu and save changes on closing */
    void showPrefMenu();
    /* starts or stops the exercise */
    void startStopPressed ();
    /*displays the flashcard */
    void showFlashcard(int numberShowing);
    /* shows the reverse side of the flashcard being displayed */
    void flipFlashcard();
    /* closes the flashcard*/
    void closeFlashcard();
    /* save preferences before exit */
    void quit();
    /* apply any new preferences */
    void slotApplyPreferences();

  private:
    /* random number generator*/
    KRandomSequence* m_randomSequence;
    /* an entry of KEduVocDocument which is currently being displayed in flashcard */
    int m_currentEntry;
    int m_currentColumn;
    /* the actual popup flashcard*/
    KWQTutorFlashCard* m_flashcard;
    /* the timer which launches flashcards after certain intervals */
    QTimer * m_timer;
    /* the vocabulary doc being opened */
    KEduVocDocument* m_tutorDoc;
    /* collection of QAction that holds Global Shortcuts */
    KActionCollection* m_globalCollection;
    /* tutor start action */
    QAction* m_tutorStartAction;
    /* a flag to know whether the tutor exercise is runnig */
    bool m_isRunning;
};

#endif // KWQTUTOR_H
