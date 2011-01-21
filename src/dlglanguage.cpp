/***************************************************************************
                          dlglanguage.cpp  -  description
                             -------------------
   copyright       : (C) 2004-2009 Peter Hedlund <peter.hedlund@kdemail.net>

 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "dlglanguage.h"

#include <QtDBus/QDBusInterface>
#include <QtDBus/QDBusReply>

#include <KLocale>
#include <KLineEdit>
#include <KIconLoader>

#include "kwqtablemodel.h"
#include "prefs.h"

DlgLanguage::DlgLanguage(KWQTableModel *model, QWidget *parent): KDialog(parent)
{
    setCaption(i18n("Column Settings"));
    setButtons(Ok|Cancel);

    setupUi(mainWidget());

    column1Picture -> setPixmap(KIconLoader::global()->loadIcon("question", KIconLoader::Panel));
    column2Picture -> setPixmap(KIconLoader::global()->loadIcon("answer", KIconLoader::Panel));

    column1TitleLineEdit->completionObject(true)->setItems(Prefs::columnTitles1());
    column1TitleLineEdit->setCompletionMode(KGlobalSettings::CompletionAuto);

    column2TitleLineEdit->completionObject(true)->setItems(Prefs::columnTitles2());
    column2TitleLineEdit->setCompletionMode(KGlobalSettings::CompletionAuto);

    column1TitleLineEdit->setText(model->headerData(0, Qt::Horizontal, Qt::DisplayRole).toString());
    column2TitleLineEdit->setText(model->headerData(1, Qt::Horizontal, Qt::DisplayRole).toString());

    // keyboard layout
    // try to talk to kxbk - get a list of keyboard layouts
    QDBusInterface kxbk("org.kde.keyboard", "/Layouts", "org.kde.KeyboardLayouts");
    QDBusReply<QStringList> reply = kxbk.call("getLayoutsList");
    if (reply.isValid()) {
        QStringList layouts = reply;
        column1LayoutComboBox->addItems(layouts);
        column2LayoutComboBox->addItems(layouts);
        column1LayoutComboBox->setEnabled(true);
        column2LayoutComboBox->setEnabled(true);

        QString layout = model->headerData(0, Qt::Horizontal, KWQTableModel::KeyboardLayoutRole).toString();
        column1LayoutComboBox->setCurrentIndex(column1LayoutComboBox->findText(layout));

        layout = model->headerData(1, Qt::Horizontal, KWQTableModel::KeyboardLayoutRole).toString();
        column2LayoutComboBox->setCurrentIndex(column2LayoutComboBox->findText(layout));

    } else {
        kDebug() << "kxkb dbus error";
        column1LayoutComboBox->setEnabled(false);
        column2LayoutComboBox->setEnabled(false);
    }

    QSize s = model->headerData(0, Qt::Horizontal, Qt::SizeHintRole).toSize();
    column1WidthSpinBox->setValue(s.width());

    s = model->headerData(1, Qt::Horizontal, Qt::SizeHintRole).toSize();
    column2WidthSpinBox->setValue(s.width());

    column1TitleLineEdit->setFocus();
}

ColumnDataList DlgLanguage::columnData()
{
    ColumnDataList result;
    ColumnData cd;

    cd.identifier = column1TitleLineEdit->text();
    cd.layout = column1LayoutComboBox->currentText();
    cd.width = column1WidthSpinBox->value();
    result.append(cd);

    cd.identifier = column2TitleLineEdit->text();
    cd.layout = column2LayoutComboBox->currentText();
    cd.width = column2WidthSpinBox->value();
    result.append(cd);

    return result;
}

#include "dlglanguage.moc"
