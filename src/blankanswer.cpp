/*
    SPDX-FileCopyrightText: 2008-2010 Peter Hedlund <peter.hedlund@kdemail.net>
    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "blankanswer.h"

#include <QRegularExpression>
#include <QStringList>

using namespace Qt::Literals::StringLiterals;

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
    static QRegularExpression rx(u"\\[(.*?)\\]"_s);

    QRegularExpressionMatchIterator matchIt = rx.globalMatch(input);

    // When there is no blank return an empty result
    if (!matchIt.hasNext()) {
        return result;
    }

    result.blankedAnswer = input;
    result.blankedAnswer.replace(rx, u".........."_s);

    QStringList answerParts;

    while (matchIt.hasNext()) {
        QRegularExpressionMatch match = matchIt.next();
        answerParts << match.captured(1);
    }

    result.correctAnswer = answerParts.join(u"; "_s);

    return result;
}

QString BlankAnswer::yourAnswerResult(const QString &givenAnswer, const QString &blankedAnswer)
{
    QString result = givenAnswer;

    if (!blankedAnswer.isEmpty()) {
        result.clear();
        QStringList ls;

        if (givenAnswer.indexOf(QLatin1Char(';')) > 0)
            ls = givenAnswer.split(QLatin1Char(';'));
        else
            ls.append(givenAnswer);

        result = QString(blankedAnswer).replace(u".........."_s, u"<u></u>"_s);

        int offset = 0, counter = 0;
        while (offset >= 0 && counter < ls.size()) {
            offset = result.indexOf(u"<u>"_s, offset);
            if (offset >= 0) {
                result.insert(offset + 3, ls[counter]);
                offset++;
                counter++;
            }
        }
        result.append(u"</qt>"_s);
        result.prepend(u"<qt>"_s);
    }

    return result;
}

#include "moc_blankanswer.cpp"
