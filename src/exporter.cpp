// SPDX-FileCopyrightText: 2002-2010 Peter Hedlund <peter.hedlund@kdemail.net>
// SPDX-FileCopyrightText: 2023 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: GPL-2.0-or-later

#include "exporter.h"

#include <KLocalizedString>
#include <QPrintDialog>
#include <QPrintPreviewDialog>
#include <QPrinter>
#include <QTextDocument>
#include <QTextTable>

#include "kwordquiz_version.h"

void Exporter::print(const PrintStyle printStyle)
{
    QPrinter printer;
    QPrintDialog printDialog(&printer, nullptr);

    if (printDialog.exec() == QDialog::Accepted) {
        QTextDocument td;
        createPages(&printer, &td, true, printStyle);
    }
}

void Exporter::printPreview(const PrintStyle printStyle)
{
    QPrinter printer;
    QPrintPreviewDialog preview(&printer);

    connect(&preview, &QPrintPreviewDialog::paintRequested, this, [this, printStyle](QPrinter *printer) {
        printPreviewRequested(printer, printStyle);
    });

    preview.exec();
}

void Exporter::printPreviewRequested(QPrinter *printer, const PrintStyle printStyle)
{
    QTextDocument td;
    createPages(printer, &td, true, printStyle);
}

void Exporter::createPages(QPrinter *printer, QTextDocument *textDoc, bool sendToPrinter, const PrintStyle printStyle)
{
    printer->setFullPage(true);
    int myDpi = printer->logicalDpiY();

    if (printStyle == Flashcard) {
        printer->setPageOrientation(QPageLayout::Landscape);

        int cardWidth = qRound(5 * qreal(myDpi));
        int cardHeight = qRound(3 * qreal(myDpi));

        QTextTable *table = textDoc->rootFrame()->lastCursorPosition().insertTable(m_cardModel->rowCount(), 2);

        QTextTableFormat tableFormat = table->format();
        tableFormat.setHeaderRowCount(0);
        tableFormat.setBorderStyle(QTextFrameFormat::BorderStyle_None);
        tableFormat.setCellSpacing(0);
        tableFormat.setCellPadding(0);

        QVector<QTextLength> constraints;
        constraints.append(QTextLength(QTextLength::FixedLength, cardWidth));
        constraints.append(QTextLength(QTextLength::FixedLength, cardWidth));

        tableFormat.setColumnWidthConstraints(constraints);
        table->setFormat(tableFormat);

        QTextBlockFormat headerFormat;
        headerFormat.setAlignment(Qt::AlignLeft);

        QTextCharFormat headerCharFormat;
        headerCharFormat.setFont(QFontDatabase::systemFont(QFontDatabase::GeneralFont));

        QTextBlockFormat cellFormat;
        cellFormat.setAlignment(Qt::AlignCenter);

        QTextCharFormat cellCharFormat;
        cellCharFormat.setFont(QFontDatabase::systemFont(QFontDatabase::GeneralFont));

        QTextFrameFormat cardFormat;
        cardFormat.setBorder(1);
        cardFormat.setBorderStyle(QTextFrameFormat::BorderStyle_Solid);
        cardFormat.setBorderBrush(QBrush(Qt::black));
        cardFormat.setWidth(QTextLength(QTextLength::FixedLength, cardWidth));
        cardFormat.setHeight(QTextLength(QTextLength::FixedLength, cardHeight));
        cardFormat.setPadding(qRound(0.25 * myDpi));

        QTextFrameFormat lineFormat;
        lineFormat.setBorder(1);
        lineFormat.setBorderStyle(QTextFrameFormat::BorderStyle_Solid);
        lineFormat.setBorderBrush(QBrush(Qt::black));
        lineFormat.setWidth(QTextLength(QTextLength::FixedLength, qRound(4.5 * myDpi)));
        lineFormat.setHeight(1.1); // 1 is drawn as a box whereas this is drawn as a line. Strange...
        lineFormat.setPadding(0);

        QTextFrame *card;
        for (int i = 0; i < m_cardModel->rowCount(); i++) {
            const std::array<int, 2> roles = {
                KWQCardModel::QuestionRole,
                KWQCardModel::AnswerRole,
            };
            int j = 0;
            for (const auto role : roles) {
                cardFormat.setPosition(QTextFrameFormat::FloatLeft);
                card = table->cellAt(i, j).firstCursorPosition().insertFrame(cardFormat);
                card->lastCursorPosition().insertText(m_cardModel->headerData(j, Qt::Horizontal, Qt::DisplayRole).toString(), headerCharFormat);
                card->lastCursorPosition().insertFrame(lineFormat);
                card->lastCursorPosition().insertBlock();
                card->lastCursorPosition().insertBlock();
                card->lastCursorPosition().insertBlock(cellFormat, cellCharFormat);
                card->lastCursorPosition().insertText(m_cardModel->data(m_cardModel->index(i, 0), role).toString(), cellCharFormat);
            }
        }
    } else {
        textDoc->rootFrame()->lastCursorPosition().insertText(QStringLiteral("kwordquiz %1").arg(KWORDQUIZ_VERSION_STRING));

        if (printStyle == Exam)
            textDoc->rootFrame()->lastCursorPosition().insertText(' ' + i18n("Name:_____________________________ Date:__________"));

        QTextTable *table;
        if (printStyle == Exam)
            table = textDoc->rootFrame()->lastCursorPosition().insertTable(m_cardModel->rowCount() + 1, 4);
        else
            table = textDoc->rootFrame()->lastCursorPosition().insertTable(m_cardModel->rowCount() + 1, 3);

        QTextTableFormat tableFormat = table->format();
        tableFormat.setHeaderRowCount(1);
        tableFormat.setBorder(1);
        tableFormat.setBorderStyle(QTextFrameFormat::BorderStyle_Solid);
        tableFormat.setCellSpacing(0);
        tableFormat.setBorderBrush(QBrush(Qt::black));
        tableFormat.setCellPadding(2);

        QVector<QTextLength> constraints;
        // constraints.append(QTextLength(QTextLength::FixedLength,
        // verticalHeader()->width()));
        constraints.append(QTextLength(QTextLength::FixedLength, 100));
        constraints.append(QTextLength(QTextLength::FixedLength, 100));
        if (printStyle == Exam)
            constraints.append(QTextLength(QTextLength::FixedLength, 50));
        tableFormat.setColumnWidthConstraints(constraints);

        table->setFormat(tableFormat);

        QTextBlockFormat headerFormat;
        headerFormat.setAlignment(Qt::AlignHCenter);

        QTextCharFormat headerCharFormat;
        headerCharFormat.setFont(QFontDatabase::systemFont(QFontDatabase::GeneralFont));

        QTextCursor cellCursor;
        cellCursor = table->cellAt(0, 1).firstCursorPosition();
        cellCursor.mergeBlockFormat(headerFormat);
        cellCursor.mergeCharFormat(headerCharFormat);
        cellCursor.insertText(m_cardModel->headerData(0, Qt::Horizontal, Qt::DisplayRole).toString());

        cellCursor = table->cellAt(0, 2).firstCursorPosition();
        cellCursor.mergeBlockFormat(headerFormat);
        cellCursor.mergeCharFormat(headerCharFormat);
        cellCursor.insertText(m_cardModel->headerData(1, Qt::Horizontal, Qt::DisplayRole).toString());

        if (printStyle == Exam) {
            cellCursor = table->cellAt(0, 3).firstCursorPosition();
            cellCursor.mergeBlockFormat(headerFormat);
            cellCursor.mergeCharFormat(headerCharFormat);
            cellCursor.insertText(i18n("Score"));
        }

        headerCharFormat = cellCursor.charFormat();
        QTextCharFormat cellCharFormat = cellCursor.charFormat();
        cellCharFormat.setFont(QFontDatabase::systemFont(QFontDatabase::GeneralFont));

        for (int i = 0; i < m_cardModel->rowCount(); i++) {
            table->cellAt(i + 1, 0).firstCursorPosition().insertText(m_cardModel->headerData(i, Qt::Vertical, Qt::DisplayRole).toString(), headerCharFormat);
            table->cellAt(i + 1, 1).firstCursorPosition().insertText(m_cardModel->data(m_cardModel->index(i, 0), KWQCardModel::QuestionRole).toString(),
                                                                     cellCharFormat);
            if (printStyle == List)
                table->cellAt(i + 1, 2).firstCursorPosition().insertText(m_cardModel->data(m_cardModel->index(i, 0), KWQCardModel::AnswerRole).toString(),
                                                                         cellCharFormat);
        }
    }

    if (sendToPrinter) {
        textDoc->print(printer);
    }
}
