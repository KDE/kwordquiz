// SPDX-FileCopyrightText: 2020 Jonah Brüchert <jbb@kaidan.im>
// SPDX-FileCopyrightText: 2020-2022 Devin Lin <espidev@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls as Controls
import org.kde.kirigami as Kirigami

Item {
    id: visualization

    property var prober

    property int maxBarHeight
    property int animationIndex // which index rectangle is being expanded
    property var volumes: []

    property int reservedBarWidth: Math.round(Kirigami.Units.gridUnit * 0.4)

    // maximum volume to base off volume bar height
    property int maxVolumeData: 1000

    // TODO: We need a more sophisticated algorithm to create the visualizer bars (probably using FFT)
    // currently we just see how much data is in a sample, which is really random and doesn't deal with spikes very well
    function processVolume(volume) {
        if (audio.metaData.audioCodec === 'Vorbis') {
            volume = Math.max(0, 17000 - volume);
        }
        return volume;
    }

    Component.onCompleted: {
        visualization.prober.maxVolumes = width / reservedBarWidth;
    }
    onWidthChanged: {
        visualization.prober.maxVolumes = width / reservedBarWidth;
    }

    Connections {
        target: visualization.prober
        function onVolumesListCleared() {
            visualization.maxVolumeData = 1000; // reset max
        }
        function onVolumesListAdded(volume) {
            visualization.maxVolumeData = Math.max(visualization.maxVolumeData, processVolume(volume));
        }
    }

    ListView {
        id: list
        model: visualization.volumes
        orientation: Qt.Horizontal
        
        interactive: false
        height: maxBarHeight
        anchors {
            verticalCenter: parent.verticalCenter
            left: parent.left
            leftMargin: 0
            right: parent.right
        }
        spacing: 0
        
        delegate: Item {
            width: reservedBarWidth
            height: list.height
        
            Rectangle {
                color: Kirigami.Theme.disabledTextColor
                width: Math.round(Kirigami.Units.gridUnit * 0.12)
                radius: Math.round(width / 2)
                height: Math.max(Math.round(Kirigami.Units.gridUnit * 0.15), maxBarHeight * processVolume(modelData) / visualization.maxVolumeData)
                antialiasing: true
                anchors.verticalCenter: parent.verticalCenter
            }
        }
    }
}
