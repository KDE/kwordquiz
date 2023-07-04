// SPDX-FileCopyrightText: 2023 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: LGPL-2.0-or-later

import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15 as QQC2
import Qt.labs.platform 1.1
import org.kde.kirigami 2.20 as Kirigami
import org.kde.kirigamiaddons.labs.mobileform 0.1 as MobileForm
import org.kde.kwordquiz 1.0
import org.kde.newstuff 1.91 as NewStuff
import QtQml 2.15

Kirigami.ScrollablePage {
    id: root

    function openFile(file, mode) {
        const row = documentModel.add(file);
        switch (mode) {
        case Prefs.MultipleChoice:
            multipleChoiceRadio.checked = true;
            break;
        case Prefs.QA:
            qaRadio.checked = true;
            break;
        case Prefs.Flashcard:
        default:
            flashcardRadio.checked = true;
            break;
        }
        applicationWindow().pageStack.layers.clear();
        documentRepeater.itemAt(row).open();
    }

    title: i18nc("@title:window", "Welcome")

    leftPadding: 0
    rightPadding: 0

    FileDialog {
        id: fileDialog

        title: i18n("Please choose an KWordQuiz document")
        folder: StandardPaths.writableLocation(StandardPaths.DocumentsLocation)
        nameFilters: [i18n("KEduDocument (*.kvtml)"), i18n("Wql documents (*.wql)"), i18n("CSV documents (*.csv)"), i18n("XDXF documents (*.xdxf)")]

        onAccepted: {
            documentModel.add(file);
            documentRepeater.itemAt(documentRepeater.count - 1).open();
        }
    }

    ColumnLayout {
        Kirigami.Icon {
            source: "org.kde.kwordquiz"
            Layout.alignment: Qt.AlignHCenter
            implicitWidth: Math.round(Kirigami.Units.iconSizes.huge * 1.5)
            implicitHeight: Math.round(Kirigami.Units.iconSizes.huge * 1.5)
        }

        Kirigami.Heading {
            text: i18n("Welcome to KWordQuiz")

            Layout.alignment: Qt.AlignHCenter
            Layout.topMargin: Kirigami.Units.largeSpacing
        }

        MobileForm.FormGridContainer {
            Layout.fillWidth: true
            Layout.topMargin: Kirigami.Units.largeSpacing

            infoCards: [
                MobileForm.FormGridContainer.InfoCard {
                    title: i18n("Open existing document")
                    action: Kirigami.Action {
                        onTriggered: fileDialog.open();
                    }
                },
                MobileForm.FormGridContainer.InfoCard {
                    title: i18n("Create Deck")
                    action: Kirigami.Action {
                        onTriggered: applicationWindow().pageStack.layers.push("qrc:/qml/DeckEditorPage.qml", {
                            documentModel: documentModel,
                        })
                    }
                },
                MobileForm.FormGridContainer.InfoCard {
                    title: i18n("Download Deck")
                    action: NewStuff.Action {
                        id: newStuffButton
                        configFile: "kwordquiz.knsrc"
                        viewMode: NewStuff.Page.ViewMode.Preview
                        onEntryEvent: function(entry, event) {
                            if (event === NewStuff.Entry.StatusChangedEvent) {
                                documentModel.entryChanged(entry);
                            }
                        }
                    }
                }
            ]
        }

        MobileForm.FormCard {
            Layout.fillWidth: true
            Layout.topMargin: Kirigami.Units.largeSpacing

            contentItem: ColumnLayout {
                spacing: 0

                MobileForm.FormCardHeader {
                    title: i18n("Mode")
                }

                QQC2.ButtonGroup {
                    id: modeGroup

                    checkedButton: switch (Prefs.startSession) {
                    case Prefs.MultipleChoice:
                        return multipleChoiceRadio;
                    case Prefs.QA:
                        return qaRadio;
                    case Prefs.Flashcard:
                    default:
                        return flashcardRadio;
                    }

                    onCheckedButtonChanged: {
                        switch (checkedButton) {
                        case qaRadio:
                            Prefs.startSession = Prefs.QA;
                            break;
                        case multipleChoiceRadio:
                            Prefs.startSession = Prefs.MultipleChoice;
                            break;
                        case flashcardRadio:
                            Prefs.startSession = Prefs.Flashcard;
                            break;
                        }

                        Prefs.save();
                    }
                }

                MobileForm.FormRadioDelegate {
                    id: flashcardRadio

                    text: i18nc("@option:check Mode selector", "Flashcard")
                    QQC2.ButtonGroup.group: modeGroup
                }

                MobileForm.FormRadioDelegate {
                    id: multipleChoiceRadio

                    text: i18nc("@option:check Mode selector", "Multiple Choice")
                    QQC2.ButtonGroup.group: modeGroup
                }

                MobileForm.FormRadioDelegate {
                    id: qaRadio

                    text: i18nc("@option:check Mode selector", "Question-Answer")
                    QQC2.ButtonGroup.group: modeGroup
                }
            }
        }

        MobileForm.FormCard {
            Layout.fillWidth: true
            Layout.topMargin: Kirigami.Units.largeSpacing

            visible: documentRepeater.count > 0

            contentItem: ColumnLayout {
                spacing: 0

                MobileForm.FormCardHeader {
                    title: i18n("Decks")
                }
                Repeater {
                    id: documentRepeater

                    model: DocumentModel {
                        id: documentModel
                    }

                    delegate: ColumnLayout {
                        id: documentDelegate

                        required property int index
                        required property string title
                        required property var document
                        readonly property alias documentButton: documentButton

                        spacing: 0

                        function open() {
                            documentButton.clicked();
                        }

                        MobileForm.AbstractFormDelegate {
                            id: documentButton

                            text: documentDelegate.title

                            contentItem: RowLayout {
                                spacing: 0

                                QQC2.Label {
                                    Layout.fillWidth: true
                                    text: documentDelegate.title
                                    elide: Text.ElideRight
                                    wrapMode: Text.Wrap
                                    maximumLineCount: 2
                                    color: root.enabled ? Kirigami.Theme.textColor : Kirigami.Theme.disabledTextColor
                                    Accessible.ignored: true
                                }

                                QQC2.ToolButton {
                                    text: i18nc("@action:button", "Remove")
                                    icon.name: "delete"
                                    display: QQC2.ToolButton.IconOnly

                                    onClicked: documentModel.remove(documentDelegate.index)

                                    Layout.rightMargin: Kirigami.Units.smallSpacing

                                    QQC2.ToolTip.visible: hovered
                                    QQC2.ToolTip.text: text
                                    QQC2.ToolTip.delay: Kirigami.Units.toolTipDelay
                                }

                                QQC2.ToolButton {
                                    text: i18nc("@action:button", "Edit")
                                    icon.name: "document-edit"
                                    display: QQC2.ToolButton.IconOnly

                                    onClicked: {
                                        const editor = applicationWindow().pageStack.layers.push('qrc:/qml/DeckEditorPage.qml', {
                                            documentModel: documentModel,
                                            mode: DeckEditorPage.EditMode,
                                        });
                                        editor.editorModel.document = documentDelegate.document;
                                    }

                                    Layout.rightMargin: Kirigami.Units.smallSpacing

                                    QQC2.ToolTip.visible: hovered
                                    QQC2.ToolTip.text: text
                                    QQC2.ToolTip.delay: Kirigami.Units.toolTipDelay
                                }

                                MobileForm.FormArrow {
                                    Layout.leftMargin: Kirigami.Units.smallSpacing
                                    Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
                                    direction: MobileForm.FormArrow.Right
                                }
                            }

                            onClicked: if (Prefs.startSession === Prefs.Flashcard){
                                applicationWindow().pageStack.layers.push("qrc:/qml/FlashCardPage.qml", {
                                    document: documentDelegate.document,
                                    documentModel: documentModel,
                                });
                            } else if (Prefs.startSession === Prefs.QA) {
                                applicationWindow().pageStack.layers.push("qrc:/qml/QuestionAnswerPage.qml", {
                                    document: documentDelegate.document,
                                    documentModel: documentModel,
                                });
                            } else  {
                                applicationWindow().pageStack.layers.push("qrc:/qml/MultipleChoicePage.qml", {
                                    document: documentDelegate.document,
                                    documentModel: documentModel,
                                });
                            }

                            Layout.fillWidth: true
                        }

                        MobileForm.FormDelegateSeparator {
                            below: documentButton
                            above: if (index === documentRepeater.count - 1) {
                                null
                            } else {
                                documentRepeater.itemAt(index + 1).children[0]
                            }
                            visible: index !== documentRepeater.count - 1
                        }
                    }
                }
            }
        }
    }
}
