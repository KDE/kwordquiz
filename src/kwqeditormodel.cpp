// SPDX-FileCopyrightText: 2006-2010 Peter Hedlund <peter.hedlund@kdemail.net>
// SPDX-FileCopyrightText: 2023 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "kwqeditormodel.h"
#include <keduvocdocument.h>
#include <keduvoclesson.h>
#include "keduvocexpression.h"
#include "kwordquiz_version.h"

#include <KLocalizedString>
#include <QStandardPaths>
#include <QFileDialog>
#include <QPointer>
#include <QDir>
#include <QRegularExpression>

KWQEditorModel::KWQEditorModel(QObject *parent)
    : QAbstractListModel(parent)
{
}

void KWQEditorModel::createNew()
{
    auto doc = new KEduVocDocument(this);
    doc->setGenerator(QStringLiteral("kwordquiz %1").arg(KWORDQUIZ_VERSION_STRING));
    doc->appendIdentifier();
    doc->identifier(0).setName(i18n("Question"));
    // doc->identifier(0).setLocale("en");
    doc->appendIdentifier();
    doc->identifier(1).setName(i18n("Answer"));
    // doc->identifier(1).setLocale("en");

    setDocument(doc);
}

void KWQEditorModel::add(const QString &question, const QString &answer)
{
    Q_ASSERT(m_document);

    beginInsertRows({}, rowCount(), rowCount());
    m_document->lesson()->appendEntry(new KEduVocExpression({ question, answer }));
    endInsertRows();
}

void KWQEditorModel::edit(const int row, const QString &question, const QString &answer)
{
    Q_ASSERT(m_document);

    if (question.length() == 0 && answer.length() == 0) {
        beginRemoveRows({}, row, row);
        const auto entry = m_document->lesson()->entries(KEduVocLesson::Recursive).value(row);
        entry->lesson()->removeEntry(entry);
        endRemoveRows();
        delete entry;

        return;
    }

    auto expression = m_document->lesson()->entries(KEduVocLesson::Recursive).value(row);
    expression->setTranslation(0, question);
    expression->setTranslation(1, answer);

    Q_EMIT dataChanged(index(row, 0), index(row, 0));
}
static QString extractSuffixFromQtPattern(const QString &qtPattern)
{
  static const QRegularExpression regexp(".*\\((.*)\\)");
  const QRegularExpressionMatch match = regexp.match(qtPattern);
  if (!match.hasMatch()) {
    qWarning() << "extractSuffixesFromQtPattern regexp match failed" << qtPattern;
    return { QStringLiteral(".report_bug_please") };
  }
  return match.captured(1);
}

bool KWQEditorModel::save()
{
    Q_ASSERT(m_document);

    QUrl url;

    if (m_document->url().isEmpty()) {
        const auto dir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
        url = QUrl::fromLocalFile(QDir::toNativeSeparators(dir + QLatin1Char('/') + m_document->title() + QStringLiteral(".kvtml")));

        // Create parent in case it doesn't exist
        QDir newDir(dir);
        if (!newDir.mkpath(dir)) {
            return false;
        }
    } else {
        url = m_document->url();
    }


    auto result = m_document->saveAs(url, KEduVocDocument::Automatic);
    return result == KEduVocDocument::NoError;
}

KEduVocDocument *KWQEditorModel::document() const
{
    return m_document;
}

void KWQEditorModel::setDocument(KEduVocDocument *document)
{
    if (document == m_document) {
        return;
    }
    beginResetModel();
    m_document = document;
    endResetModel();

    Q_EMIT documentChanged();
    Q_EMIT enabledChanged();
    Q_EMIT titleChanged();
    Q_EMIT authorContact();
    Q_EMIT authorContactChanged();
    Q_EMIT licenseChanged();
}

QString KWQEditorModel::title() const
{
    return m_document ? m_document->title() : QString{};
}

void KWQEditorModel::setTitle(const QString &title)
{
    Q_ASSERT(m_document);

    if (title == m_document->title()) {
        return;
    }
    m_document->setTitle(title);

    Q_EMIT titleChanged();
}

QString KWQEditorModel::author() const
{
    return m_document ? m_document->author() : QString{};
}

void KWQEditorModel::setAuthor(const QString &author)
{
    Q_ASSERT(m_document);

    if (author == m_document->author()) {
        return;
    }
    m_document->setAuthor(author);
    Q_EMIT authorChanged();
}

QString KWQEditorModel::authorContact() const
{
    return m_document ? m_document->authorContact() : QString{};
}

void KWQEditorModel::setAuthorContact(const QString &authorContact)
{
    Q_ASSERT(m_document);

    if (authorContact == m_document->authorContact()) {
        return;
    }
    m_document->setAuthorContact(authorContact);
    Q_EMIT authorContactChanged();
}

QString KWQEditorModel::license() const
{
    return m_document ? m_document->license() : QString{};
}

void KWQEditorModel::setLicense(const QString &license)
{
    Q_ASSERT(m_document);

    if (license == m_document->license()) {
        return;
    }
    m_document->setLicense(license);
    Q_EMIT licenseChanged();
}

bool KWQEditorModel::enabled() const
{
    return m_document;
}

int KWQEditorModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid() || !m_document) {
        return 0;
    }

    return m_document->lesson()->entryCount(KEduVocLesson::Recursive);
}

QVariant KWQEditorModel::data(const QModelIndex &index, int role) const
{
    Q_ASSERT(checkIndex(index, QAbstractItemModel::CheckIndexOption::IndexIsValid));

    const auto entry = m_document->lesson()->entries(KEduVocLesson::Recursive).value(index.row());

    switch (role) {
    case Qt::DisplayRole:
      return QString(entry->translation(0)->text() + " ->" + entry->translation(1)->text());

    case QuestionRole:
        return entry->translation(0)->text();
    case QuestionImageRole:
        return entry->translation(0)->imageUrl().toLocalFile();
    case QuestionSoundRole:
        return entry->translation(0)->soundUrl().toLocalFile();

    case AnswerRole:
        return entry->translation(1)->text();
    case AnswerImageRole:
        return entry->translation(1)->imageUrl().toLocalFile();
    case AnswerSoundRole:
        return entry->translation(1)->soundUrl().toLocalFile();

    default:
        return {};
    }
}

QHash<int, QByteArray> KWQEditorModel::roleNames() const
{
    return {
        { QuestionRole, "question" },
        { QuestionImageRole, "questionImage" },
        { QuestionSoundRole, "questionSound" },

        { AnswerRole, "answer" },
        { AnswerImageRole, "answerImage" },
        { AnswerSoundRole, "answerSound" },
    };
}
