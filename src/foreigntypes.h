// SPDX-FileCopyrightText: 2026 Carl Schwan <carlschwan@kde.org>
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <KEduVocDocument>
#include <qqmlintegration.h>

struct DocumentForeign {
    Q_GADGET
    QML_FOREIGN(KEduVocDocument)
    QML_ANONYMOUS
};
