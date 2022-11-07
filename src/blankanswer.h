/*
    SPDX-FileCopyrightText: 2008-2010 Peter Hedlund <peter.hedlund@kdemail.net>
    SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef BLANKANSWER_H
#define BLANKANSWER_H

#include <QString>

namespace BlankAnswer
{

struct BlankResult {
    QString blankedAnswer;
    QString correctAnswer;
};

BlankResult blankAnswer(const QString &input);

QString yourAnswerResult(const QString &givenAnswer, const QString &blankedAnswer);

}; // namespace BlankAnswer

#endif
