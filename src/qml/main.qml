// SPDX-FileCopyrightText: 2023 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: LGPL-2.0-or-later

import QtQuick 2.15
import QtQuick.Controls 2.15 as QQC2
import org.kde.kirigami 2.20 as Kirigami

Kirigami.ApplicationWindow {
    id: root

    width: Kirigami.Units.gridUnit * 27
    height: Kirigami.Units.gridUnit * 23

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
}

