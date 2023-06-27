// SPDX-FileCopyrightText: 2021 Carl Schwan <carlschwan@kde.org>
// SPDX-License-Identifier: LGPL-2.0-or-later

import QtQuick 2.15
import org.kde.kirigami 2.20 as Kirigami
import QtQuick.Layouts 1.15
import org.kde.kirigamiaddons.labs.mobileform 0.1 as MobileForm
import org.kde.kwordquiz 1.0

Kirigami.ScrollablePage {
    id: root

    title: i18nc("@title:window", "Settings")

    leftPadding: 0
    rightPadding: 0

    ColumnLayout {
        MobileForm.FormCard {
            Layout.fillWidth: true
            contentItem: ColumnLayout {
                spacing: 0

                MobileForm.FormCardHeader {
                    title: i18n("General")
                }

                MobileForm.FormCheckDelegate {
                    id: showScoreCheckbox
                    text: i18n("Show score as percentage")
                    checked: Prefs.percent
                    onCheckedChanged: {
                        Prefs.percent = checked;
                        Prefs.save();
                    }
                }
            }
        }

        MobileForm.FormCard {
            Layout.fillWidth: true
            Layout.topMargin: Kirigami.Units.largeSpacing
            contentItem: ColumnLayout {
                spacing: 0

                MobileForm.FormCardHeader {
                    title: i18n("Flash Card")
                }

                MobileForm.FormCheckDelegate {
                    id: autoFlipCheck
                    text: i18n("Automatically flip flashcard")
                    checked: Prefs.autoFlip
                    onCheckedChanged: {
                        Prefs.autoFlip = checked;
                        Prefs.save();
                    }
                }

                MobileForm.FormDelegateSeparator { above: flipDelayCheck; below: autoFlipCheck }

                MobileForm.FormSpinBoxDelegate {
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
        }

        MobileForm.FormCard {
            Layout.fillWidth: true
            Layout.topMargin: Kirigami.Units.largeSpacing
            contentItem: ColumnLayout {
                spacing: 0

                MobileForm.FormCardHeader {
                    title: i18n("Multiple choice")
                }

                MobileForm.FormCheckDelegate {
                    text: i18n("Automatically check selected answer")
                    checked: Prefs.autoCheck
                    onCheckedChanged: {
                        Prefs.autoCheck = checked;
                        Prefs.save();
                    }
                }
            }
        }

        MobileForm.FormCard {
            Layout.fillWidth: true
            Layout.topMargin: Kirigami.Units.largeSpacing
            contentItem: ColumnLayout {
                spacing: 0

                MobileForm.FormCardHeader {
                    title: i18n("Question and Answer")
                }

                MobileForm.FormCheckDelegate {
                    id: hintErrorCheckbox
                    text: i18n("Count using hint as an error")
                    checked: Prefs.hintError
                    onCheckedChanged: {
                        Prefs.hintError = checked
                        Prefs.save();
                    }
                }

                MobileForm.FormDelegateSeparator { above: caseSensitiveCheck; below: hintErrorCheckbox }

                MobileForm.FormCheckDelegate {
                    id: caseSensitiveCheck
                    text: i18n("Compare answers in a case sensitive way")
                    checked: Prefs.caseSensitiveAnswers
                    onCheckedChanged: {
                        Prefs.caseSensitiveAnswers = checked
                        Prefs.save();
                    }
                }
            }
        }

        MobileForm.FormCard {
            Layout.topMargin: Kirigami.Units.largeSpacing
            Layout.fillWidth: true
            contentItem: ColumnLayout {
                spacing: 0

                Component {
                    id: aboutPage
                    MobileForm.AboutPage {
                        aboutData: About
                    }
                }
                Component {
                    id: abouteKDE
                    MobileForm.AboutKDE {}
                }

                MobileForm.FormButtonDelegate {
                    id: aboutPageButton
                    icon.name: "org.kde.kwordquiz"
                    text: i18n("About KWordQuiz")
                    onClicked: applicationWindow().pageStack.layers.push(aboutPage)
                }

                MobileForm.FormDelegateSeparator { above: aboutKDEButton; below: aboutKDEButton}

                MobileForm.FormButtonDelegate {
                    id: aboutKDEButton
                    icon.name: "kde"
                    text: i18n("About KDE")
                    onClicked: applicationWindow().pageStack.layers.push(aboutKDE)
                }
            }
        }
    }
}