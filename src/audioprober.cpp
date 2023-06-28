/*
 * SPDX-FileCopyrightText: 2020 Jonah Brüchert <jbb@kaidan.im>
 * SPDX-FileCopyrightText: 2020-2021 Devin Lin <espidev@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "audioprober.h"
#include <qmediaobject.h>

AudioProber::AudioProber(QObject *parent)
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    : QAudioProbe{parent}
#else
    : QObject{parent}
#endif
{
}

void AudioProber::setSource(QObject *source)
{
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    auto player = qvariant_cast<QMediaPlayer *>(source->property("mediaObject"));

    m_playerSource = player;
    QAudioProbe::setSource(player);

    connect(this, &AudioProber::audioBufferProbed, this, &AudioProber::process);

    // connect to player
    connect(m_playerSource, &QMediaPlayer::stateChanged, this, &AudioProber::handlePlayerState);

    // loop to add volume bars
    volumeBarTimer = new QTimer(this);
    connect(volumeBarTimer, &QTimer::timeout, this, &AudioProber::processVolumeBar);
    volumeBarTimer->setInterval(150);
#endif
}

void AudioProber::handlePlayerState(QMediaPlayer::State state)
{
    if (state == QMediaPlayer::PlayingState) {
        volumeBarTimer->start();
    } else if (state == QMediaPlayer::PausedState) {
        volumeBarTimer->stop();
    } else if (state == QMediaPlayer::StoppedState) {
        volumeBarTimer->stop();
    }
}

void AudioProber::processVolumeBar()
{
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    if (isActive()) {
        // m_audioLen might be 0
        const int val = m_audioLen == 0 ? 0 : m_audioSum / m_audioLen;

        m_volumesList.append(val);
        Q_EMIT volumesListAdded(val);

        if (m_volumesList.count() > m_maxVolumes) {
            m_volumesList.removeFirst();
        }

        Q_EMIT volumesListChanged();

        // index of rectangle to animate
        if (m_volumesList.count() != 0) {
            m_animationIndex = m_volumesList.count();
            Q_EMIT animationIndexChanged();
        }

        m_audioSum = 0;
        m_audioLen = 0;
    }
#endif
}

void AudioProber::process(QAudioBuffer buffer)
{
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    int sum = 0;
    for (int i = 0; i < buffer.sampleCount(); i++) {
        sum += abs(static_cast<short *>(buffer.data())[i]);
    }

    sum /= buffer.sampleCount();

    m_audioSum += sum;
    m_audioLen++;
#endif
}

QVariantList AudioProber::volumesList() const
{
    return m_volumesList;
}

int AudioProber::maxVolumes()
{
    return m_maxVolumes;
}

void AudioProber::setMaxVolumes(int m)
{
    m_maxVolumes = m;
    Q_EMIT maxVolumesChanged();
}

int AudioProber::animationIndex()
{
    return m_animationIndex;
}
