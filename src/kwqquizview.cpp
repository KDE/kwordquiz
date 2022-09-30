/*
    SPDX-FileCopyrightText: 2010 Peter Hedlund <peter.hedlund@kdemail.net>
    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "kwqquizview.h"


#include "kwqquizmodel.h"

KWQQuizView::KWQQuizView(QWidget* parent, KActionCollection* actionCollection) :  QWidget(parent), m_actionCollection(actionCollection)
{
  m_player = 0;
  m_quiz = 0;
}

void KWQQuizView::setQuiz(KWQQuizModel* quiz)
{
  m_quiz = quiz;
}

void KWQQuizView::slotRestart()
{
  m_quiz->activateBaseList();
  init();
}

void KWQQuizView::slotRepeat()
{
  m_quiz->activateErrorList();
  init();
}

void KWQQuizView::slotAudioPlay()
{
    // repeat playing last file
    audioPlayFile(QUrl(), false);
}

void KWQQuizView::audioPlayAnswer()
{
    audioPlayFile(m_quiz->soundAnswer(), true);
}

void KWQQuizView::audioPlayQuestion()
{
    audioPlayFile(m_quiz->soundQuestion(), true);
}

void KWQQuizView::audioPlayFile(const QUrl &soundUrl, bool overwrite)
{
    static QUrl lastUrl;

    QUrl url = soundUrl;
    if (overwrite)
       lastUrl = soundUrl;

    if (url.isEmpty()) {
        if (lastUrl.isEmpty()) {
            m_actionCollection->action(QStringLiteral("quiz_audio_play"))->setEnabled(false);
            return;
        }
        url = lastUrl;
    } else {
        lastUrl = url;
    }
    m_actionCollection->action(QStringLiteral("quiz_audio_play"))->setEnabled(true);

    qDebug() << "Attempting to play sound: " << url;

    if (!m_player) {
        m_player = new Phonon::MediaObject(this);
        Phonon::AudioOutput *audioOutput = new Phonon::AudioOutput(Phonon::NoCategory, this);
        createPath(m_player, audioOutput);
    } else {
        m_player->stop();
    }
    m_player->setCurrentSource(url);
    m_player->play();
}
