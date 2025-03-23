// SPDX-FileCopyrightText: 2023 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: LGPL-2.0-or-later

import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15 as QQC2
import Qt.labs.platform 1.1
import org.kde.kirigami 2.20 as Kirigami
import org.kde.kirigamiaddons.formcard 1.0 as FormCard
import org.kde.kwordquiz 1.0
import org.kde.newstuff 1.91 as NewStuff
import QtQml 2.15

FormCard.FormCardPage {
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

    data: FileDialog {
        id: fileDialog

        title: i18n("Please choose a WordQuiz document")
        folder: StandardPaths.writableLocation(StandardPaths.DocumentsLocation)
        nameFilters: [i18n("KEduDocument (*.kvtml)"), i18n("Wql documents (*.wql)"), i18n("CSV documents (*.csv)"), i18n("XDXF documents (*.xdxf)")]

        onAccepted: {
            documentModel.add(file);
            documentRepeater.itemAt(documentRepeater.count - 1).open();
        }
    }

    Kirigami.Icon {
        source: "org.kde.kwordquiz"
        Layout.alignment: Qt.AlignHCenter
        implicitWidth: Math.round(Kirigami.Units.iconSizes.huge * 1.5)
        implicitHeight: Math.round(Kirigami.Units.iconSizes.huge * 1.5)
    }

    Kirigami.Heading {
        text: i18n("Welcome to WordQuiz")

        Layout.alignment: Qt.AlignHCenter
        Layout.topMargin: Kirigami.Units.largeSpacing
    }

    FormCard.FormGridContainer {
        Layout.fillWidth: true
        Layout.topMargin: Kirigami.Units.largeSpacing

        infoCards: [
            FormCard.FormGridContainer.InfoCard {
                title: i18n("Open Existing Document")
                action: Kirigami.Action {
                    onTriggered: fileDialog.open();
                }
            },
            FormCard.FormGridContainer.InfoCard {
                title: i18n("Create Deck")
                action: Kirigami.Action {
                    onTriggered: applicationWindow().pageStack.layers.push("qrc:/qml/DeckEditorPage.qml", {
                        documentModel: documentModel,
                    })
                }
            },
            FormCard.FormGridContainer.InfoCard {
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

    FormCard.FormHeader {
        title: i18nc("@title:group", "Mode")
    }

    FormCard.FormCard {
        data: QQC2.ButtonGroup {
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

        FormCard.FormRadioDelegate {
            id: flashcardRadio

            text: i18nc("@option:check Mode selector", "Flashcard")
            QQC2.ButtonGroup.group: modeGroup
        }

        FormCard.FormRadioDelegate {
            id: multipleChoiceRadio

            text: i18nc("@option:check Mode selector", "Multiple choice")
            QQC2.ButtonGroup.group: modeGroup
        }

        FormCard.FormRadioDelegate {
            id: qaRadio

            text: i18nc("@option:check Mode selector", "Question-Answer")
            QQC2.ButtonGroup.group: modeGroup
        }
    }

    FormCard.FormHeader {
        title: i18n("Decks")
    }

    FormCard.FormCard {
        visible: documentRepeater.count > 0

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
                required property var documentStatus
                readonly property alias documentButton: documentButton
                readonly property bool readError: documentStatus === DocumentModel.CannotLoadFile

                spacing: 0

                function open() {
                    documentButton.clicked();
                }

                FormCard.AbstractFormDelegate {
                    id: documentButton
                    enabled: documentStatus !== DocumentModel.CannotLoadFile

                    text: documentDelegate.title

                    QQC2.ToolTip.visible: enabled ? false : hovered
                    QQC2.ToolTip.text: i18n("Document cannot be opened")

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

                        FormCard.FormArrow {
                            Layout.leftMargin: Kirigami.Units.smallSpacing
                            Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
                            direction: Qt.RightArrow
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

                FormCard.FormDelegateSeparator {
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
