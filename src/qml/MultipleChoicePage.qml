// SPDX-FileCopyrightText: 2023 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: LGPL-2.0-or-later

import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15 as QQC2
import org.kde.kirigami 2.20 as Kirigami
import org.kde.kirigamiaddons.labs.mobileform 0.1 as MobileForm
import org.kde.kwordquiz 1.0

BasePage {
    id: root

    property bool wasCorrect: true

    actions.contextualActions: [
        Kirigami.Action {
            id: checkAction

            text: i18nc("@action:button", "Check")
            enabled: listView.currentItem && listView.currentItem.hasSelection
            onTriggered: {
                listView.currentItem.check();
                root.showAnswer = true;
            }
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
        required property var multipleChoice

        readonly property bool isCurrentItem: ListView.isCurrentItem
        readonly property bool hasSelection: multipleChoiceGroup.checkedButton !== null

        width: ListView.view.width
        height: ListView.view.height

        color: Kirigami.Theme.backgroundColor

        visible: listView.count >= 4 && !root.finished

        onIsCurrentItemChanged: if (!isCurrentItem) {
            multipleChoiceGroup.checkState = Qt.Unchecked;
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

                MobileForm.FormRadioDelegate {
                    text: modelData
                    leftPadding: Kirigami.Units.smallSpacing
                    rightPadding: Kirigami.Units.smallSpacing
                    topPadding: Kirigami.Units.smallSpacing
                    bottomPadding: Kirigami.Units.smallSpacing
                    Layout.fillWidth: true
                    QQC2.ButtonGroup.group: multipleChoiceGroup
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

    readonly property var _placeHolder: Kirigami.PlaceholderMessage {
        parent: root
        anchors.centerIn: parent
        width: parent.width - Kirigami.Units.gridUnit * 4
        visible: listView.count < 4
        text: i18n("Not enough words in your deck")
        explanation: i18n("You need at least 3 words but more is better")
    }
}
