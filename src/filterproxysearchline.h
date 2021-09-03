/*
    This file is part of KDevelop

    SPDX-FileCopyrightText: 2018 Friedrich W. H. Kossebau <kossebau@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KDEPROVIDER_FILTERPROXYSEARCHLINE_H
#define KDEPROVIDER_FILTERPROXYSEARCHLINE_H

// Qt
#include <QLineEdit>

class QTimer;
class QSortFilterProxyModel;

class FilterProxySearchLine : public QLineEdit
{
    Q_OBJECT

public:
    explicit FilterProxySearchLine(QWidget* parent = nullptr);

public:
    void setFilterProxyModel(QSortFilterProxyModel* filterProxyModel);

private:
    void updateFilter();

private:
    QTimer* m_delayTimer = nullptr;
    QSortFilterProxyModel* m_filterProxyModel = nullptr;
};

#endif
