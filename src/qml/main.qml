// SPDX-FileCopyrightText: 2023 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: LGPL-2.0-or-later

import QtQuick 2.15
import QtQuick.Controls 2.15 as QQC2
import org.kde.kirigami 2.20 as Kirigami

Kirigami.ApplicationWindow {
    id: root

    width: Kirigami.Units.gridUnit * 30
    height: Kirigami.Units.gridUnit * 26

    property real previousWidth: width
    property real previousHeight: height

    pageStack {
        defaultColumnWidth: Kirigami.Units.gridUnit * 30
        initialPage: WelcomePage {}

        layers {
            onCurrentItemChanged: if (pageStack.layers.currentItem && pageStack.layers.currentItem.editPage) {
                root.previousWidth = root.width;
                root.previousHeight = root.height;
                root.width = Kirigami.Units.gridUnit * 40;
                root.height = Kirigami.Units.gridUnit * 35;
            } else {
                root.width = root.previousWidth;
                root.height = root.previousHeight;
            }
        }

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
            onTriggered: root.pageStack.pushDialogLayer('qrc:/qml/SettingsPage.qml', {}, {
                title: i18n("Configure"),
                width: Kirigami.Units.gridUnit * 15,
            });
        }
    }
}

