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

    property bool showAnswer: false

    title: i18n("Cards %1/%2 Errors %3", 4, 7, 2)

    actions.contextualActions: [
        Kirigami.Action {
            text: i18nc("@action:button", "Check")
            onTriggered: root.showAnswer = true;
            visible: !root.showAnswer
        },
        Kirigami.Action {
            icon.name: "know"
            text: i18nc("@action:button", "Correct")
            visible: root.showAnswer
            onTriggered: root.showAnswer = false;
        },
        Kirigami.Action {
            text: i18nc("@action:button", "Not Correct")
            icon.name: "dontknow"
            visible: root.showAnswer
            onTriggered: root.showAnswer = false;
        },
        Kirigami.Action {
            text: i18nc("@action:button", "Edit")
            icon.name: "document-edit"
        }
    ]

    ColumnLayout {
        anchors.centerIn: parent
        width: parent.width - Kirigami.Units.gridUnit * 4

        QQC2.Label {
            text: "the overcoat"
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
            text: "el abrigo"
            wrapMode: Text.Wrap
            horizontalAlignment: Text.AlignHCenter
            font.pixelSize: 30
            visible: root.showAnswer

            Layout.fillWidth: true
        }
    }
}
