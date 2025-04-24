// SPDX-FileCopyrightText: 2023 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: LGPL-2.0-or-later

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls as QQC2
import org.kde.kirigami as Kirigami
import org.kde.kwordquiz

Kirigami.Page {
    id: root

    property alias document: cardModel.document
    property alias listView: listView
    property alias randomSortModel: randomSortModel
    property alias cardModel: cardModel
    property var documentModel

    property bool finished: false
    property int errors: 0
    property bool showAnswer: false

    title: if (Prefs.percent) {
        i18n("Cards %1% Errors %2", Math.round((listView.currentIndex + (finished ? 1 : 0)) / listView.count * 100), root.errors)
    } else {
        i18n("Cards %1/%2 Errors %3", listView.currentIndex + 1, listView.count, root.errors)
    }

    actions: [
        OptionsAction {
            cardModel: root.cardModel
        },
        EditAction {
            cardModel: root.cardModel
            documentModel: root.documentModel
        }
    ]

    function reset() {
        root.randomSortModel.showErrorsOnly = false;
        root.randomSortModel.reset();
        root.errors = 0;
        root.showAnswer = false;
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
                    text: i18n("Repeat Errors")
                    visible: root.errors > 0
                    onClicked: {
                        root.randomSortModel.showErrorsOnly = true;
                        root.listView.currentIndex = 0;
                        root.errors = 0;
                        root.finished = 0;
                    }
                }

                QQC2.Button {
                    text: i18n("Try Again")
                    onClicked: root.reset();
                }
            }
        }
    }
}
