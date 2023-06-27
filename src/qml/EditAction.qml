// SPDX-FileCopyrightText: 2023 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: LGPL-2.0-or-later

import org.kde.kirigami 2.20 as Kirigami
import org.kde.kwordquiz 1.0

Kirigami.Action {
    id: root

    required property DocumentModel documentModel
    required property CardModel cardModel

    text: i18nc("@action:button", "Edit")
    icon.name: "document-edit"
    onTriggered: applicationWindow().pageStack.layers.push('qrc:/qml/DeckEditorPage.qml', {
        documentModel: root.documentModel,
        mode: DeckEditorPage.EditMode,
        editorModel: cardModel,
    })
}
