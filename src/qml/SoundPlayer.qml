// SPDX-FileCopyrightText: 2023 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: LGPL-2.0-or-later

import QtMultimedia 5.15
import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15 as QQC2
import org.kde.kirigami 2.20 as Kirigami
import org.kde.kwordquiz 1.0

QQC2.AbstractButton {
    id: root

    property alias source: audio.source

    function play() {
        audio.play();
    }

    onClicked: if (audio.playbackState === Audio.PlayingState) {
        audio.stop();
    } else {
        audio.play();
    }

    leftPadding: Kirigami.Units.largeSpacing
    rightPadding: Kirigami.Units.largeSpacing

    Audio {
        id: audio
        source: root.progressInfo.localPath
        autoLoad: false
    }

    contentItem: RowLayout {
        Kirigami.Icon {
            source: audio.playbackState === Audio.PlayingState ? "media-playback-pause" : "media-playback-start"
            Layout.preferredWidth: Kirigami.Units.iconSizes.mediumLarge
            Layout.preferredHeight: Kirigami.Units.iconSizes.mediumLarge
        }

        Visualization {
            id: visualization

            Layout.fillWidth: true
            implicitHeight: Kirigami.Units.gridUnit * 3

            prober: AudioProber {
                Component.onCompleted: setSource(audio)
            }

            maxBarHeight: Kirigami.Units.gridUnit * 3
            animationIndex: prober.animationIndex
            volumes: prober.volumesList
        }
    }

    background: Item {
        Kirigami.ShadowedRectangle {
            id: bubbleBackground
            anchors.fill: parent
            Kirigami.Theme.colorSet: Kirigami.Theme.View
            Kirigami.Theme.inherit: false
            color: {
                if (root.hovered) {
                    return Kirigami.ColorUtils.tintWithAlpha(Kirigami.Theme.backgroundColor, Kirigami.Theme.highlightColor, 0.15)
                } else {
                    return Kirigami.Theme.backgroundColor
                }
            }
            radius: Kirigami.Units.largeSpacing
            shadow {
                size: Kirigami.Units.smallSpacing
                color: Qt.rgba(Kirigami.Theme.textColor.r, Kirigami.Theme.textColor.g, Kirigami.Theme.textColor.b, 0.10)
            }

            Behavior on color {
                ColorAnimation {target: bubbleBackground; duration: Kirigami.Units.veryLongDuration; easing.type: Easing.InOutCubic}
            }
        }
    }
}
