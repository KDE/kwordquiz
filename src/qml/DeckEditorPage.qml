// SPDX-FileCopyrightText: 2023 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: LGPL-2.0-or-later

pragma ComponentBehavior: Bound

import QtQuick
import org.kde.ki18n
import QtQuick.Layouts
import QtQuick.Controls as QQC2
import Qt.labs.platform
import org.kde.kirigami as Kirigami
import org.kde.kwordquiz
import org.kde.kitemmodels

Kirigami.ScrollablePage {
    id: root

    readonly property Kirigami.ApplicationWindow applicationWindow: QQC2.ApplicationWindow.window as Kirigami.ApplicationWindow

    property DocumentModel documentModel
    property int mode: DeckEditorPage.CreateMode
    property CardModel editorModel: CardModel {}
    property string filterText: ''

    readonly property bool editPage: true

    enum Mode {
        EditMode,
        CreateMode
    }

    Component.onCompleted: if (root.mode === DeckEditorPage.CreateMode) {
        root.editorModel.createNew();
    }

    title: if (root.mode === DeckEditorPage.CreateMode) {
        KI18n.i18nc("@title:window", "Create Deck")
    } else {
        KI18n.i18nc("@title:window", "Edit Deck")
    }

    actions: [
        Kirigami.Action {
            text: KI18n.i18n("Filter")
            displayComponent: Kirigami.SearchField {
                focus: false
                placeholderText: KI18n.i18n("Filter...")
                onTextChanged: {
                    root.filterText = text;
                    filterProxy.invalidate();
                }
            }
        },
        Kirigami.Action {
            text: KI18n.i18nc("@action:button", "Print Preview")
            icon.name: "document-print-preview"

            Kirigami.Action {
                text: KI18n.i18n("Flashcard")
                icon.name: "org.kde.kwordquiz"
                onTriggered: exporter.printPreview(Exporter.Flashcard)
            }

            Kirigami.Action {
                text: KI18n.i18n("List")
                icon.name: "view-list-text"
                onTriggered: exporter.printPreview(Exporter.List)
            }

            Kirigami.Action {
                text: KI18n.i18n("Exam")
                icon.name: "table"
                onTriggered: exporter.printPreview(Exporter.Exam)
            }
        },
        Kirigami.Action {
            text: KI18n.i18nc("@action:button", "Print")
            icon.name: "document-print"

            Kirigami.Action {
                text: KI18n.i18n("Flashcard")
                onTriggered: exporter.print(Exporter.Flashcard)
                icon.name: "org.kde.kwordquiz"
            }

            Kirigami.Action {
                text: KI18n.i18n("List")
                icon.name: "view-list-text"
                onTriggered: exporter.print(Exporter.List)
            }

            Kirigami.Action {
                text: KI18n.i18n("Exam")
                icon.name: "table"
                onTriggered: exporter.print(Exporter.Exam)
            }
        },
        Kirigami.Action {
            text: root.mode === DeckEditorPage.CreateMode ? KI18n.i18nc("@action:button", "Create") : KI18n.i18nc("@action:button", "Save")
            icon.name: "document-save"
            onTriggered: {
                root.editorModel.save();
                if (root.mode === DeckEditorPage.CreateMode) {
                    root.documentModel.addDocument(root.editorModel.document);
                } else {
                    root.editorModel.reloaded();
                }
                root.applicationWindow.pageStack.layers.pop();
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
            file ? KI18n.i18nc("@action:button", "Remove Image") : KI18n.i18nc("@action:button", "Link Image")
        } else {
            file ? KI18n.i18nc("@action:button", "Remove Sound") : KI18n.i18nc("@action:button", "Link Sound")
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
                ? imageFileDialog.createObject(fileButton.QQC2.Overlay.overlay)
                : soundFileDialog.createObject(fileButton.QQC2.Overlay.overlay)

            dialog.accepted.connect(() => {
                if (!dialog.file) {
                    return;
                }
                fileButton.fileAdded(dialog.file);
            });
            dialog.open();
        }
    }

    component LanguageSelectorButton: QQC2.ToolButton {
        id: button

        property string language

        QQC2.ToolTip.text: KI18n.i18nc("@action:button", "Set language")
        QQC2.ToolTip.visible: hovered
        QQC2.ToolTip.delay: Kirigami.Units.toolTipDelay

        font.family: language.length > 0 ? "emoji" : Kirigami.Theme.defaultFont.family

        text: language.length > 0 ? LanguageListModel.flagFromName(language) : KI18n.i18nc("@action:button", "Set Language")
        display: language.length > 0 ? QQC2.ToolButton.TextOnly : QQC2.ToolButton.IconOnly
        icon.name: language.length > 0 ? "" : "language-chooser"

        onClicked: {
            var page  = root.applicationWindow.pageStack.pushDialogLayer(languageSelectorPage, {
                width: Kirigami.Units.gridUnit * 10,
                height: Kirigami.Units.gridUnit * 12,
                language: button.language,
            });

            page.languageChanged.connect(() => {
                button.language = page.language;
            });
        }
    }

    Component {
        id: imageFileDialog

        FileDialog {
            title: KI18n.i18n("Please Choose an Image")
            folder: StandardPaths.writableLocation(StandardPaths.PicturesLocation)
        }
    }

    Component {
        id: soundFileDialog

        FileDialog {
            title: KI18n.i18n("Please Choose a Sound")
            folder: StandardPaths.writableLocation(StandardPaths.MusicLocation)
        }
    }

    Component {
        id: languageSelectorPage

        LanguageSelectorPage {}
    }

    ListView {
        id: listView

        header: EditorHeader {}

        component EditorHeader: QQC2.Pane {
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
                        text: KI18n.i18nc("@label", "Name:")
                    }

                    QQC2.TextField {
                        Layout.fillWidth: true
                        text: root.editorModel.title
                        onTextChanged: root.editorModel.title = text;
                        enabled: root.editorModel.enabled
                    }

                    QQC2.Label {
                        text: KI18n.i18nc("@label", "Author:")
                    }

                    QQC2.TextField {
                        Layout.fillWidth: true
                        text: root.editorModel.author
                        onTextChanged: root.editorModel.author = text;
                        enabled: root.editorModel.enabled
                    }

                    QQC2.Label {
                        text: KI18n.i18nc("@label", "License:")
                    }

                    QQC2.ComboBox {
                        Layout.fillWidth: true
                        editText: root.editorModel.license
                        onCurrentIndexChanged: if (root.editorModel.document) {
                            root.editorModel.license = currentIndex;
                        }
                        currentIndex: 0
                        Component.onCompleted: if (root.editorModel.license.length === 0) {
                            editText = currentValue;
                        }
                        enabled: root.editorModel.enabled
                        editable: true
                        model: [
                            'CC-BY-SA-4.0',
                            'CC-BY-4.0',
                            'CC0-1.0',
                        ]
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
                            const item = listView.itemAtIndex(0) as EditorDelegate;
                            if (item) {
                                item.focusQuestionField();
                            }
                        }
                        onAccepted: identifierRightField.forceActiveFocus()
                    }

                    LanguageSelectorButton {
                        id: identifierLeftLanguage
                        onLanguageChanged: {
                            root.editorModel.langQuestion = language;
                            root.editorModel.identifierLeft = LanguageListModel.languageName(language);
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
                            const item = listView.itemAtIndex(0) as EditorDelegate;
                            if (item) {
                                item.focusAnswerField();
                            }
                        }

                        onAccepted: {
                            const item = listView.itemAtIndex(0) as EditorDelegate;
                            if (item) {
                                item.focusQuestionField();
                            }
                        }
                    }

                    LanguageSelectorButton {
                        id: identifierRightLanguage
                        onLanguageChanged: {
                            root.editorModel.langAnswer = language;
                            root.editorModel.identifierRight = LanguageListModel.languageName(language);
                        }
                    }
                }
            }
        }

        footer: EditorFooter {}

        component EditorFooter: QQC2.ItemDelegate {
            id: footer

            background: null

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
                        const item = listView.itemAtIndex(listView.count - 1) as EditorDelegate;
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
                        const item = listView.itemAtIndex(listView.count - 1) as EditorDelegate;
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
                let question = filterProxy.sourceModel.data(filterProxy.sourceModel.index(sourceRow, 0, sourceParent), CardModel.QuestionRole);
                let answer = filterProxy.sourceModel.data(filterProxy.sourceModel.index(sourceRow, 0, sourceParent), CardModel.AnswerRole);

                return question.includes(root.filterText) || answer.includes(root.filterText);
            }
        }

        delegate: EditorDelegate {}

        component EditorDelegate: ColumnLayout {
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
                            const item = listView.itemAtIndex(editorDelegate.index - 1) as EditorDelegate;
                            if (item) {
                                item.focusQuestionField();
                            } else {
                                (listView.headerItem as EditorHeader).focusQuestionField();
                            }
                        }
                        Keys.onDownPressed: {
                            const item = listView.itemAtIndex(editorDelegate.index + 1) as EditorDelegate;
                            if (item) {
                                item.focusQuestionField();
                            } else {
                                (listView.footerItem as EditorFooter).focusQuestionField();
                            }
                        }
                        onAccepted: answerField.forceActiveFocus()
                    }

                    FileSelectorButton {
                        isImage: true
                        file: editorDelegate.questionImage
                        onFileRemoved: root.editorModel.removeQuestionImage(editorDelegate.index);
                        onFileAdded: file => root.editorModel.addQuestionImage(editorDelegate.index, file);
                    }

                    FileSelectorButton {
                        isImage: false
                        file: editorDelegate.questionSound
                        onFileRemoved: root.editorModel.removeQuestionSound(editorDelegate.index);
                        onFileAdded: file => root.editorModel.addQuestionSound(editorDelegate.index, file);
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
                            const item = listView.itemAtIndex(editorDelegate.index - 1) as EditorDelegate;
                            if (item) {
                                item.focusAnswerField();
                            } else {
                                (listView.headerItem as EditorHeader).focusAnswerField();
                            }
                        }
                        Keys.onDownPressed: {
                            const item = listView.itemAtIndex(editorDelegate.index + 1) as EditorDelegate;
                            if (item) {
                                item.focusAnswerField();
                            } else {
                                (listView.footerItem as EditorFooter).focusAnswerField();
                            }
                        }
                        onAccepted: {
                            const item = listView.itemAtIndex(editorDelegate.index + 1) as EditorDelegate;
                            if (item) {
                                item.focusQuestionField();
                            } else {
                                (listView.headerItem as EditorHeader).focusQuestionField();
                            }
                        }

                    }

                    FileSelectorButton {
                        isImage: true
                        file: editorDelegate.answerImage
                        onFileRemoved: root.editorModel.removeAnswerImage(editorDelegate.index);
                        onFileAdded: file => root.editorModel.addAnswerImage(editorDelegate.index, file);
                    }

                    FileSelectorButton {
                        isImage: false
                        file: editorDelegate.answerSound
                        onFileRemoved: root.editorModel.removeAnswerSound(editorDelegate.index);
                        onFileAdded: file => root.editorModel.addAnswerSound(editorDelegate.index, file);
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
