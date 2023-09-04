// SPDX-FileCopyrightText: 2021 Carl Schwan <carlschwan@kde.org>
// SPDX-License-Identifier: LGPL-2.0-or-later

import QtQuick 2.15
import org.kde.kirigami 2.20 as Kirigami
import QtQuick.Layouts 1.15
import org.kde.kirigamiaddons.formcard 1.0 as FormCard
import org.kde.kwordquiz 1.0

FormCard.FormCardPage {
    id: root

    title: i18nc("@title:window", "Settings")

    FormCard.FormHeader {
        title: i18n("General")
    }

    FormCard.FormCard {
        FormCard.FormCheckDelegate {
            id: showScoreCheckbox
            text: i18n("Show score as percentage")
            checked: Prefs.percent
            onCheckedChanged: {
                Prefs.percent = checked;
                Prefs.save();
            }
        }
    }

    FormCard.FormHeader {
        title: i18n("Flash Card")
    }

    FormCard.FormCard {
        FormCard.FormCheckDelegate {
            id: autoFlipCheck
            text: i18n("Automatically flip flashcard")
            checked: Prefs.autoFlip
            onCheckedChanged: {
                Prefs.autoFlip = checked;
                Prefs.save();
            }
        }

        FormCard.FormDelegateSeparator { above: flipDelayCheck; below: autoFlipCheck }

        FormCard.FormSpinBoxDelegate {
            id: flipDelayCheck
            label: i18n("Time delay for flipping the flashcard")
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
        title: i18n("Multiple choice")
    }

    FormCard.FormCard {
        FormCard.FormCheckDelegate {
            text: i18n("Automatically check selected answer")
            checked: Prefs.autoCheck
            onCheckedChanged: {
                Prefs.autoCheck = checked;
                Prefs.save();
            }
        }
    }

    FormCard.FormHeader {
        title: i18n("Question and Answer")
    }

    FormCard.FormCard {
        FormCard.FormCheckDelegate {
            id: hintErrorCheckbox
            text: i18n("Count using hint as an error")
            checked: Prefs.hintError
            onCheckedChanged: {
                Prefs.hintError = checked
                Prefs.save();
            }
        }

        FormCard.FormDelegateSeparator { above: caseSensitiveCheck; below: hintErrorCheckbox }

        FormCard.FormCheckDelegate {
            id: caseSensitiveCheck
            text: i18n("Compare answers in a case sensitive way")
            checked: Prefs.caseSensitiveAnswers
            onCheckedChanged: {
                Prefs.caseSensitiveAnswers = checked
                Prefs.save();
            }
        }
    }

    FormCard.FormCard {
        Layout.topMargin: Kirigami.Units.gridUnit

        FormCard.FormButtonDelegate {
            id: aboutPageButton
            icon.name: "org.kde.kwordquiz"
            text: i18n("About KWordQuiz")
            onClicked: applicationWindow().pageStack.layers.push(aboutPage)

            Component {
                id: aboutPage
                FormCard.AboutPage {
                    aboutData: About
                }
            }
        }

        FormCard.FormDelegateSeparator { above: aboutKDEButton; below: aboutKDEButton}

        FormCard.FormButtonDelegate {
            id: aboutKDEButton
            icon.name: "kde"
            text: i18n("About KDE")
            onClicked: applicationWindow().pageStack.layers.push(aboutKde)

            Component {
                id: aboutKde
                FormCard.AboutKDE {}
            }
        }
    }
}