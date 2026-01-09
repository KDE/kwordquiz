// SPDX-FileCopyrightText: 2007-2010 Peter Hedlund <peter.hedlund@kdemail.net>
// SPDX-License-Identifier: GPL-2.0-or-later

#include "kwqrandomsortmodel.h"
#include "prefs.h"

#include <KRandom>
#include <QRandomGenerator>

KWQRandomSortModel::KWQRandomSortModel(QObject *parent)
    : QSortFilterProxyModel(parent)
{
    m_sourceModel = nullptr;
    setSortCaseSensitivity(Qt::CaseInsensitive);
    setFilterCaseSensitivity(Qt::CaseInsensitive);
    setFilterKeyColumn(-1);
    m_restoreNativeOrder = false;
    m_shuffle = false;

    auto reset = [this] {
        this->reset();
        Prefs::self()->save();
    };

    connect(Prefs::self(), &Prefs::RandomizeChanged, this, reset);
    connect(Prefs::self(), &Prefs::QuestionInLeftColumnChanged, this, reset);
    connect(Prefs::self(), &Prefs::QuestionInRightColumnChanged, this, reset);
}

QVariant KWQRandomSortModel::data(const QModelIndex &idx, int role) const
{
    auto mode = idx.row() >= m_modes.count() ? QuestionInLeftColumn : m_modes[idx.row()];

    if (role == MultipleChoiceRole) {
        if (rowCount() < 4) {
            return QStringList();
        }

        const auto rowRole = mode == QuestionInRightColumn ? KWQCardModel::QuestionRole : KWQCardModel::AnswerRole;

        QList<int> possibleIndex;
        for (int i = 0; i < rowCount(); i++) {
            possibleIndex << i;
        }
        possibleIndex.removeAll(idx.row());

        KRandom::shuffle(possibleIndex, QRandomGenerator::global());

        QStringList possibleAnswer;
        possibleAnswer << QSortFilterProxyModel::data(index(possibleIndex[0], 0), rowRole).toString();
        possibleAnswer << QSortFilterProxyModel::data(index(possibleIndex[1], 0), rowRole).toString();
        possibleAnswer << QSortFilterProxyModel::data(index(possibleIndex[3], 0), rowRole).toString();
        possibleAnswer << QSortFilterProxyModel::data(index(idx.row(), 0), rowRole).toString();

        KRandom::shuffle(possibleAnswer, QRandomGenerator::global());

        return QVariant::fromValue(possibleAnswer);
    }

    if (mode == QuestionInRightColumn) {
        switch (role) {
        case KWQCardModel::AnswerRole:
            role = KWQCardModel::QuestionRole;
            break;
        case KWQCardModel::AnswerSoundRole:
            role = KWQCardModel::QuestionSoundRole;
            break;
        case KWQCardModel::AnswerImageRole:
            role = KWQCardModel::QuestionImageRole;
            break;
        case KWQCardModel::QuestionRole:
            role = KWQCardModel::AnswerRole;
            break;
        case KWQCardModel::QuestionSoundRole:
            role = KWQCardModel::AnswerSoundRole;
            break;
        case KWQCardModel::QuestionImageRole:
            role = KWQCardModel::AnswerImageRole;
            break;
        }
    }

    return QSortFilterProxyModel::data(idx, role);
}

QHash<int, QByteArray> KWQRandomSortModel::roleNames() const
{
    auto roles = QSortFilterProxyModel::roleNames();
    roles[MultipleChoiceRole] = "multipleChoice";
    return roles;
}

void KWQRandomSortModel::setCardModel(KWQCardModel *sourceModel)
{
    if (m_sourceModel == sourceModel) {
        return;
    }
    if (m_sourceModel) {
        disconnect(m_sourceModel, &KWQCardModel::modelReset, this, &KWQRandomSortModel::reset);
    }
    m_sourceModel = sourceModel;
    QSortFilterProxyModel::setSourceModel(sourceModel);
    Q_EMIT cardModelChanged();

    if (m_sourceModel) {
        connect(m_sourceModel, &KWQCardModel::modelReset, this, &KWQRandomSortModel::reset);
    }
    reset();
}

KWQCardModel *KWQRandomSortModel::cardModel() const
{
    return m_sourceModel;
}

bool KWQRandomSortModel::lessThan(const QModelIndex &left, const QModelIndex &right) const
{
    if (m_shuffle) {
        return m_shuffleList.at(right.row()) < m_shuffleList.at(left.row());
    } else if (m_restoreNativeOrder) {
        return sourceModel()->index(right.row(), right.column()).row() < sourceModel()->index(left.row(), left.column()).row();
    } else {
        return QSortFilterProxyModel::lessThan(left, right);
    }
}

bool KWQRandomSortModel::filterAcceptsRow(const int sourceRow, const QModelIndex &sourceParent) const
{
    if (!m_showErrorsOnly) {
        return true;
    }

    return m_errors.contains(sourceRow);
}

void KWQRandomSortModel::setRandomModes()
{
    m_modes.clear();
    if (Prefs::questionInLeftColumn() && !Prefs::questionInRightColumn()) {
        for (int i = 0; i < rowCount({}); ++i) {
            m_modes.append(QuestionInLeftColumn);
        }
    } else if (!Prefs::questionInLeftColumn() && Prefs::questionInRightColumn()) {
        for (int i = 0; i < rowCount({}); ++i) {
            m_modes.append(QuestionInRightColumn);
        }
    } else {
        auto generator = QRandomGenerator::global();
        for (int i = 0; i < rowCount({}); ++i) {
            if (generator->bounded(2) == 1) {
                m_modes.append(QuestionInLeftColumn);
            } else {
                m_modes.append(QuestionInRightColumn);
            }
        }
    }
}

void KWQRandomSortModel::reset()
{
    if (Prefs::randomize()) {
        shuffle();
    } else {
        restoreNativeOrder();
    }
}

void KWQRandomSortModel::restoreNativeOrder()
{
    m_errors.clear();
    m_restoreNativeOrder = true;
    setRandomModes();
    sort(-1, Qt::AscendingOrder);
    invalidate();
    m_restoreNativeOrder = false;
    m_modes.clear();
}

void KWQRandomSortModel::shuffle()
{
    m_errors.clear();
    m_shuffleList.clear();
    setRandomModes();
    for (int i = 0; i < rowCount({}); ++i)
        m_shuffleList.append(i);

    KRandom::shuffle(m_shuffleList, QRandomGenerator::global());
    m_shuffle = true;
    sort(0, Qt::AscendingOrder);
    invalidate();
    m_shuffle = false;
}

void KWQRandomSortModel::markAsError(const int row)
{
    m_errors << mapToSource(index(row, 0)).row();
}

void KWQRandomSortModel::unMarkAsError(const int row)
{
    m_errors.removeAll(mapToSource(index(row, 0)).row());
}

bool KWQRandomSortModel::showErrorsOnly() const
{
    return m_showErrorsOnly;
}

void KWQRandomSortModel::setShowErrorsOnly(const bool showErrorsOnly)
{
    if (m_showErrorsOnly == showErrorsOnly) {
        return;
    }
    m_showErrorsOnly = showErrorsOnly;
    Q_EMIT showErrorsOnlyChanged();
    invalidateFilter();
}

#include "moc_kwqrandomsortmodel.cpp"
