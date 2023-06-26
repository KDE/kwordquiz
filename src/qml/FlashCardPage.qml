// SPDX-FileCopyrightText: 2023 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: LGPL-2.0-or-later

import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15 as QQC2
import org.kde.kirigami 2.20 as Kirigami
import org.kde.kwordquiz 1.0

BasePage {
    id: root

    property bool showAnswer: false

    actions.contextualActions: [
        Kirigami.Action {
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
        root.showAnswer = false;
        root.errors = 0;
        root.finished = false;
        listView.currentIndex = 0;
    }

    listView.delegate: Rectangle {
        id: wordDelegate

        required property string question
        required property string answer

        width: ListView.view.width
        height: ListView.view.height

        color: Kirigami.Theme.backgroundColor

        visible: !root.finished

        ColumnLayout {
            anchors.centerIn: parent
            width: parent.width - Kirigami.Units.gridUnit * 4

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
