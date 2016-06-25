/*
    This file is part of KWordQuiz
    Copyright (C) 2010 Peter Hedlund <peter.hedlund@kdemail.net>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

*/

#include "kwqquizview.h"

#include <QAction>

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
