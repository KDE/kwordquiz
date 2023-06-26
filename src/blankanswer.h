// SPDX-FileCopyrightText: 2006-2010 Peter Hedlund <peter.hedlund@kdemail.net>
// SPDX-FileCopyrightText: 2023 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <QString>
#include <QObject>


class BlankAnswer : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString input READ input WRITE setInput NOTIFY inputChanged)
    Q_PROPERTY(bool hasBlank READ hasBlank NOTIFY inputChanged)
    Q_PROPERTY(QString blankedAnswer READ blankedAnswer NOTIFY inputChanged)
    Q_PROPERTY(QString correctAnswer READ correctAnswer NOTIFY inputChanged)

public:

    QString input() const;
    void setInput(const QString &input);

    bool hasBlank() const;

    QString blankedAnswer() const;

    QString correctAnswer() const;

    static QString yourAnswerResult(const QString &givenAnswer, const QString &blankedAnswer);

    struct BlankResult {
        QString blankedAnswer;
        QString correctAnswer;
    };

    static BlankResult blankAnswer(const QString &input);

Q_SIGNALS:
    void inputChanged();

private:

    QString m_input;
    BlankResult m_blankResult;
};
