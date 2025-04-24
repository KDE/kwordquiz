// SPDX-FileCopyrightText: 2023 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: LGPL-2.0-or-later

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls as QQC2
import org.kde.kirigami as Kirigami
import org.kde.kirigamiaddons.formcard as FormCard
import org.kde.kwordquiz

BasePage {
    id: root

    property bool wasCorrect: true

    readonly property int multipleChoiceLimit: 4

    listView.delegate: Rectangle {
        id: wordDelegate

        required property string question
        required property string questionImage
        required property string questionSound
        required property string answer
        required property string answerImage
        required property string answerSound
        required property var multipleChoice

        readonly property bool isCurrentItem: ListView.isCurrentItem
        readonly property bool hasSelection: multipleChoiceGroup.checkedButton !== null

        width: ListView.view.width
        height: ListView.view.height

        color: Kirigami.Theme.backgroundColor

        visible: listView.count >= root.multipleChoiceLimit && !root.finished

        onIsCurrentItemChanged: if (!isCurrentItem) {
            multipleChoiceGroup.checkState = Qt.Unchecked;
        }

        onHasSelectionChanged: if (Prefs.autoCheck && hasSelection) {
            check();
            root.showAnswer = true;
        }

        function check() {
            if (!multipleChoiceGroup.checkedButton || multipleChoiceGroup.checkedButton.text !== wordDelegate.answer) {
                root.wasCorrect = false;
                randomSortModel.markAsError(listView.currentIndex);
                root.errors++;
            } else {
                root.wasCorrect = true;
                randomSortModel.unMarkAsError(listView.currentIndex);
            }
        }

        ColumnLayout {
            anchors.centerIn: parent
            width: parent.width - Kirigami.Units.gridUnit * 4

            Kirigami.Heading {
                text: wordDelegate.question
            }

            QQC2.ButtonGroup {
                id: multipleChoiceGroup
            }

            Repeater {
                model: wordDelegate.multipleChoice

                FormCard.FormRadioDelegate {
                    text: modelData
                    leftPadding: Kirigami.Units.smallSpacing
                    rightPadding: Kirigami.Units.smallSpacing
                    topPadding: Kirigami.Units.smallSpacing
                    bottomPadding: Kirigami.Units.smallSpacing
                    Layout.fillWidth: true
                    QQC2.ButtonGroup.group: multipleChoiceGroup
                }
            }

            QQC2.Button {
                icon.name: "checkbox"
                text: i18nc("@action:button", "Check")
                enabled: listView.currentItem && listView.currentItem.hasSelection
                onClicked: {
                    listView.currentItem.check();
                    root.showAnswer = true;
                }
                visible: !root.showAnswer && !root.finished && !Prefs.autoCheck
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

    readonly property var _placeHolder: Kirigami.PlaceholderMessage {
        parent: root
        anchors.centerIn: parent
        width: parent.width - Kirigami.Units.gridUnit * 4
        visible: listView.count < 4
        text: i18n("Not enough words in your deck")
        explanation: i18n("You need at least 3 words to enable the multiple choice mode")
    }
}
