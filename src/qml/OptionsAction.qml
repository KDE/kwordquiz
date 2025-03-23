// SPDX-FileCopyrightText: 2023 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: LGPL-2.0-or-later

import org.kde.kirigami 2.20 as Kirigami
import org.kde.kwordquiz 1.0

Kirigami.Action {
    id: root

    required property CardModel cardModel

    text: i18nc("@action:button", "Options")

    Kirigami.Action {
        text: i18nc("@action:inmenu", "Shuffle")
        checkable: true
        checked: Prefs.randomize
        onCheckedChanged: Prefs.randomize = true
    }

    Kirigami.Action {
        text: `${root.cardModel.identifierLeft} -> ${root.cardModel.identifierRight}`
        checkable: true
        checked: Prefs.questionInLeftColumn
        onCheckedChanged: checked => Prefs.questionInLeftColumn = checked
    }

    Kirigami.Action {
        text: `${root.cardModel.identifierRight} -> ${root.cardModel.identifierLeft}`
        checkable: true
        checked: Prefs.questionInRightColumn
        onCheckedChanged: checked => Prefs.questionInRightColumn = checked
    }
}
