// SPDX-FileCopyrightText: 2023 Carl Schwan <carlschwan@kde.org>
// SPDX-License-Identifier: GPL-2.0-or-later

#include "fileopener.h"

void FileOpener::openFile(const QUrl &file, const Prefs::EnumStartSession mode)
{
    Q_EMIT fileOpened(file, mode);
}
