// SPDX-FileCopyrightText: 2023 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: LGPL-2.0-or-later

import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15 as QQC2
import org.kde.kirigami 2.20 as Kirigami
import org.kde.kwordquiz 1.0

BasePage {
    id: root

    actions.contextualActions: [
        Kirigami.Action {
            id: checkAction
            text: i18nc("@action:button", "Check")
            onTriggered: root.showAnswer = true;
            visible: !root.showAnswer && !root.finished
        },
        Kirigami.Action {
            icon.name: "know"
            text: i18nc("@action:button", "Correct")
            visible: root.showAnswer
            onTriggered: {
                root.showAnswer = false;
                randomSortModel.unMarkAsError(listView.currentIndex);
                if (listView.currentIndex + 1 === listView.count) {
                    root.finished = true;
                } else {
                    listView.incrementCurrentIndex();
                }

                if (Prefs.autoFlip) {
                    timer.restart();
                }
            }
        },
        Kirigami.Action {
            text: i18nc("@action:button", "Not Correct")
            icon.name: "dontknow"
            visible: root.showAnswer
            onTriggered: {
                root.showAnswer = false;
                root.errors++;
                randomSortModel.markAsError(listView.currentIndex);
                if (listView.currentIndex + 1 === listView.count) {
                    root.finished = true;
                } else {
                    listView.incrementCurrentIndex();
                }

                if (Prefs.autoFlip) {
                    timer.restart();
                }
            }
        },
        OptionsAction {
            cardModel: root.cardModel
        },
        EditAction {
            cardModel: root.cardModel
            documentModel: root.documentModel
        }
    ]

    Timer {
        id: timer
        interval: Prefs.flipDelay * 1000
        onTriggered: checkAction.trigger()
        running: Prefs.autoFlip
    }

    listView.delegate: Rectangle {
        id: wordDelegate

        required property string question
        required property string questionSound
        required property string questionImage
        required property string answer
        required property string answerSound
        required property string answerImage

        width: ListView.view.width
        height: ListView.view.height

        color: Kirigami.Theme.backgroundColor

        visible: !root.finished

        ColumnLayout {
            anchors.centerIn: parent
            width: parent.width - Kirigami.Units.gridUnit * 4

            Image {
                Layout.fillWidth: true
                Layout.maximumHeight: Kirigami.Units.gridUnit * 8

                fillMode: Image.PreserveAspectFit
                smooth: true
                source: 'file:' + wordDelegate.questionImage
            }

            Loader {
                active: wordDelegate.questionSound

                Layout.fillWidth: true
                Layout.maximumWidth: Kirigami.Units.gridUnit * 10
                Layout.alignment: Qt.AlignHCenter

                sourceComponent: SoundPlayer {
                    source: 'file:' + wordDelegate.questionSound
                }
            }

            Kirigami.Heading {
                text: wordDelegate.question.replace(/\[(.*?)\]/, '$1')
                wrapMode: Text.Wrap
                horizontalAlignment: Text.AlignHCenter
                font.pixelSize: 30

                Layout.fillWidth: true
            }


            Kirigami.Separator {
                visible: root.showAnswer

                Layout.fillWidth: true
            }

            Image {
                Layout.fillWidth: true
                Layout.maximumHeight: Kirigami.Units.gridUnit * 8

                fillMode: Image.PreserveAspectFit
                visible: root.showAnswer
                smooth: true
                source: 'file:' + wordDelegate.answerImage
            }

            Loader {
                active: wordDelegate.questionSound && root.showAnswer

                Layout.fillWidth: true
                Layout.maximumWidth: Kirigami.Units.gridUnit * 10
                Layout.alignment: Qt.AlignHCenter

                sourceComponent: SoundPlayer {
                    source: 'file:' + wordDelegate.answerSound
                }
            }

            Kirigami.Heading {
                text: wordDelegate.answer.replace(/\[(.*?)\]/, '$1')
                wrapMode: Text.Wrap
                horizontalAlignment: Text.AlignHCenter
                font.pixelSize: 30
                visible: root.showAnswer

                Layout.fillWidth: true
            }
        }
    }
}
