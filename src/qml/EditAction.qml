// SPDX-FileCopyrightText: 2023 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: LGPL-2.0-or-later

pragma ComponentBehavior: Bound

import QtQuick.Controls as QQC2
import org.kde.ki18n
import org.kde.kirigami as Kirigami
import org.kde.kwordquiz

Kirigami.Action {
    id: root

    readonly property Kirigami.ApplicationWindow applicationWindow: root.page.QQC2.ApplicationWindow.window as Kirigami.ApplicationWindow

    required property Kirigami.Page page
    required property DocumentModel documentModel
    required property CardModel cardModel

    text: KI18n.i18nc("@action:button", "Edit")
    icon.name: "document-edit"
    onTriggered: root.applicationWindow.pageStack.layers.push('./DeckEditorPage.qml', {
        documentModel: root.documentModel,
        mode: DeckEditorPage.EditMode,
        editorModel: root.cardModel,
    })
}
