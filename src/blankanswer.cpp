/*
    SPDX-FileCopyrightText: 2008-2010 Peter Hedlund <peter.hedlund@kdemail.net>
    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "blankanswer.h"

#include <QRegularExpression>

namespace BlankAnswer
{

BlankResult blankAnswer(const QString &input)
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

}; // namespace BlankAnswer
