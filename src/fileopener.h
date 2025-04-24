// SPDX-FileCopyrightText: 2023 Carl Schwan <carlschwan@kde.org>
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "prefs.h"
#include <QObject>
#include <qqmlintegration.h>

class FileOpener : public QObject
{
    Q_OBJECT
    QML_ELEMENT

public:
    Q_INVOKABLE void openFile(const QUrl &file, const Prefs::EnumStartSession mode);

Q_SIGNALS:
    void fileOpened(const QUrl &file, int mode);
};
