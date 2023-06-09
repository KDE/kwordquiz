// SPDX-FileCopyrightText: 2023 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: LGPL-2.0-or-later

import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15 as QQC2
import org.kde.kirigami 2.20 as Kirigami
import org.kde.kirigamiaddons.labs.mobileform 0.1 as MobileForm
import org.kde.kwordquiz 1.0

Kirigami.ScrollablePage {
    id: root

    required property DocumentModel documentModel
    property int mode: DeckEditorPage.CreateMode

    enum Mode {
        EditMode,
        CreateMode
    }

    Component.onCompleted: if (mode === DeckEditorPage.CreateMode) {
        editorModel.createNew();
    }

    title: if (mode === DeckEditorPage.CreateMode) {
        i18nc("@title:window", "Create deck")
    } else {
        i18nc("@title:window", "Edit deck")
    }

    actions.main: Kirigami.Action {
        text: i18nc("@action:button", "Create")
        icon.name: "document-save"
        visible: mode === DeckEditorPage.CreateMode
        onTriggered: {
            editorModel.save();
            root.documentModel.add(editorModel.document);
            applicationWindow().pageStack.pop();
        }
    }

    ListView {
        header: QQC2.Pane {
            width: parent.width

            Kirigami.Theme.colorSet: Kirigami.Theme.Window
            Kirigami.Theme.inherit: false

            contentItem: ColumnLayout {
                RowLayout {
                    Layout.fillWidth: true

                    QQC2.Label {
                        text: i18nc("@label", "Name:")
                    }

                    QQC2.TextField {
                        Layout.fillWidth: true
                        text: editorModel.title
                        onTextChanged: editorModel.title = text;
                        enabled: editorModel.enabled
                    }
                }

                Kirigami.Separator {
                    Layout.fillWidth: true
                }

                RowLayout {
                    Layout.fillWidth: true

                    QQC2.TextField {
                        text: editorModel.identifierLeft
                        background: null
                        onEditingFinished: editorModel.identifierLeft = text
                        enabled: editorModel.enabled

                        Layout.fillWidth: true
                        font.bold: true
                    }

                    Kirigami.Separator {
                        Layout.fillHeight: true
                        Layout.preferredWidth: 1
                    }

                    QQC2.TextField {
                        text: editorModel.identifierRight
                        background: null
                        onEditingFinished: editorModel.identifierRight = text
                        enabled: editorModel.enabled

                        Layout.fillWidth: true
                        font.bold: true
                    }
                }
            }
        }

        footer: QQC2.ItemDelegate {
            id: footer

            function insertRow() {
                if (newQuestionField.text.length === 0 || newAnswerField.text.length === 0) {
                    return;
                }

                editorModel.add(newQuestionField.text, newAnswerField.text);
                newQuestionField.text = '';
                newAnswerField.text = '';
            }

            width: parent.width

            contentItem: RowLayout {
                QQC2.TextField {
                    id: newQuestionField
                    background: null
                    placeholderText: editorModel.identifierLeft
                    enabled: editorModel.enabled
                    onEditingFinished: footer.insertRow();
                    onAccepted: newAnswerField.forceActiveFocus();

                    Layout.fillWidth: true
                }

                Kirigami.Separator {
                    Layout.fillHeight: true
                    Layout.preferredWidth: 1
                }

                QQC2.TextField {
                    id: newAnswerField
                    background: null
                    placeholderText: editorModel.identifierRight
                    onEditingFinished: footer.insertRow()
                    onAccepted: newQuestionField.forceActiveFocus();
                    enabled: editorModel.enabled

                    Layout.fillWidth: true
                }
            }
        }

        model: EditorModel {
            id: editorModel
        }

        delegate: ColumnLayout {
            id: editorDelegate

            required property string question
            required property string answer

            required property int index

            width: ListView.view.width
            spacing: 0

            QQC2.ItemDelegate {
                Layout.fillWidth: true

                contentItem: RowLayout {
                    QQC2.TextField {
                        id: questionField

                        text: editorDelegate.question
                        background: null
                        onEditingFinished: editorModel.edit(editorDelegate.index, questionField.text, answerField.text)

                        Layout.fillWidth: true
                    }

                    Kirigami.Separator {
                        Layout.fillHeight: true
                        Layout.preferredWidth: 1
                    }

                    QQC2.TextField {
                        id: answerField

                        text: editorDelegate.answer
                        background: null
                        onEditingFinished: editorModel.edit(editorDelegate.index, questionField.text, answerField.text)

                        Layout.fillWidth: true
                    }
                }
            }

            Kirigami.Separator {
                Layout.fillWidth: true
            }
        }
    }
}
