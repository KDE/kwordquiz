/*
    SPDX-FileCopyrightText: 2010 Peter Hedlund <peter.hedlund@kdemail.net>
    SPDX-License-Identifier: GPL-2.0-or-later
*/

// Own
#include "kwqcleardialog.h"

// Qt
#include <QPushButton>
#include <QDialogButtonBox>

// Kwq
#include "prefs.h"

KWQClearDialog::KWQClearDialog(QWidget* parent): QDialog(parent)
{
    setWindowTitle(i18nc("@title:window", "Clear Contents"));
    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok|QDialogButtonBox::Cancel);
    QWidget *mainWidget = new QWidget(this);
    QVBoxLayout *mainLayout = new QVBoxLayout;
    setLayout(mainLayout);
    mainLayout->addWidget(mainWidget);
    QPushButton *okButton = buttonBox->button(QDialogButtonBox::Ok);
    okButton->setDefault(true);
    okButton->setShortcut(Qt::CTRL | Qt::Key_Return);
    connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
    mainLayout->addWidget(buttonBox);
    setupUi(mainWidget);

    connect(clearButtonGroup, &QGroupBox::clicked, this, &KWQClearDialog::buttonGroupChanged);

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

void KWQClearDialog::accept()
{
  Prefs::setClearAll(clearAllCheckBox->isChecked());
  Prefs::setClearText(clearTextCheckBox->isChecked());
  Prefs::setClearImageLink(clearImageCheckBox->isChecked());
  Prefs::setClearSoundLink(clearSoundCheckBox->isChecked());
  QDialog::accept();
}
