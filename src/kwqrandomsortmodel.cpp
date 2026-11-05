// SPDX-FileCopyrightText: 2007-2010 Peter Hedlund <peter.hedlund@kdemail.net>
// SPDX-License-Identifier: GPL-2.0-or-later

#include "kwqrandomsortmodel.h"
#include "prefs.h"

#include <KConfigGroup>
#include <KRandom>
#include <QCryptographicHash>
#include <QDateTime>
#include <QJsonArray>
#include <QJsonDocument>
#include <QRandomGenerator>
#include <algorithm>
#include <utility>

KWQRandomSortModel::KWQRandomSortModel(QObject *parent)
    : QSortFilterProxyModel(parent)
    , m_practiceConfig(KSharedConfig::openConfig(QStringLiteral("kwordquiz-practice"), KConfig::SimpleConfig))
{
    m_sourceModel = nullptr;
    setSortCaseSensitivity(Qt::CaseInsensitive);
    setFilterCaseSensitivity(Qt::CaseInsensitive);
    setFilterKeyColumn(-1);
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
    if (!idx.isValid()) {
        return {};
    }
    const int sourceRow = mapToSource(idx).row();
    const auto mode = m_modes.value(sourceRow, QuestionInLeftColumn);

    if (role == MultipleChoiceRole) {
        if (!m_sourceModel || m_sourceModel->rowCount() < 4) {
            return QStringList();
        }

        const auto rowRole = mode == QuestionInRightColumn ? KWQCardModel::QuestionRole : KWQCardModel::AnswerRole;

        QList<int> possibleIndex;
        for (int i = 0; i < m_sourceModel->rowCount(); i++) {
            possibleIndex << i;
        }
        possibleIndex.removeAll(sourceRow);

        KRandom::shuffle(possibleIndex, QRandomGenerator::global());

        QStringList possibleAnswer;
        possibleAnswer << m_sourceModel->index(possibleIndex[0], 0).data(rowRole).toString();
        possibleAnswer << m_sourceModel->index(possibleIndex[1], 0).data(rowRole).toString();
        possibleAnswer << m_sourceModel->index(possibleIndex[2], 0).data(rowRole).toString();
        possibleAnswer << m_sourceModel->index(sourceRow, 0).data(rowRole).toString();

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
    if (m_smartPractice) {
        return m_practiceOrder.value(left.row()) < m_practiceOrder.value(right.row());
    } else if (m_shuffle) {
        return m_shuffleList.value(right.row()) < m_shuffleList.value(left.row());
    } else {
        return QSortFilterProxyModel::lessThan(left, right);
    }
}

bool KWQRandomSortModel::filterAcceptsRow(const int sourceRow, const QModelIndex &) const
{
    return (!m_smartPractice || m_practiceRows.contains(sourceRow)) && (!m_showErrorsOnly || m_reviewRows.contains(sourceRow));
}

void KWQRandomSortModel::setRandomModes()
{
    m_modes.resize(m_sourceModel ? m_sourceModel->rowCount() : 0);
    if (Prefs::questionInLeftColumn() && !Prefs::questionInRightColumn()) {
        std::ranges::fill(m_modes, QuestionInLeftColumn);
    } else if (!Prefs::questionInLeftColumn() && Prefs::questionInRightColumn()) {
        std::ranges::fill(m_modes, QuestionInRightColumn);
    } else {
        std::ranges::generate(m_modes, [] {
            return QRandomGenerator::global()->bounded(2) == 1 ? QuestionInLeftColumn : QuestionInRightColumn;
        });
    }
}

void KWQRandomSortModel::preparePractice()
{
    m_practiceKeys.clear();
    m_practiceRows.clear();
    m_practiceOrder.clear();
    if (!m_sourceModel || !m_sourceModel->document()) {
        return;
    }

    const auto document = m_sourceModel->document();
    QString deck = document->url().toString(QUrl::FullyEncoded);
    if (deck.isEmpty()) {
        // Unsaved decks must not share history just because they have the same title.
        QJsonArray cards;
        for (int row = 0; row < m_sourceModel->rowCount(); ++row) {
            const auto index = m_sourceModel->index(row, 0);
            cards.append(QJsonArray{index.data(KWQCardModel::QuestionRole).toString(), index.data(KWQCardModel::AnswerRole).toString()});
        }
        deck = QString::fromLatin1(QCryptographicHash::hash(QJsonDocument(cards).toJson(QJsonDocument::Compact), QCryptographicHash::Sha256).toHex());
    }

    struct Card {
        int row;
        int category;
        qint64 lastPracticed;
    };
    QList<Card> cards;
    for (int row = 0; row < m_sourceModel->rowCount(); ++row) {
        const auto index = m_sourceModel->index(row, 0);
        const QJsonArray identity{deck, index.data(KWQCardModel::QuestionRole).toString(), index.data(KWQCardModel::AnswerRole).toString()};
        const auto key =
            QString::fromLatin1(QCryptographicHash::hash(QJsonDocument(identity).toJson(QJsonDocument::Compact), QCryptographicHash::Sha256).toHex());
        m_practiceKeys.append(key);
        const KConfigGroup history(m_practiceConfig, key);
        const bool practiced = history.hasKey("LastPracticed");
        const bool weak = history.readEntry("NeedsPractice", false);
        cards.append({row, weak ? 0 : practiced ? 2 : 1, history.readEntry("LastPracticed", qint64(0))});
    }
    std::ranges::stable_sort(cards, {}, [](const Card &card) {
        return std::pair{card.category, card.lastPracticed};
    });
    m_practiceOrder.resize(cards.size());
    for (int rank = 0; rank < cards.size(); ++rank) {
        m_practiceOrder[cards[rank].row] = rank;
        if (rank < 20) {
            m_practiceRows.insert(cards[rank].row);
        }
    }
}

void KWQRandomSortModel::recordAnswer(int sourceRow, bool correct)
{
    if (sourceRow < 0 || sourceRow >= m_practiceKeys.size() || m_answeredRows.contains(sourceRow)) {
        return;
    }
    m_answeredRows.insert(sourceRow);
    KConfigGroup history(m_practiceConfig, m_practiceKeys[sourceRow]);
    const int streak = correct ? std::min(2, history.readEntry("CorrectStreak", 0) + 1) : 0;
    history.writeEntry("CorrectStreak", streak);
    history.writeEntry("NeedsPractice", !correct || (history.readEntry("NeedsPractice", false) && streak < 2));
    history.writeEntry("LastPracticed", QDateTime::currentMSecsSinceEpoch());
    history.sync();
}

void KWQRandomSortModel::reset()
{
    m_errors.clear();
    m_reviewRows.clear();
    m_answeredRows.clear();
    if (m_showErrorsOnly) {
        m_showErrorsOnly = false;
        Q_EMIT showErrorsOnlyChanged();
    }
    m_smartPractice = Prefs::smartPractice();
    setRandomModes();
    preparePractice();
    m_shuffleList.clear();
    for (int row = 0; m_sourceModel && row < m_sourceModel->rowCount(); ++row) {
        m_shuffleList.append(row);
    }
    KRandom::shuffle(m_shuffleList, QRandomGenerator::global());
    m_shuffle = Prefs::randomize();
    sort(m_smartPractice || m_shuffle ? 0 : -1, Qt::AscendingOrder);
    invalidate();
}

void KWQRandomSortModel::restoreNativeOrder()
{
    reset();
    m_smartPractice = false;
    m_shuffle = false;
    sort(-1);
    invalidate();
}

void KWQRandomSortModel::shuffle()
{
    reset();
    m_shuffle = true;
    sort(0);
    invalidate();
}

void KWQRandomSortModel::markAsError(const int row)
{
    const auto sourceIndex = mapToSource(index(row, 0));
    if (!sourceIndex.isValid()) {
        return;
    }
    if (!m_errors.contains(sourceIndex.row())) {
        m_errors.append(sourceIndex.row());
    }
    recordAnswer(sourceIndex.row(), false);
}

void KWQRandomSortModel::unMarkAsError(const int row)
{
    const auto sourceIndex = mapToSource(index(row, 0));
    if (!sourceIndex.isValid()) {
        return;
    }
    m_errors.removeAll(sourceIndex.row());
    recordAnswer(sourceIndex.row(), true);
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
    m_reviewRows = QSet<int>(m_errors.begin(), m_errors.end());
    m_answeredRows.clear();
    Q_EMIT showErrorsOnlyChanged();
    invalidate();
}

#include "moc_kwqrandomsortmodel.cpp"
