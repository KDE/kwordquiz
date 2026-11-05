// SPDX-FileCopyrightText: 2023 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: LGPL-2.0-or-later

pragma ComponentBehavior: Bound

import QtQuick
import org.kde.ki18n
import org.kde.kirigami as Kirigami
import org.kde.kwordquiz

Kirigami.ApplicationWindow {
    id: root

    width: Kirigami.Units.gridUnit * 40
    height: Kirigami.Units.gridUnit * 35

    FileOpener {
        objectName: "FileOpener"

        onFileOpened: (file, mode) => welcomePage.openFile(file, mode)
    }

    pageStack {
        defaultColumnWidth: Kirigami.Units.gridUnit * 30
        initialPage: WelcomePage {
            id: welcomePage
        }

        globalToolBar {
            canContainHandles: true
            style: Kirigami.ApplicationHeaderStyle.ToolBar
            showNavigationButtons: if (root.pageStack.currentIndex > 0
                || root.pageStack.currentIndex > 0) {
                Kirigami.ApplicationHeaderStyle.ShowBackButton
            } else {
                0
            }
        }
    }

    globalDrawer: Kirigami.GlobalDrawer {
        isMenu: true
        actions: Kirigami.Action {
            text: KI18n.i18nc("@action:inmenu", "Settings")
            onTriggered: root.pageStack.pushDialogLayer('qrc:/qt/qml/org/kde/kwordquiz/qml/SettingsPage.qml', {}, {
                title: KI18n.i18n("Configure"),
                width: Kirigami.Units.gridUnit * 15,
            });
        }
    }
}

