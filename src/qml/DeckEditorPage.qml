// SPDX-FileCopyrightText: 2023 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: LGPL-2.0-or-later

import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15 as QQC2
import Qt.labs.platform 1.1
import org.kde.kirigami 2.20 as Kirigami
import org.kde.kirigamiaddons.labs.mobileform 0.1 as MobileForm
import org.kde.kwordquiz 1.0
import org.kde.kitemmodels 1.0

Kirigami.ScrollablePage {
    id: root

    property DocumentModel documentModel
    property int mode: DeckEditorPage.CreateMode
    property CardModel editorModel: CardModel {}
    property string filterText: ''

    readonly property bool editPage: true

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

    actions.contextualActions: [
        Kirigami.Action {
            text: i18n("Filter")
            displayComponent: Kirigami.SearchField {
                focus: false
                placeholderText: i18n("Filter...")
                onTextChanged: {
                    root.filterText = text;
                    filterProxy.invalidate();
                }
            }
        },
        Kirigami.Action {
            text: i18nc("@action:button", "Print Preview")
            icon.name: "document-print-preview"

            Kirigami.Action {
                text: i18n("Flashcard")
                icon.name: "org.kde.kwordquiz"
                onTriggered: exporter.printPreview(Exporter.Flashcard)
            }

            Kirigami.Action {
                text: i18n("List")
                icon.name: "view-list-text"
                onTriggered: exporter.printPreview(Exporter.List)
            }

            Kirigami.Action {
                text: i18n("Exam")
                icon.name: "table"
                onTriggered: exporter.printPreview(Exporter.Exam)
            }
        },
        Kirigami.Action {
            text: i18nc("@action:button", "Print")
            icon.name: "document-print"

            Kirigami.Action {
                text: i18n("Flashcard")
                onTriggered: exporter.print(Exporter.Flashcard)
                icon.name: "org.kde.kwordquiz"
            }

            Kirigami.Action {
                text: i18n("List")
                icon.name: "view-list-text"
                onTriggered: exporter.print(Exporter.List)
            }

            Kirigami.Action {
                text: i18n("Exam")
                icon.name: "table"
                onTriggered: exporter.print(Exporter.Exam)
            }
        },
        Kirigami.Action {
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
    ]

    component FileSelectorButton: QQC2.ToolButton {
        id: fileButton

        required property string file
        required property bool isImage
        readonly property bool isSound: !isImage

        signal fileRemoved()
        signal fileAdded(url file)

        icon.name: file ? "delete" : (isImage ? "insert-image" : "folder-music")
        text: if (isImage) {
            file ? i18nc("@action:button", "Remove image") : i18nc("@action:button", "Link image")
        } else {
            file ? i18nc("@action:button", "Remove sound") : i18nc("@action:button", "Link sound")
        }
        display: QQC2.ToolButton.IconOnly

        QQC2.ToolTip.text: text
        QQC2.ToolTip.visible: hovered
        QQC2.ToolTip.delay: Kirigami.Units.toolTipDelay

        onClicked: {
            if (file) {
                fileButton.fileRemoved();
                return;
            }

            var dialog = isImage
                ? imageFileDialog.createObject(QQC2.ApplicationWindow.Overlay)
                : soundFileDialog.createObject(QQC2.ApplicationWindow.Overlay)

            dialog.accepted.connect(() => {
                if (!dialog.file) {
                    return;
                }
                fileButton.fileAdded(dialog.file);
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

    Component {
        id: soundFileDialog

        FileDialog {
            title: i18n("Please choose a sound")
            folder: StandardPaths.writableLocation(StandardPaths.MusicLocation)
        }
    }

    ListView {
        id: listView

        header: QQC2.Pane {
            width: parent.width

            Kirigami.Theme.colorSet: Kirigami.Theme.Window
            Kirigami.Theme.inherit: false

            function focusQuestionField() {
                identifierLeftField.forceActiveFocus();
            }

            function focusAnswerField() {
                identifierRightField.forceActiveFocus();
            }

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
                        id: identifierLeftField
                        text: root.editorModel.identifierLeft
                        background: null
                        onEditingFinished: root.editorModel.identifierLeft = text
                        enabled: root.editorModel.enabled

                        Layout.fillWidth: true
                        font.bold: true
                        Keys.onDownPressed: {
                            const item = listView.itemAtIndex(0);
                            if (item) {
                                item.focusQuestionField();
                            }
                        }
                    }

                    Kirigami.Separator {
                        Layout.fillHeight: true
                        Layout.preferredWidth: 1
                    }

                    QQC2.TextField {
                        id: identifierRightField
                        text: root.editorModel.identifierRight
                        background: null
                        onEditingFinished: root.editorModel.identifierRight = text
                        enabled: root.editorModel.enabled

                        Layout.fillWidth: true
                        font.bold: true
                        Keys.onDownPressed: {
                            const item = listView.itemAtIndex(0);
                            if (item) {
                                item.focusAnswerField();
                            }
                        }
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

            function focusQuestionField() {
                newQuestionField.forceActiveFocus();
            }

            function focusAnswerField() {
                newAnswerField.forceActiveFocus();
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
                    Keys.onUpPressed: {
                        const item = listView.itemAtIndex(listView.count - 1);
                        if (item) {
                            item.focusQuestionField();
                        }
                    }
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
                    Keys.onUpPressed: {
                        const item = listView.itemAtIndex(listView.count - 1);
                        if (item) {
                            item.focusAnswerField();
                        }
                    }
                }
            }
        }

        model: KSortFilterProxyModel {
            id: filterProxy
            sourceModel: root.editorModel
            filterRowCallback: function(sourceRow, sourceParent) {
                let question = sourceModel.data(sourceModel.index(sourceRow, 0, sourceParent), CardModel.QuestionRole);
                let answer = sourceModel.data(sourceModel.index(sourceRow, 0, sourceParent), CardModel.AnswerRole);

                return question.includes(filterText) || answer.includes(filterText);
            }
        }

        delegate: ColumnLayout {
            id: editorDelegate

            required property int index

            required property string question
            required property string questionImage
            required property string questionSound
            required property string answer
            required property string answerImage
            required property string answerSound

            function focusQuestionField() {
                questionField.forceActiveFocus();
            }

            function focusAnswerField() {
                answerField.forceActiveFocus();
            }

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
                        Keys.onUpPressed: {
                            const item = listView.itemAtIndex(editorDelegate.index - 1);
                            if (item) {
                                item.focusQuestionField();
                            } else {
                                listView.headerItem.focusQuestionField();
                            }
                        }
                        Keys.onDownPressed: {
                            const item = listView.itemAtIndex(editorDelegate.index + 1);
                            if (item) {
                                item.focusQuestionField();
                            } else {
                                listView.footerItem.focusQuestionField();
                            }
                        }
                    }

                    FileSelectorButton {
                        isImage: true
                        file: editorDelegate.questionImage
                        onFileRemoved: root.editorModel.removeQuestionImage(editorDelegate.index);
                        onFileAdded: root.editorModel.addQuestionImage(editorDelegate.index, file);
                    }

                    FileSelectorButton {
                        isImage: false
                        file: editorDelegate.questionSound
                        onFileRemoved: root.editorModel.removeQuestionSound(editorDelegate.index);
                        onFileAdded: root.editorModel.addQuestionSound(editorDelegate.index, file);
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
                        Keys.onUpPressed: {
                            const item = listView.itemAtIndex(editorDelegate.index - 1);
                            if (item) {
                                item.focusAnswerField();
                            } else {
                                root.listView.headerItem.focusAnswerField();
                            }
                        }
                        Keys.onDownPressed: {
                            const item = listView.itemAtIndex(editorDelegate.index + 1);
                            if (item) {
                                item.focusAnswerField();
                            } else {
                                listView.footerItem.focusAnswerField();
                            }
                        }
                    }

                    FileSelectorButton {
                        isImage: true
                        file: editorDelegate.answerImage
                        onFileRemoved: root.editorModel.removeAnswerImage(editorDelegate.index);
                        onFileAdded: root.editorModel.addAnswerImage(editorDelegate.index, file);
                    }

                    FileSelectorButton {
                        isImage: false
                        file: editorDelegate.answerSound
                        onFileRemoved: root.editorModel.removeAnswerSound(editorDelegate.index);
                        onFileAdded: root.editorModel.addAnswerSound(editorDelegate.index, file);
                    }
                }
            }

            Kirigami.Separator {
                Layout.fillWidth: true
            }
        }
    }

    Exporter {
        id: exporter
        cardModel: root.editorModel
    }
}