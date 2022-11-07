/*
    SPDX-FileCopyrightText: 2022 Nicolas Fella <nicolas.fella@gmx.de>
    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include <QTest>

#include "../src/blankanswer.h"

class YourAnswerTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void testYourAnswer_data();
    void testYourAnswer();
};

void YourAnswerTest::testYourAnswer_data()
{
    QTest::addColumn<QString>("givenAnswer");
    QTest::addColumn<QString>("blankedAnswer");
    QTest::addColumn<QString>("expectedYourAnswer");

    QTest::addRow("no_blank") << "Hallo"
                              << ""
                              << "Hallo";

    QTest::addRow("one_blank_empty") << ""
                                     << "One blank .........."
                                     << "<qt>One blank <u></u></qt>";

    QTest::addRow("one_blank_not_empty") << "filled"
                                         << "One blank .........."
                                         << "<qt>One blank <u>filled</u></qt>";

    QTest::addRow("two_blanks_filled") << "blanks;filled"
                                       << "Two .......... .........."
                                       << "<qt>Two <u>blanks</u> <u>filled</u></qt>";

    QTest::addRow("two_blanks_one_filled") << "blanks"
                                           << "Two .......... .........."
                                           << "<qt>Two <u>blanks</u> <u></u></qt>";

    QTest::addRow("two_blanks_empty") << ""
                                      << "Two .......... .........."
                                      << "<qt>Two <u></u> <u></u></qt>";
}

void YourAnswerTest::testYourAnswer()
{
    QFETCH(QString, givenAnswer);
    QFETCH(QString, blankedAnswer);
    QFETCH(QString, expectedYourAnswer);

    const QString result = BlankAnswer::yourAnswerResult(givenAnswer, blankedAnswer);

    QCOMPARE(result, expectedYourAnswer);
}

QTEST_MAIN(YourAnswerTest)

#include "youranswertest.moc"
