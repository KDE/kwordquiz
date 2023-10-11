/*
 * SPDX-FileCopyrightText: 2020 Jonah Brüchert <jbb@kaidan.im>
 * SPDX-FileCopyrightText: 2020-2021 Devin Lin <espidev@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "audioprober.h"

AudioProber::AudioProber(QObject *parent)
    : QObject{parent}
{
}

void AudioProber::setSource(QObject *source)
{
}

void AudioProber::processVolumeBar()
{
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

#include "moc_audioprober.cpp"
