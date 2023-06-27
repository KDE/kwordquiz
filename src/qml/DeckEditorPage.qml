// SPDX-FileCopyrightText: 2023 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: LGPL-2.0-or-later

import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15 as QQC2
import Qt.labs.platform 1.1
import org.kde.kirigami 2.20 as Kirigami
import org.kde.kirigamiaddons.labs.mobileform 0.1 as MobileForm
import org.kde.kwordquiz 1.0

Kirigami.ScrollablePage {
    id: root

    property DocumentModel documentModel
    property int mode: DeckEditorPage.CreateMode
    property CardModel editorModel: CardModel {}

    enum Mode {
        EditMode,
        CreateMode
    }

    Component.onCompleted: if (mode === DeckEditorPage.CreateMode) {
        root.editorModel.createNew();
    }

    title: if (mode === DeckEditorPage.CreateMode) {
        i18nc("@title:window", "Create deck")
    } else {
        i18nc("@title:window", "Edit deck")
    }

    actions.main: Kirigami.Action {
        text: mode === DeckEditorPage.CreateMode ? i18nc("@action:button", "Create") : i18nc("@action:button", "Save")
        icon.name: "document-save"
        onTriggered: {
            root.editorModel.save();
            if (mode === DeckEditorPage.CreateMode) {
                root.documentModel.add(root.editorModel.document);
            } else {
                root.editorModel.reloaded();
            }
            applicationWindow().pageStack.layers.pop();
        }
    }

    component ImageSelectorButton: QQC2.ToolButton {
        id: imageButton

        required property string image

        signal imageRemoved()
        signal imageAdded(url image)

        icon.name: image ? "delete" : "insert-image"
        display: QQC2.ToolButton.IconOnly
        text: image ? i18nc("@action:button", "Remove image") : i18nc("@action:button", "Add image")

        QQC2.ToolTip.text: text
        QQC2.ToolTip.visible: hovered
        QQC2.ToolTip.delay: Kirigami.Units.toolTipDelay

        onClicked: {
            if (image) {
                imageButton.imageRemoved();
                return;
            }

            var dialog = imageFileDialog.createObject(QQC2.ApplicationWindow.Overlay)

            dialog.accepted.connect(() => {
                if (!dialog.file) {
                    return;
                }
                imageButton.imageAdded(dialog.file);
            });
            dialog.open();
        }
    }

    Component {
        id: imageFileDialog

        FileDialog {
            title: i18n("Please choose an image")
            folder: StandardPaths.writableLocation(StandardPaths.PicturesLocation)
        }
    }

    ListView {
        header: QQC2.Pane {
            width: parent.width

            Kirigami.Theme.colorSet: Kirigami.Theme.Window
            Kirigami.Theme.inherit: false

            contentItem: ColumnLayout {
                GridLayout {
                    Layout.fillWidth: true

                    columns: 2

                    QQC2.Label {
                        text: i18nc("@label", "Name:")
                    }

                    QQC2.TextField {
                        Layout.fillWidth: true
                        text: root.editorModel.title
                        onTextChanged: root.editorModel.title = text;
                        enabled: root.editorModel.enabled
                    }

                    QQC2.Label {
                        text: i18nc("@label", "Author:")
                    }

                    QQC2.TextField {
                        Layout.fillWidth: true
                        text: root.editorModel.author
                        onTextChanged: root.editorModel.author = text;
                        enabled: root.editorModel.enabled
                    }

                    QQC2.Label {
                        text: i18nc("@label", "License:")
                    }

                    QQC2.TextField {
                        Layout.fillWidth: true
                        text: root.editorModel.license
                        onTextChanged: root.editorModel.license = text;
                        enabled: root.editorModel.enabled
                        placeholderText: i18nc("Example license of licenses", "e.g. CC-BY-SA-4.0 or CC-BY-4.0")
                    }
                }

                Kirigami.Separator {
                    Layout.fillWidth: true
                }

                RowLayout {
                    Layout.fillWidth: true

                    QQC2.TextField {
                        text: root.editorModel.identifierLeft
                        background: null
                        onEditingFinished: root.editorModel.identifierLeft = text
                        enabled: root.editorModel.enabled

                        Layout.fillWidth: true
                        font.bold: true
                    }

                    Kirigami.Separator {
                        Layout.fillHeight: true
                        Layout.preferredWidth: 1
                    }

                    QQC2.TextField {
                        text: root.editorModel.identifierRight
                        background: null
                        onEditingFinished: root.editorModel.identifierRight = text
                        enabled: root.editorModel.enabled

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

                root.editorModel.add(newQuestionField.text, newAnswerField.text);
                newQuestionField.text = '';
                newAnswerField.text = '';
            }

            width: parent.width

            contentItem: RowLayout {
                QQC2.TextField {
                    id: newQuestionField
                    background: null
                    placeholderText: root.editorModel.identifierLeft
                    enabled: root.editorModel.enabled
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
                    placeholderText: root.editorModel.identifierRight
                    onEditingFinished: footer.insertRow()
                    onAccepted: newQuestionField.forceActiveFocus();
                    enabled: root.editorModel.enabled

                    Layout.fillWidth: true
                }
            }
        }

        model: root.editorModel
        delegate: ColumnLayout {
            id: editorDelegate

            required property int index

            required property string question
            required property string questionImage
            required property string questionSound
            required property string answer
            required property string answerImage
            required property string answerSound

            width: ListView.view.width
            spacing: 0

            QQC2.ItemDelegate {
                Layout.fillWidth: true

                contentItem: RowLayout {
                    QQC2.TextField {
                        id: questionField

                        text: editorDelegate.question
                        background: null
                        onEditingFinished: root.editorModel.edit(editorDelegate.index, questionField.text, answerField.text)

                        Layout.fillWidth: true
                    }

                    ImageSelectorButton {
                        image: editorDelegate.questionImage
                        onImageRemoved: root.editorModel.removeQuestionImage(editorDelegate.index);
                        onImageAdded: root.editorModel.addQuestionImage(editorDelegate.index, image);
                    }

                    Kirigami.Separator {
                        Layout.fillHeight: true
                        Layout.preferredWidth: 1
                    }

                    QQC2.TextField {
                        id: answerField

                        text: editorDelegate.answer
                        background: null
                        onEditingFinished: root.editorModel.edit(editorDelegate.index, questionField.text, answerField.text)

                        Layout.fillWidth: true
                    }

                    ImageSelectorButton {
                        image: editorDelegate.answerImage
                        onImageRemoved: root.editorModel.removeAnswerImage(editorDelegate.index);
                        onImageAdded: root.editorModel.addAnswerImage(editorDelegate.index, image);
                    }
                }
            }

            Kirigami.Separator {
                Layout.fillWidth: true
            }
        }
    }
}
