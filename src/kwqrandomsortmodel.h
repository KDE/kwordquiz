// SPDX-FileCopyrightText: 2007-2010 Peter Hedlund <peter.hedlund@kdemail.net>
// SPDX-FileCopyrightText: 2023 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "kwqcardmodel.h"
#include <QSortFilterProxyModel>

/// Randomize the order of the items contained inside the KWQCardModel
///
/// \author Peter Hedlund <peter.hedlund@kdemail.net>
class KWQRandomSortModel : public QSortFilterProxyModel
{
    Q_OBJECT
    Q_PROPERTY(KWQCardModel *cardModel READ cardModel WRITE setCardModel NOTIFY cardModelChanged)
    Q_PROPERTY(bool showErrorsOnly READ showErrorsOnly WRITE setShowErrorsOnly NOTIFY showErrorsOnlyChanged)

public:
    enum ExtraRoles {
        MultipleChoiceRole = KWQCardModel::ExtraUserRoles + 1,
    };

    explicit KWQRandomSortModel(QObject *parent = nullptr);

    KWQCardModel *cardModel() const;
    void setCardModel(KWQCardModel *cardModel);

    bool showErrorsOnly() const;
    void setShowErrorsOnly(const bool showErrorsOnly);

    QVariant data(const QModelIndex &index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;

public Q_SLOTS:
    void reset();
    void restoreNativeOrder();
    void shuffle();
    void markAsError(const int row);
    void unMarkAsError(const int row);

Q_SIGNALS:
    void cardModelChanged();
    void showErrorsOnlyChanged();

protected:
    bool lessThan(const QModelIndex &left, const QModelIndex &right) const override;
    bool filterAcceptsRow(const int sourceRow, const QModelIndex &sourceParent) const override;

private:
    KWQCardModel *m_sourceModel;

    enum Mode {
        QuestionInLeftColumn = 1,
        QuestionInRightColumn = 2,
    };
    void setRandomModes();

    bool m_showErrorsOnly = false;
    bool m_restoreNativeOrder = false;
    bool m_shuffle = false;
    QList<int> m_shuffleList;
    QList<int> m_errors;
    QList<Mode> m_modes;
};
