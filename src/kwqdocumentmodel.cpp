// SPDX-FileCopyrightText: 2023 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: GPL-2.0-or-later

#include "kwqdocumentmodel.h"
#include "stateprefs.h"
#include <KEduVocDocument>

constexpr KWQDocumentModel::DocumentStatus mapErrorCodeToDocumentStatus(KEduVocDocument::ErrorCode errorCode)
{
    switch (errorCode) {
    case KEduVocDocument::ErrorCode::NoError:
        return KWQDocumentModel::DocumentStatus::NoError;
    case KEduVocDocument::ErrorCode::FileTypeUnknown:
    case KEduVocDocument::ErrorCode::FileReaderFailed:
    case KEduVocDocument::ErrorCode::FileDoesNotExist:
    case KEduVocDocument::ErrorCode::Unknown:
    case KEduVocDocument::ErrorCode::InvalidXml:
    case KEduVocDocument::ErrorCode::FileCannotRead:
        return KWQDocumentModel::DocumentStatus::CannotLoadFile;
    case KEduVocDocument::ErrorCode::FileIsReadOnly:
    case KEduVocDocument::ErrorCode::FileCannotWrite:
    case KEduVocDocument::ErrorCode::FileWriterFailed:
    case KEduVocDocument::ErrorCode::FileLocked:
    case KEduVocDocument::ErrorCode::FileCannotLock:
        return KWQDocumentModel::DocumentStatus::CannotSaveFile;
    }
    return KWQDocumentModel::DocumentStatus::NoError;
};

KWQDocumentModel::KWQDocumentModel(QObject *parent)
    : QAbstractListModel(parent)
{
    load();
}

KWQDocumentModel::~KWQDocumentModel()
{
    save();
}

void KWQDocumentModel::load()
{
    const auto urls = StatePrefs::documents();
    if (urls.empty()) {
        add(QUrl::fromLocalFile(QStandardPaths::locate(QStandardPaths::GenericDataLocation, QStringLiteral("kwordquiz/examples/example.kvtml"))));
        add(QUrl::fromLocalFile(QStandardPaths::locate(QStandardPaths::GenericDataLocation, QStringLiteral("kwordquiz/examples/french_verbs.kvtml"))));
        add(QUrl::fromLocalFile(QStandardPaths::locate(QStandardPaths::GenericDataLocation, QStringLiteral("kwordquiz/examples/fill_in_the_blank.kvtml"))));
        add(QUrl::fromLocalFile(
            QStandardPaths::locate(QStandardPaths::GenericDataLocation, QStringLiteral("kwordquiz/examples/us_states_and_capitals.kvtml"))));

        return;
    }

    for (const auto &url : urls) {
        auto doc = std::make_unique<KEduVocDocument>(nullptr);
        auto error = doc->open(QUrl(url), KEduVocDocument::FileIgnoreLock);
        m_documents.push_back(std::make_pair(std::move(doc), mapErrorCodeToDocumentStatus(error)));
    }
}

void KWQDocumentModel::save() const
{
    QStringList urls;
    for (const auto &document : std::as_const(m_documents)) {
        urls << document.first->url().url();
    }
    StatePrefs::setDocuments(urls);
    StatePrefs::self()->save();
}

int KWQDocumentModel::rowCount(const QModelIndex &parent) const
{
    return parent.isValid() ? 0 : m_documents.size();
}

QVariant KWQDocumentModel::data(const QModelIndex &index, int role) const
{
    Q_ASSERT(checkIndex(index, QAbstractItemModel::CheckIndexOption::IndexIsValid));

    const auto &document = m_documents[index.row()];

    switch (role) {
    case Qt::DisplayRole:
    case TitleRole:
        return document.first->title();

    case UrlRole:
        return document.first->url().url();

    case DocumentRole:
        return QVariant::fromValue(document.first.get());

    case DocumentStatusRole:
        return QVariant::fromValue(document.second);
    default:
        return {};
    }
}

QHash<int, QByteArray> KWQDocumentModel::roleNames() const
{
    return {
        {TitleRole, "title"},
        {UrlRole, "url"},
        {DocumentRole, "document"},
        {DocumentStatusRole, "documentStatus"},
    };
}

void KWQDocumentModel::addDocument(KEduVocDocument *document)
{
    document->setParent(nullptr);
    const int row = rowCount();
    beginInsertRows({}, row, row);
    m_documents.push_back(std::make_pair(std::unique_ptr<KEduVocDocument>(document), DocumentStatus::NoError));
    endInsertRows();

    save();
}

int KWQDocumentModel::add(const QUrl &url)
{
    int row = 0;
    for (const auto &doc : std::as_const(m_documents)) {
        if (doc.first->url() == url) {
            qDebug() << row;
            return row;
        }
        row++;
    }

    auto doc = std::make_unique<KEduVocDocument>(nullptr);
    auto error = doc->open(url, KEduVocDocument::FileIgnoreLock);

    row = rowCount();
    beginInsertRows({}, row, row);
    m_documents.push_back(std::make_pair(std::move(doc), mapErrorCodeToDocumentStatus(error)));
    endInsertRows();

    save();

    return row;
}

void KWQDocumentModel::remove(const int row)
{
    beginRemoveRows({}, row, row);
    m_documents.erase(std::begin(m_documents) + row);
    endRemoveRows();
}

void KWQDocumentModel::entryChanged(const KNSCore::Entry &entry)
{
    if (entry.status() == KNSCore::Entry::Deleted) {
        const auto uninstalledFiles = entry.uninstalledFiles();
        for (const auto &url : uninstalledFiles) {
            auto it = std::find_if(cbegin(m_documents), cend(m_documents), [url](const auto &document) {
                return document.first->url() == QUrl(url);
            });

            if (it == cend(m_documents)) {
                continue;
            }

            beginRemoveRows({}, cend(m_documents) - it, cend(m_documents) - it);
            m_documents.erase(it);
            endInsertRows();
        }
    } else if (entry.status() == KNSCore::Entry::Installed) {
        qDebug() << entry.installedFiles();
        const auto installedFiles = entry.installedFiles();
        beginInsertRows({}, rowCount(), rowCount() + entry.installedFiles().count() - 1);
        for (const auto &url : installedFiles) {
            auto doc = std::make_unique<KEduVocDocument>(nullptr);
            auto error = doc->open(QUrl(QStringLiteral("file:") + url), KEduVocDocument::FileIgnoreLock);
            m_documents.push_back(std::make_pair(std::move(doc), mapErrorCodeToDocumentStatus(error)));
        }
        endInsertRows();
    }
}

#include "moc_kwqdocumentmodel.cpp"
