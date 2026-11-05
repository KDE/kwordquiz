// SPDX-FileCopyrightText: 2023 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: LGPL-2.0-or-later

pragma ComponentBehavior: Bound

import QtQuick
import org.kde.ki18n
import QtQuick.Layouts
import QtQuick.Controls as QQC2
import Qt.labs.platform
import org.kde.kirigami as Kirigami
import org.kde.kirigamiaddons.formcard as FormCard
import org.kde.kwordquiz
import org.kde.newstuff as NewStuff
import QtQml

FormCard.FormCardPage {
    id: root

    readonly property Kirigami.ApplicationWindow applicationWindow: QQC2.ApplicationWindow.window as Kirigami.ApplicationWindow

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
        root.applicationWindow.pageStack.layers.clear();
        (documentRepeater.itemAt(row) as DocumentDelegate).open();
    }

    title: KI18n.i18nc("@title:window", "Welcome")

    data: FileDialog {
        id: fileDialog

        title: KI18n.i18n("Please choose a WordQuiz document")
        folder: StandardPaths.writableLocation(StandardPaths.DocumentsLocation)
        nameFilters: [KI18n.i18n("KEduDocument (*.kvtml)"), KI18n.i18n("Wql documents (*.wql)"), KI18n.i18n("CSV documents (*.csv)"), KI18n.i18n("XDXF documents (*.xdxf)")]

        onAccepted: {
            documentModel.add(file);
            (documentRepeater.itemAt(documentRepeater.count - 1) as DocumentDelegate).open();
        }
    }

    Kirigami.Icon {
        source: "org.kde.kwordquiz"
        Layout.alignment: Qt.AlignHCenter
        implicitWidth: Math.round(Kirigami.Units.iconSizes.huge * 1.5)
        implicitHeight: Math.round(Kirigami.Units.iconSizes.huge * 1.5)
    }

    Kirigami.Heading {
        text: KI18n.i18n("Welcome to WordQuiz")

        Layout.alignment: Qt.AlignHCenter
        Layout.topMargin: Kirigami.Units.largeSpacing
    }

    FormCard.FormGridContainer {
        Layout.fillWidth: true
        Layout.topMargin: Kirigami.Units.largeSpacing

        infoCards: [
            FormCard.FormGridContainer.InfoCard {
                title: KI18n.i18n("Open Existing Document")
                action: Kirigami.Action {
                    onTriggered: fileDialog.open();
                }
            },
            FormCard.FormGridContainer.InfoCard {
                title: KI18n.i18n("Create Deck")
                action: Kirigami.Action {
                    onTriggered: root.applicationWindow.pageStack.layers.push("./DeckEditorPage.qml", {
                        documentModel: documentModel,
                    })
                }
            },
            FormCard.FormGridContainer.InfoCard {
                title: KI18n.i18n("Download Deck")
                action: NewStuff.Action {
                    id: newStuffButton
                    configFile: "kwordquiz.knsrc"
                    viewMode: NewStuff.Page.ViewMode.Preview
                    onEntryEvent: function(entry, event) {
                        documentModel.entryChanged(entry, event);
                    }
                }
            }
        ]
    }

    FormCard.FormHeader {
        title: KI18n.i18nc("@title:group", "Mode")
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

            text: KI18n.i18nc("@option:check Mode selector", "Flashcard")
            QQC2.ButtonGroup.group: modeGroup
        }

        FormCard.FormRadioDelegate {
            id: multipleChoiceRadio

            text: KI18n.i18nc("@option:check Mode selector", "Multiple choice")
            QQC2.ButtonGroup.group: modeGroup
        }

        FormCard.FormRadioDelegate {
            id: qaRadio

            text: KI18n.i18nc("@option:check Mode selector", "Question-Answer")
            QQC2.ButtonGroup.group: modeGroup
        }
    }

    FormCard.FormCard {
        Layout.topMargin: Kirigami.Units.largeSpacing

        FormCard.FormSwitchDelegate {
            text: KI18n.i18nc("@option:check", "Smart practice")
            description: KI18n.i18n("Practice up to 20 cards, starting with mistakes and unseen cards. Your progress is saved on this device.")
            checked: Prefs.smartPractice
            onToggled: {
                Prefs.smartPractice = checked;
                Prefs.save();
            }
        }
    }

    FormCard.FormHeader {
        title: KI18n.i18n("Decks")
    }

    FormCard.FormCard {
        visible: documentRepeater.count > 0
        autoSeparators: true

        Repeater {
            id: documentRepeater

            model: DocumentModel {
                id: documentModel
            }

            delegate: DocumentDelegate {}

            component DocumentDelegate: FormCard.AbstractFormDelegate {
                id: documentDelegate

                required property int index
                required property string title
                required property var document
                required property var documentStatus

                enabled: documentStatus !== DocumentModel.CannotLoadFile

                function open() {
                    documentDelegate.clicked();
                }

                text: documentDelegate.title

                QQC2.ToolTip.visible: enabled ? false : hovered
                QQC2.ToolTip.text: KI18n.i18n("Document cannot be opened")

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
                        text: KI18n.i18nc("@action:button", "Remove")
                        icon.name: "delete"
                        display: QQC2.ToolButton.IconOnly

                        onClicked: documentModel.remove(documentDelegate.index)

                        Layout.rightMargin: Kirigami.Units.smallSpacing

                        QQC2.ToolTip.visible: hovered
                        QQC2.ToolTip.text: text
                        QQC2.ToolTip.delay: Kirigami.Units.toolTipDelay
                    }

                    QQC2.ToolButton {
                        text: KI18n.i18nc("@action:button", "Edit")
                        icon.name: "document-edit"
                        display: QQC2.ToolButton.IconOnly

                        onClicked: {
                            const editor = root.applicationWindow.pageStack.layers.push('./DeckEditorPage.qml', {
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
                    root.applicationWindow.pageStack.layers.push("./FlashCardPage.qml", {
                        document: documentDelegate.document,
                        documentModel: documentModel,
                    });
                } else if (Prefs.startSession === Prefs.QA) {
                    root.applicationWindow.pageStack.layers.push("./QuestionAnswerPage.qml", {
                        document: documentDelegate.document,
                        documentModel: documentModel,
                    });
                } else  {
                    root.applicationWindow.pageStack.layers.push("./MultipleChoicePage.qml", {
                        document: documentDelegate.document,
                        documentModel: documentModel,
                    });
                }

                Layout.fillWidth: true
            }
        }
    }
}
