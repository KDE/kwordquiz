/*
    This file is part of KDevelop

    SPDX-FileCopyrightText: 2018 Friedrich W. H. Kossebau <kossebau@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "filterproxysearchline.h"

// KF
#include <KLocalizedString>
// Qt
#include <QSortFilterProxyModel>
#include <QTimer>


FilterProxySearchLine::FilterProxySearchLine(QWidget *parent)
    : QLineEdit(parent)
    , m_delayTimer(new QTimer(this))
{
    setClearButtonEnabled(true);
    setPlaceholderText(i18n("Search"));

    m_delayTimer->setSingleShot(true);
    m_delayTimer->setInterval(300);
    connect(m_delayTimer, &QTimer::timeout,
            this, &FilterProxySearchLine::updateFilter);
    connect(this, &FilterProxySearchLine::textChanged,
            m_delayTimer, static_cast<void(QTimer::*)()>(&QTimer::start));
}

void FilterProxySearchLine::setFilterProxyModel(QSortFilterProxyModel* filterProxyModel)
{
    m_filterProxyModel = filterProxyModel;
}

void FilterProxySearchLine::updateFilter()
{
    if (!m_filterProxyModel) {
        return;
    }

    m_filterProxyModel->setFilterFixedString(text());
}

#include "moc_filterproxysearchline.cpp"
