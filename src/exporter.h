// SPDX-FileCopyrightText: 2002-2010 Peter Hedlund <peter.hedlund@kdemail.net>
// SPDX-FileCopyrightText: 2023 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "kwqcardmodel.h"
#include <QObject>

class QPrinter;
class QTextDocument;

class Exporter : public QObject
{
    Q_OBJECT
    Q_PROPERTY(KWQCardModel *cardModel MEMBER m_cardModel NOTIFY cardModelChanged)

public:
    enum PrintStyle {
        Flashcard,
        List,
        Exam,
    };
    Q_ENUM(PrintStyle);

    Q_INVOKABLE void printPreview(const PrintStyle printStyle);
    Q_INVOKABLE void print(const PrintStyle printStyle);

Q_SIGNALS:
    void cardModelChanged();

private:
    void printPreviewRequested(QPrinter *printer, const PrintStyle printStyle);
    void createPages(QPrinter *printer, QTextDocument *textDoc, bool sendToPrinter, const PrintStyle printStyle);

    KWQCardModel *m_cardModel = nullptr;
};
