// SPDX-FileCopyrightText: 2023 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: LGPL-2.0-or-later

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls as QQC2
import org.kde.kirigami as Kirigami
import org.kde.kwordquiz

BasePage {
    id: root

    property bool wasCorrect: true
    property bool hintGiven: false

    listView.delegate: Rectangle {
        id: wordDelegate

        required property string question
        required property string questionImage
        required property string questionSound
        required property string answer
        required property string answerImage
        required property string answerSound

        readonly property bool isCurrentItem: ListView.isCurrentItem
        readonly property bool hasSelection: answerField.text.length > 0

        onIsCurrentItemChanged: if (!isCurrentItem) {
            answerField.text = '';
        } else {
            answerField.forceActiveFocus();
        }

        function check() {
            let correctAnswer = blankAnswer.hasBlank ? blankAnswer.correctAnswer.trim() : answer.trim();
            let givenAnswer = answerField.text.trim();

            if (!Prefs.caseSensitiveAnswers) {
                correctAnswer = correctAnswer.toLowerCase();
                givenAnswer = givenAnswer.toLowerCase();
            }

            if (correctAnswer === givenAnswer && (!root.hintGiven || !Prefs.hintError)) {
                root.wasCorrect = true;
                randomSortModel.unMarkAsError(listView.currentIndex);
            } else {
                root.wasCorrect = correctAnswer === givenAnswer;
                randomSortModel.markAsError(listView.currentIndex);
                root.errors++;
            }
            root.hintGiven = false;
        }

        function giveHint() {
            const currentAnswer = answerField.text;
            const correctAnswer = blankAnswer.hasBlank ? blankAnswer.correctAnswer.trim() : answer.trim();

            for (let i = 0; i < correctAnswer.length; i++) {
                if (i < currentAnswer.length && currentAnswer[i] === correctAnswer[i]) {
                    continue;
                }

                answerField.text = correctAnswer.substring(0, i + 1);
                break;
            }
        }

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
                source: wordDelegate.questionImage !== '' ? 'file:' + wordDelegate.questionImage : ''
            }

            Loader {
                active: wordDelegate.questionSound

                Layout.fillWidth: true
                Layout.maximumWidth: Kirigami.Units.gridUnit * 10
                Layout.alignment: Qt.AlignHCenter

                sourceComponent: SoundPlayer {
                    source: wordDelegate.questionSound !== '' ? 'file:' + wordDelegate.questionSound : ''
                }
            }

            Kirigami.Heading {
                text: wordDelegate.question.replace(/\[(.*?)\]/, '$1')
                wrapMode: Text.Wrap
                horizontalAlignment: Text.AlignHCenter

                Layout.fillWidth: true
            }

            Kirigami.Separator {
                Layout.fillWidth: true
            }

            BlankAnswer {
                id: blankAnswer
                input: wordDelegate.answer
            }

            Kirigami.Heading {
                visible: blankAnswer.hasBlank
                text: blankAnswer.blankedAnswer
                wrapMode: Text.Wrap
                horizontalAlignment: Text.AlignHCenter

                Layout.fillWidth: true
            }

            QQC2.TextField {
                id: answerField

                Layout.alignment: Qt.AlignHCenter
                onAccepted: if (root.showAnswer) {
                    nextAction.trigger();
                } else {
                    checkAction.trigger();
                }
            }

            RowLayout {
                Layout.alignment: Qt.AlignHCenter
                visible: !root.showAnswer && !root.finished

                QQC2.Button {
                    text: i18nc("@action:button", "Hint")
                    onClicked: {
                        root.hintGiven = true;
                        listView.currentItem.giveHint();
                    }
                }

                QQC2.Button {
                    text: i18nc("@action:button", "Check")
                    onClicked: {
                        listView.currentItem.check();
                        root.showAnswer = true;
                    }
                    enabled: listView.currentItem && listView.currentItem.hasSelection
                }
            }


            Kirigami.Heading {
                level: 3
                visible: root.showAnswer
                text: root.wasCorrect ? i18n("This is correct") : i18n("Wrong, the correct answer was \"%1\".", wordDelegate.answer)

                horizontalAlignment: Text.AlignHCenter

                Layout.fillWidth: true
            }

            Image {
                Layout.fillWidth: true
                Layout.maximumHeight: Kirigami.Units.gridUnit * 8

                fillMode: Image.PreserveAspectFit
                visible: root.showAnswer
                smooth: true
                source: wordDelegate.answerImage !== '' ? 'file:' + wordDelegate.answerImage : ''
            }

            Loader {
                active: wordDelegate.answerSound && root.showAnswer

                Layout.fillWidth: true
                Layout.maximumWidth: Kirigami.Units.gridUnit * 10
                Layout.alignment: Qt.AlignHCenter

                sourceComponent: SoundPlayer {
                    source: wordDelegate.answerSound !== '' ? 'file:' + wordDelegate.answerSound : ''
                }
            }

            QQC2.Button {
                icon.name: "go-next"
                text: i18nc("@action:button", "Next")
                visible: root.showAnswer
                onClicked: {
                    root.showAnswer = false;
                    if (listView.currentIndex + 1 === listView.count) {
                        root.finished = true;
                    } else {
                        listView.incrementCurrentIndex();
                    }
                }

                Layout.alignment: Qt.AlignHCenter
            }
        }
    }
}
