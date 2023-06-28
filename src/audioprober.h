/*
 * SPDX-FileCopyrightText: 2020 Jonah Brüchert <jbb@kaidan.im>
 * SPDX-FileCopyrightText: 2020-2021 Devin Lin <devin@kde.org>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

#include <QMediaPlayer>
#include <QObject>
#include <QTimer>
#include <QVariant>

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
#include <QAudioProbe>

class AudioProber : public QAudioProbe
#else
class AudioProber : public QObject
#endif
{
    Q_OBJECT
    Q_PROPERTY(QVariantList volumesList READ volumesList NOTIFY volumesListChanged)
    Q_PROPERTY(int animationIndex READ animationIndex NOTIFY animationIndexChanged)
    Q_PROPERTY(int maxVolumes READ maxVolumes WRITE setMaxVolumes NOTIFY maxVolumesChanged)

public:
    AudioProber(QObject *parent = nullptr);

    Q_INVOKABLE void setSource(QObject *source);

    void process(QAudioBuffer buffer);
    void processVolumeBar();

    QVariantList volumesList() const;

    int maxVolumes();
    void setMaxVolumes(int m);

    int animationIndex();

private:
    void handlePlayerState(QMediaPlayer::State state);

    int m_audioSum = 0; //
    int m_audioLen = 0; // used for calculating the value of one volume bar from many
    int m_animationIndex = 0; // which index rectangle is being expanded
    int m_maxVolumes = 100; // based on width of screen

    QVariantList m_volumesList;

    QTimer *volumeBarTimer;
    QMediaPlayer *m_playerSource;

Q_SIGNALS:
    void volumesListAdded(int volume);
    void volumesListChanged();
    void animationIndexChanged();
    void maxVolumesChanged();
    void volumesListCleared();
};
