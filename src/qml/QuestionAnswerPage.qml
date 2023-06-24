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

    actions.contextualActions: [
        Kirigami.Action {
            text: i18nc("@action:button", "Check")
            onTriggered: {
                listView.currentItem.check();
                root.showAnswer = true;
            }
            visible: !root.showAnswer && !root.finished
        },
        Kirigami.Action {
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
        Kirigami.Action {
            text: i18nc("@action:button", "Edit")
            icon.name: "document-edit"
            onTriggered: applicationWindow().pageStack.layers.push('qrc:/qml/DeckEditorPage.qml', {
                documentModel: root.documentModel,
                mode: DeckEditorPage.EditMode,
                editorModel: cardModel,
            })
        }
    ]

    function reset() {
        root.randomSortModel.showErrorsOnly = false;
        root.randomSortModel.shuffle();
        root.errors = 0;
        root.showAnswer = false;
        root.finished = false;
        listView.currentIndex = 0;
    }

    listView.delegate: Rectangle {
        id: wordDelegate

        required property string question
        required property string answer

        readonly property bool isCurrentItem: ListView.isCurrentItem

        onIsCurrentItemChanged: if (!isCurrentItem) {
            answerField.text = '';
        }

        function check() {
            if (answerField.text.trim() === answer.trim()) {
                root.wasCorrect = true;
                randomSortModel.unMarkAsError(listView.currentIndex);
            } else {
                root.wasCorrect = false;
                randomSortModel.markAsError(listView.currentIndex);
                root.errors++;
            }
        }

        width: ListView.view.width
        height: ListView.view.height

        color: Kirigami.Theme.backgroundColor

        visible: !root.finished

        ColumnLayout {
            anchors.centerIn: parent
            width: parent.width - Kirigami.Units.gridUnit * 4

            Kirigami.Heading {
                text: wordDelegate.question
                wrapMode: Text.Wrap
                horizontalAlignment: Text.AlignHCenter

                Layout.fillWidth: true
            }

            Kirigami.Separator {
                Layout.fillWidth: true
            }

            QQC2.TextField {
                id: answerField

                Layout.alignment: Qt.AlignHCenter
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
