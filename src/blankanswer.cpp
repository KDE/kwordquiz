/*
    SPDX-FileCopyrightText: 2008-2010 Peter Hedlund <peter.hedlund@kdemail.net>
    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "blankanswer.h"

#include <QRegularExpression>
#include <QStringList>
#include <QDebug>

QString BlankAnswer::input() const
{
    return m_input;
}

void BlankAnswer::setInput(const QString &input)
{
    if (input == m_input) {
        return;
    }
    m_input = input;

    m_blankResult = blankAnswer(m_input);

    qDebug() << m_input << m_blankResult.correctAnswer << m_blankResult.blankedAnswer;

    Q_EMIT inputChanged();
}

bool BlankAnswer::hasBlank() const
{
    return m_blankResult.correctAnswer.length() > 0;
}

QString BlankAnswer::blankedAnswer() const
{
    return m_blankResult.blankedAnswer;
}

QString BlankAnswer::correctAnswer() const
{
    return m_blankResult.correctAnswer;
}

BlankAnswer::BlankResult BlankAnswer::blankAnswer(const QString &input)
{
    BlankResult result;
    QRegularExpression rx(QStringLiteral("\\[(.*?)\\]"));

    QRegularExpressionMatchIterator matchIt = rx.globalMatch(input);

    // When there is no blank return an empty result
    if (!matchIt.hasNext()) {
        return result;
    }

    result.blankedAnswer = input;
    result.blankedAnswer.replace(rx, QStringLiteral(".........."));

    QStringList answerParts;

    while (matchIt.hasNext()) {
        QRegularExpressionMatch match = matchIt.next();
        answerParts << match.captured(1);
    }

    result.correctAnswer = answerParts.join(QLatin1String("; "));

    return result;
}

QString BlankAnswer::yourAnswerResult(const QString &givenAnswer, const QString &blankedAnswer)
{
    QString result = givenAnswer;

    if (!blankedAnswer.isEmpty()) {
        result.clear();
        QStringList ls;

        if (givenAnswer.indexOf(';') > 0)
            ls = givenAnswer.split(';');
        else
            ls.append(givenAnswer);

        result = QString(blankedAnswer).replace(QLatin1String(".........."), QLatin1String("<u></u>"));

        int offset = 0, counter = 0;
        while (offset >= 0 && counter < ls.size()) {
            offset = result.indexOf(QStringLiteral("<u>"), offset);
            if (offset >= 0) {
                result.insert(offset + 3, ls[counter]);
                offset++;
                counter++;
            }
        }
        result.append("</qt>");
        result.prepend("<qt>");
    }

    return result;
}
