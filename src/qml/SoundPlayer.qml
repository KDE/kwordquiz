// SPDX-FileCopyrightText: 2023 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: LGPL-2.0-or-later

import QtMultimedia
import QtQuick
import QtQuick.Layouts
import QtQuick.Controls as QQC2
import org.kde.kirigami as Kirigami
import org.kde.kwordquiz

QQC2.AbstractButton {
    id: root

    property alias source: audio.source

    function play() {
        audio.play();
    }

    onClicked: if (audio.playbackState === MediaPlayer.PlayingState) {
        audio.pause();
    } else {
        audio.play();
    }

    leftPadding: Kirigami.Units.largeSpacing
    rightPadding: Kirigami.Units.largeSpacing

    MediaPlayer {
        id: audio
        source: root.progressInfo.localPath
        audioOutput: AudioOutput {}
    }

    contentItem: RowLayout {
        Kirigami.Icon {
            source: audio.playbackState === MediaPlayer.PlayingState ? "media-playback-pause" : "media-playback-start"
            Layout.preferredWidth: Kirigami.Units.iconSizes.medium
            Layout.preferredHeight: Kirigami.Units.iconSizes.medium
        }

        QQC2.Slider {
            Layout.fillWidth: true
            from: 0.0
            to: 1.0
            enabled: audio.seekable
            value: audio.position / audio.duration
            onMoved: audio.setPosition(value * audio.duration)
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
