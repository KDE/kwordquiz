// SPDX-FileCopyrightText: 2021 Carl Schwan <carlschwan@kde.org>
// SPDX-License-Identifier: LGPL-2.0-or-later

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls as QQC2
import org.kde.ki18n
import org.kde.kirigami as Kirigami
import QtQuick.Layouts
import org.kde.kirigamiaddons.formcard as FormCard
import org.kde.kwordquiz

FormCard.FormCardPage {
    id: root

    readonly property Kirigami.ApplicationWindow applicationWindow: QQC2.ApplicationWindow.window as Kirigami.ApplicationWindow

    title: KI18n.i18nc("@title:window", "Settings")

    FormCard.FormHeader {
        title: KI18n.i18n("General")
    }

    FormCard.FormCard {
        FormCard.FormCheckDelegate {
            id: showScoreCheckbox
            text: KI18n.i18n("Show score as percentage")
            checked: Prefs.percent
            onCheckedChanged: {
                Prefs.percent = checked;
                Prefs.save();
            }
        }
    }

    FormCard.FormHeader {
        title: KI18n.i18n("Flash Card")
    }

    FormCard.FormCard {
        autoSeparators: true

        FormCard.FormCheckDelegate {
            id: autoFlipCheck
            text: KI18n.i18n("Automatically flip flashcard")
            checked: Prefs.autoFlip
            onCheckedChanged: {
                Prefs.autoFlip = checked;
                Prefs.save();
            }
        }

        FormCard.FormSpinBoxDelegate {
            id: flipDelayCheck
            label: KI18n.i18n("Time delay for flipping the flashcard")
            value: Prefs.flipDelay
            from: 1
            to: 30
            enabled: Prefs.autoFlip
            onValueChanged: {
                Prefs.flipDelay = value;
                Prefs.save();
            }
        }
    }

    FormCard.FormHeader {
        title: KI18n.i18n("Multiple choice")
    }

    FormCard.FormCard {
        FormCard.FormCheckDelegate {
            text: KI18n.i18n("Automatically check selected answer")
            checked: Prefs.autoCheck
            onCheckedChanged: {
                Prefs.autoCheck = checked;
                Prefs.save();
            }
        }
    }

    FormCard.FormHeader {
        title: KI18n.i18n("Question and Answer")
    }

    FormCard.FormCard {
        autoSeparators: true

        FormCard.FormCheckDelegate {
            id: hintErrorCheckbox
            text: KI18n.i18n("Count using hint as an error")
            checked: Prefs.hintError
            onCheckedChanged: {
                Prefs.hintError = checked
                Prefs.save();
            }
        }

        FormCard.FormCheckDelegate {
            id: caseSensitiveCheck
            text: KI18n.i18n("Compare answers in a case sensitive way")
            checked: Prefs.caseSensitiveAnswers
            onCheckedChanged: {
                Prefs.caseSensitiveAnswers = checked
                Prefs.save();
            }
        }
    }

    FormCard.FormCard {
        Layout.topMargin: Kirigami.Units.gridUnit
        autoSeparators: true

        FormCard.FormButtonDelegate {
            id: aboutPageButton
            icon.name: "org.kde.kwordquiz"
            text: KI18n.i18n("About KWordQuiz")
            onClicked: root.applicationWindow.pageStack.layers.push(aboutPage)

            Component {
                id: aboutPage
                FormCard.AboutPage {
                }
            }
        }

        FormCard.FormButtonDelegate {
            id: aboutKDEButton
            icon.name: "kde"
            text: KI18n.i18n("About KDE")
            onClicked: root.applicationWindow.pageStack.layers.push(aboutKde)

            Component {
                id: aboutKde
                FormCard.AboutKDEPage {}
            }
        }
    }
}
