/*
    SPDX-FileCopyrightText: 2022 Nicolas Fella <nicolas.fella@gmx.de>
    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include <QTest>

#include "../src/blankanswer.h"

class BlankTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void testBlank_data();
    void testBlank();
};

void BlankTest::testBlank_data()
{
    QTest::addColumn<QString>("input");
    QTest::addColumn<QString>("blankedAnswer");
    QTest::addColumn<QString>("correctAnswer");

    QTest::addRow("empty") << ""
                           << ""
                           << "";
    QTest::addRow("no_blank") << "Just a sentence"
                              << ""
                              << "";
    QTest::addRow("one_blank") << "This is [one] test"
                               << "This is .......... test"
                               << "one";
    QTest::addRow("two_blank") << "This is [one] test with [two] blanks"
                               << "This is .......... test with .......... blanks"
                               << "one; two";
    QTest::addRow("two_words") << "[Two words] in one blank"
                               << ".......... in one blank"
                               << "Two words";
}

void BlankTest::testBlank()
{
    QFETCH(QString, input);
    QFETCH(QString, blankedAnswer);
    QFETCH(QString, correctAnswer);

    BlankAnswer::BlankResult result = BlankAnswer::blankAnswer(input);

    QCOMPARE(result.blankedAnswer, blankedAnswer);
    QCOMPARE(result.correctAnswer, correctAnswer);
}

QTEST_MAIN(BlankTest)

#include "blanktest.moc"
