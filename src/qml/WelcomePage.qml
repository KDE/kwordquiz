// SPDX-FileCopyrightText: 2023 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: LGPL-2.0-or-later

import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15 as QQC2
import org.kde.kirigami 2.20 as Kirigami
import org.kde.kirigamiaddons.labs.mobileform 0.1 as MobileForm
import org.kde.kwordquiz 1.0

Kirigami.ScrollablePage {
    id: root

    title: i18nc("@title:window", "Welcome")

    property var _placeholder: Kirigami.PlaceholderMessage {
        parent: root
        anchors.centerIn: parent
        icon.name: "org.kde.kwordquiz"
        width: parent.width - Kirigami.Units.gridUnit * 4
        text: i18nc("@label", "Get started with your first deck")
        helpfulAction: QQC2.Action {
            text: i18nc("@action:button", "Create Deck")
            onTriggered: applicationWindow().pageStack.push("qrc:/qml/DeckEditorPage.qml", {
                documentModel: documentModel,
            })
        }
        visible: documentRepeater.count === 0
    }

    ColumnLayout {
        visible: documentRepeater.count > 0

        MobileForm.FormCard {
            Layout.fillWidth: true

            contentItem: ColumnLayout {
                MobileForm.FormCardHeader {
                    title: i18n("Decks")
                }
                Repeater {
                    id: documentRepeater

                    model: DocumentModel {
                        id: documentModel
                    }

                    delegate: ColumnLayout {
                        id: documentDelegate

                        required property string title

                        MobileForm.FormButtonDelegate {
                            id: documentButton

                            text: documentDelegate.title
                            Layout.fillWidth: true
                        }

                        MobileForm.FormDelegateSeparator {
                            above: documentButton
                        }
                    }
                }
            }
        }
    }
}
