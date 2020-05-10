/***************************************************************************
                               kwqquizmodel.h
                             -------------------

    copyright            : (C) 2008-2010 by Peter Hedlund
    email                : peter.hedlund@kdemail.net

 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef KWQQUIZMODEL_H
#define KWQQUIZMODEL_H

#include <QSortFilterProxyModel>

#include "prefs.h"

/**
  @author Peter Hedlund <peter.hedlund@kdemail.net>
*/

class KWQSortFilterModel;

class KWQQuizModel : public QSortFilterProxyModel
{
    Q_OBJECT
public:
    enum QuizIcon {IconLeftCol, IconRightCol, IconQuestion, IconCorrect, IconError};

    explicit KWQQuizModel(QObject *parent);

    void setFilterModel(KWQSortFilterModel * sourceModel);
    KWQSortFilterModel * sourceModel() const;

    bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const override;

    bool init();

    void activateErrorList();
    void activateBaseList();

    void finish();
    void toNext();
    bool atEnd();
    bool checkAnswer(const QString & );
    bool hasErrors();
    QStringList multiOptions();
    QString quizIcon(QuizIcon ico);
    QString yourAnswer(const QString & );
    QString hint();

    Prefs::EnumStartSession::type quizType() const {return m_quizType;}
    void setQuizType(Prefs::EnumStartSession::type qt);

    int quizMode() const {return m_quizMode;}
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

    QList<int> errorList() const {return m_errorList;}
    void removeLastError();

signals:
  void checkingAnswer(int );

protected:
    bool lessThan(const QModelIndex & left, const QModelIndex & right) const override;

    // Disable from external calls; use setFilterModel() instead
    // (It's not even implemented so will give a link error anyway.)
    //void setSourceModel(QAbstractItemModel * sourceModel);

private:
    KWQSortFilterModel * m_sourceModel;

    QList<int> m_list;
    QList<int> m_errorList;
    QList<int> m_quizList;

    int m_quizMode;
    int m_currentQuestion;
    int m_questionCount;

    Prefs::EnumStartSession::type m_quizType;
    QString m_correctBlank;
    QString m_answerBlank;

    bool isOdd(int ) const;
    int column(int );
};

#endif // KWQQUIZMODEL_H
