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

#ifndef KWQQUIZVIEW_H
#define KWQQUIZVIEW_H

#include <QUrl>
#include <QWidget>
#include <phonon/MediaObject>
#include <phonon/Path>
#include <phonon/AudioOutput>
#include <phonon/Global>

#include <KActionCollection>

class KWQQuizModel;

/**
@author Peter Hedlund
*/

class KWQQuizView : public QWidget
{
  Q_OBJECT
  public:
    explicit KWQQuizView(QWidget *parent, KActionCollection * actionCollection);

    void setQuiz(KWQQuizModel *quiz);
    virtual void init() = 0;

  public slots:
    virtual void slotCheck() = 0;
    virtual void slotApplySettings() = 0;
    void slotRepeat();
    void slotRestart();
    void slotAudioPlay();

  protected:
    KWQQuizModel *m_quiz;
    KActionCollection *m_actionCollection;

    void audioPlayAnswer();
    void audioPlayQuestion();
    void audioPlayFile(const QUrl &soundFile, bool);

  private:
    class Phonon::MediaObject *m_player;

};

#endif // KWQQUIZVIEW_H
