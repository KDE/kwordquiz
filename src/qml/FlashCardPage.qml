// SPDX-FileCopyrightText: 2023 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: LGPL-2.0-or-later

import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15 as QQC2
import org.kde.kirigami 2.20 as Kirigami
import org.kde.kirigamiaddons.labs.mobileform 0.1 as MobileForm
import org.kde.kwordquiz 1.0

Kirigami.Page {
    id: root

    property alias document: cardModel.document
    property var documentModel

    property bool finished: false
    property bool showAnswer: false
    property int errors: 0

    title: i18n("Cards %1/%2 Errors %3", listView.currentIndex + 1, listView.count, root.errors)

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
        randomSortModel.showErrorsOnly = false;
        randomSortModel.shuffle();
        root.showAnswer = false;
        root.errors = 0;
        root.finished = false;
        listView.currentIndex = 0;
    }

    ListView {
        id: listView

        anchors.fill: parent

        highlightMoveVelocity: width * 2

        model: RandomSortModel {
            id: randomSortModel

            cardModel: CardModel {
                id: cardModel

                onReloaded: root.reset()
            }
        }

        snapMode: ListView.SnapOneItem
        orientation: ListView.Horizontal
        interactive: false

        delegate: Rectangle {
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

                QQC2.Label {
                    text: wordDelegate.question
                    wrapMode: Text.Wrap
                    horizontalAlignment: Text.AlignHCenter
                    font.pixelSize: 30

                    Layout.fillWidth: true
                }

                Kirigami.Separator {
                    visible: root.showAnswer

                    Layout.fillWidth: true
                }

                QQC2.Label {
                    text: wordDelegate.answer
                    wrapMode: Text.Wrap
                    horizontalAlignment: Text.AlignHCenter
                    font.pixelSize: 30
                    visible: root.showAnswer

                    Layout.fillWidth: true
                }
            }
        }

        Kirigami.PlaceholderMessage {
            anchors.centerIn: parent
            width: parent.width - Kirigami.Units.gridUnit * 4
            visible: root.finished
            text: i18nc("@label", "Finished")

            explanation: root.errors === 0 ? i18n("You got a perfect score") : i18np("You made one mistake", "You made %1 mistakes", root.errors)

            RowLayout {
                Layout.alignment: Qt.AlignHCenter
                Layout.topMargin: Kirigami.Units.gridUnit

                QQC2.Button {
                    text: i18n("Repeat errors")
                    visible: root.errors > 0
                    onClicked: {
                        randomSortModel.showErrorsOnly = true;
                        listView.currentIndex = 0;
                        root.errors = 0;
                        root.finished = 0;
                    }
                }

                QQC2.Button {
                    text: i18n("Try again")
                    onClicked: root.reset();
                }
            }
        }
    }
}
