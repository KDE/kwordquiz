// SPDX-FileCopyrightText: 2023 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: LGPL-2.0-or-later

import QtQuick 2.15
import QtQuick.Controls 2.15 as QQC2
import QtQuick.Layouts 1.15
import org.kde.kirigami 2.20 as Kirigami
import org.kde.kwordquiz 1.0

Kirigami.ScrollablePage {
    id: root

    property string language

    title: i18n("Select a language")

    ListView {
        model: LanguageListModel
        delegate: QQC2.ItemDelegate {
            id: languageDelegate

            required property string nativeName
            required property string languageCode
            required property string flag

            text: nativeName

            contentItem: RowLayout {
                Kirigami.Heading {
                    level: 2
                    font.family: "emoji"
                    text: flag
                }

                QQC2.Label {
                    text: languageDelegate.text

                    Layout.fillWidth: true
                }
            }

            width: ListView.view.width
            highlighted: languageCode === root.language
            onClicked: {
                root.language = languageCode;
                root.closeDialog();
            }
        }
    }
}
