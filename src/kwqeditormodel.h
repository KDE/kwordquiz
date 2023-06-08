// SPDX-FileCopyrightText: 2006-2010 Peter Hedlund <peter.hedlund@kdemail.net>
// SPDX-FileCopyrightText: 2023 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <QAbstractListModel>
#include <KImageCache>
#include <keduvocdocument.h>

/// @author Carl Schwan <carl@carlschwan.eu>
class KWQEditorModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(KEduVocDocument *document READ document WRITE setDocument NOTIFY documentChanged)
    Q_PROPERTY(QString title READ title WRITE setTitle NOTIFY titleChanged)
    Q_PROPERTY(QString author READ author WRITE setAuthor NOTIFY authorChanged)
    Q_PROPERTY(QString authorContact READ authorContact WRITE setAuthorContact NOTIFY authorContactChanged)
    Q_PROPERTY(QString license READ license WRITE setLicense NOTIFY licenseChanged)
    Q_PROPERTY(bool enabled READ enabled NOTIFY enabledChanged)

public:
    enum ExtraRoles {
        QuestionRole = Qt::DisplayRole + 1,
        QuestionImageRole,
        QuestionSoundRole,

        AnswerRole,
        AnswerImageRole,
        AnswerSoundRole,
    };

    explicit KWQEditorModel(QObject *parent = nullptr);

    bool enabled() const;

    KEduVocDocument *document() const;
    void setDocument(KEduVocDocument *document);

    QString title() const;
    void setTitle(const QString &title);

    QString author() const;
    void setAuthor(const QString &author);

    QString authorContact() const;
    void setAuthorContact(const QString &authorContact);

    QString license() const;
    void setLicense(const QString &license);

    int rowCount(const QModelIndex &parent = {}) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;

    Q_INVOKABLE void createNew();
    Q_INVOKABLE void add(const QString &question, const QString &answer);
    Q_INVOKABLE void edit(const int row, const QString &question, const QString &answer);
    Q_INVOKABLE bool save();

Q_SIGNALS:
    void enabledChanged();
    void documentChanged();
    void titleChanged();
    void authorChanged();
    void authorContactChanged();
    void licenseChanged();

private:
    KEduVocLesson *currentLesson(int row) const;
    KEduVocDocument *m_document = nullptr;
};
