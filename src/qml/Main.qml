// SPDX-FileCopyrightText: 2023 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: LGPL-2.0-or-later

import QtQuick
import QtQuick.Controls as QQC2
import org.kde.kirigami as Kirigami
import org.kde.kwordquiz

Kirigami.ApplicationWindow {
    id: root

    width: Kirigami.Units.gridUnit * 40
    height: Kirigami.Units.gridUnit * 35

    FileOpener {
        objectName: "FileOpener"

        onFileOpened: pageStack.items[0].openFile(file, mode);
    }

    pageStack {
        defaultColumnWidth: Kirigami.Units.gridUnit * 30
        initialPage: WelcomePage {}

        globalToolBar {
            canContainHandles: true
            style: Kirigami.ApplicationHeaderStyle.ToolBar
            showNavigationButtons: if (applicationWindow().pageStack.currentIndex > 0
                || applicationWindow().pageStack.currentIndex > 0) {
                Kirigami.ApplicationHeaderStyle.ShowBackButton
            } else {
                0
            }
        }
    }

    globalDrawer: Kirigami.GlobalDrawer {
        isMenu: true
        actions: Kirigami.Action {
            text: i18nc("@action:inmenu", "Settings")
            onTriggered: root.pageStack.pushDialogLayer('qrc:/qt/qml/org/kde/kwordquiz/qml/SettingsPage.qml', {}, {
                title: i18n("Configure"),
                width: Kirigami.Units.gridUnit * 15,
            });
        }
    }
}

