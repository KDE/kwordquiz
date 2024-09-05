// SPDX-FileCopyrightText: 2006-2010 Peter Hedlund <peter.hedlund@kdemail.net>
// SPDX-FileCopyrightText: 2023 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <QAbstractListModel>
#include <qsortfilterproxymodel.h>

#include "prefs.h"

class KWQCardModel;
class KWQSortFilterModel;

/**
  @author Peter Hedlund <peter.hedlund@kdemail.net>
*/
class KWQQuizModel : public QAbstractListModel
{
    Q_OBJECT

public:
    enum QuizIcon {
        IconLeftCol,
        IconRightCol,
        IconQuestion,
        IconCorrect,
        IconError,
    };

    explicit KWQQuizModel(QObject *parent = nullptr);

    void setFilterModel(KWQCardModel *sourceModel);
    KWQCardModel *sourceModel() const;

    bool init();

    void activateErrorList();
    void activateBaseList();

    void finish();
    void toNext();
    bool atEnd();
    bool checkAnswer(const QString &);
    bool hasErrors();
    QStringList multiOptions();
    QString quizIcon(QuizIcon ico);
    QString yourAnswer(const QString &) const;
    QString hint();

    Prefs::EnumStartSession::type quizType() const
    {
        return m_quizType;
    }
    void setQuizType(Prefs::EnumStartSession::type qt);

    int quizMode() const
    {
        return m_quizMode;
    }
    void setQuizMode(int qm);

    int questionCount();
    QString question();
    QString blankAnswer();
    QString answer();
    QString langQuestion();
    QString langAnswer();
    QPixmap imageQuestion();
    QPixmap imageAnswer();
    QUrl soundQuestion();
    QUrl soundAnswer();

    QString kbAnswer();

    QList<int> errorList() const
    {
        return m_errorList;
    }
    void removeLastError();

Q_SIGNALS:
    void checkingAnswer(int);

protected:
    bool lessThan(const QModelIndex &left, const QModelIndex &right) const override;

    // Disable from external calls; use setFilterModel() instead
    // (It's not even implemented so will give a link error anyway.)
    // void setSourceModel(QAbstractItemModel * sourceModel);

private:
    KWQSortFilterModel *m_sourceModel;

    QList<int> m_list;
    QList<int> m_errorList;
    QList<int> m_quizList;

    int m_quizMode;
    int m_currentQuestion;
    int m_questionCount;

    Prefs::EnumStartSession::type m_quizType;
    QString m_correctBlank;
    QString m_answerBlank;

    bool isOdd(int) const;
    int column(int);
};

#endif // KWQQUIZMODEL_H
