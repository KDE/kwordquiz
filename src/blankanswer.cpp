/*
    SPDX-FileCopyrightText: 2008-2010 Peter Hedlund <peter.hedlund@kdemail.net>
    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "blankanswer.h"

#include <QRegExp>
#include <QStringList>

namespace BlankAnswer
{

BlankResult blankAnswer(const QString &input)
{
    BlankResult result;

    QString r;
    QString tTemp = input;
    r = tTemp;

    QRegExp rx;
    rx.setMinimal(true);
    rx.setPattern(QStringLiteral("\\[.*\\]"));

    r.replace(rx, QStringLiteral(".........."));

    if (r != tTemp) {
        result.blankedAnswer = r;
        int offset = 0;
        while (offset >= 0) {
            offset = rx.indexIn(tTemp, offset);
            if (offset >= 0) {
                if (result.correctAnswer.length() > 0)
                    result.correctAnswer = result.correctAnswer + "; " + tTemp.mid(offset + 1, tTemp.indexOf(']', offset) - offset - 1);
                else
                    result.correctAnswer = tTemp.mid(offset + 1, tTemp.indexOf(']', offset) - offset - 1);
                offset++;
            }
        }
    }

    return result;
}

QString yourAnswerResult(const QString &givenAnswer, const QString &blankedAnswer)
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

}; // namespace BlankAnswer
