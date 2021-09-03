/*
    This file is part of KWordQuiz
    SPDX-FileCopyrightText: 2010 Peter Hedlund <peter.hedlund@kdemail.net>

    SPDX-License-Identifier: GPL-2.0-or-later

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

  public Q_SLOTS:
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
