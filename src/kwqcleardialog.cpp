/*
    This file is part of KWordQuiz
    Copyright (C) 2010 Peter Hedlund <peter.hedlund@kdemail.net>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

*/

#include "kwqcleardialog.h"

#include "prefs.h"

KWQClearDialog::KWQClearDialog(QWidget* parent): KDialog(parent)
{
    setCaption(i18n("Clear Contents"));
    setButtons(Ok|Cancel);
    setupUi(mainWidget());

    connect(clearButtonGroup, SIGNAL(changed(int)), this, SLOT(buttonGroupChanged(int)));

    updateCheckBoxes();
}

void KWQClearDialog::buttonGroupChanged(int id)
{
    Q_UNUSED(id);
    Prefs::setClearAll(clearAllCheckBox->isChecked());
    Prefs::setClearText(clearTextCheckBox->isChecked());
    Prefs::setClearImageLink(clearImageCheckBox->isChecked());
    Prefs::setClearSoundLink(clearSoundCheckBox->isChecked());
    updateCheckBoxes();
}

void KWQClearDialog::updateCheckBoxes()
{
    bool all = Prefs::clearAll();
    clearAllCheckBox->setChecked(all);
    clearTextCheckBox->setChecked(Prefs::clearText());
    clearImageCheckBox->setChecked(Prefs::clearImageLink());
    clearSoundCheckBox->setChecked(Prefs::clearSoundLink());

    clearTextCheckBox->setEnabled(!all);
    clearImageCheckBox->setEnabled(!all);
    clearSoundCheckBox->setEnabled(!all);
}

void KWQClearDialog::slotButtonClicked(int button)
{
    if (button == KDialog::Ok) {
      Prefs::setClearAll(clearAllCheckBox->isChecked());
      Prefs::setClearText(clearTextCheckBox->isChecked());
      Prefs::setClearImageLink(clearImageCheckBox->isChecked());
      Prefs::setClearSoundLink(clearSoundCheckBox->isChecked());
      accept();
    }
    else
        KDialog::slotButtonClicked(button);
}

#include "kwqcleardialog.moc"
