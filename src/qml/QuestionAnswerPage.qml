// SPDX-FileCopyrightText: 2023 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: LGPL-2.0-or-later

import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15 as QQC2
import org.kde.kirigami 2.20 as Kirigami
import org.kde.kwordquiz 1.0

BasePage {
    id: root

    property bool wasCorrect: true
    property bool hintGiven: false

    actions.contextualActions: [
        Kirigami.Action {
            id: hintAction
            text: i18nc("@action:button", "Hint")
            onTriggered: {
                root.hintGiven = true;
                listView.currentItem.giveHint();
            }
        },
        Kirigami.Action {
            id: checkAction

            text: i18nc("@action:button", "Check")
            onTriggered: {
                listView.currentItem.check();
                root.showAnswer = true;
            }
            enabled: listView.currentItem && listView.currentItem.hasSelection
            visible: !root.showAnswer && !root.finished
        },
        Kirigami.Action {
            id: nextAction

            icon.name: "go-next"
            text: i18nc("@action:button", "Next")
            visible: root.showAnswer
            onTriggered: {
                root.showAnswer = false;
                if (listView.currentIndex + 1 === listView.count) {
                    root.finished = true;
                } else {
                    listView.incrementCurrentIndex();
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

            Kirigami.Heading {
                level: 3
                visible: root.showAnswer
                text: root.wasCorrect ? i18n("This is correct") : i18n("Wrong, the correct answer was \"%1\".", wordDelegate.answer)

                horizontalAlignment: Text.AlignHCenter

                Layout.fillWidth: true
            }
        }
    }
}
