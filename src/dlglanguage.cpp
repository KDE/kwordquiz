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

    QWidget * w = new QWidget();
    setMainWidget(w);

    dlgBase = new Ui::ColumnDialogBase();
    dlgBase->setupUi(mainWidget());

    dlgBase -> column1Picture -> setPixmap(KIconLoader::global()->loadIcon("question", KIconLoader::Panel));
    dlgBase -> column2Picture -> setPixmap(KIconLoader::global()->loadIcon("answer", KIconLoader::Panel));

    dlgBase->column1TitleLineEdit->completionObject(true)->setItems(Prefs::columnTitles1());
    dlgBase->column1TitleLineEdit->setCompletionMode(KGlobalSettings::CompletionAuto);

    dlgBase->column2TitleLineEdit->completionObject(true)->setItems(Prefs::columnTitles2());
    dlgBase->column2TitleLineEdit->setCompletionMode(KGlobalSettings::CompletionAuto);

    dlgBase->column1TitleLineEdit->setText(model->headerData(0, Qt::Horizontal, Qt::DisplayRole).toString());
    dlgBase->column2TitleLineEdit->setText(model->headerData(1, Qt::Horizontal, Qt::DisplayRole).toString());

    // keyboard layout
    // try to talk to kxbk - get a list of keyboard layouts
    QDBusInterface kxbk("org.kde.kxkb", "/kxkb", "org.kde.KXKB");
    QDBusReply<QStringList> reply = kxbk.call("getLayoutsList");
    if (reply.isValid()) {
        QStringList layouts = reply;
        dlgBase->column1LayoutComboBox->addItems(layouts);
        dlgBase->column2LayoutComboBox->addItems(layouts);
        dlgBase->column1LayoutComboBox->setEnabled(true);
        dlgBase->column2LayoutComboBox->setEnabled(true);

        QString layout = model->headerData(0, Qt::Horizontal, KWQTableModel::KeyboardLayoutRole).toString();
        dlgBase->column1LayoutComboBox->setCurrentIndex(dlgBase->column1LayoutComboBox->findText(layout));

        layout = model->headerData(1, Qt::Horizontal, KWQTableModel::KeyboardLayoutRole).toString();
        dlgBase->column2LayoutComboBox->setCurrentIndex(dlgBase->column2LayoutComboBox->findText(layout));

    } else {
        kDebug() << "kxkb dbus error";
        dlgBase->column1LayoutComboBox->setEnabled(false);
        dlgBase->column2LayoutComboBox->setEnabled(false);
    }

    QSize s = model->headerData(0, Qt::Horizontal, Qt::SizeHintRole).toSize();
    dlgBase->column1WidthSpinBox->setValue(s.width());

    s = model->headerData(1, Qt::Horizontal, Qt::SizeHintRole).toSize();
    dlgBase->column2WidthSpinBox->setValue(s.width());

    dlgBase->column1TitleLineEdit->setFocus();
}

ColumnDataList DlgLanguage::columnData()
{
    ColumnDataList result;
    ColumnData cd;

    cd.identifier = dlgBase->column1TitleLineEdit->text();
    cd.layout = dlgBase->column1LayoutComboBox->currentText();
    cd.width = dlgBase->column1WidthSpinBox->value();
    result.append(cd);

    cd.identifier = dlgBase->column2TitleLineEdit->text();
    cd.layout = dlgBase->column2LayoutComboBox->currentText();
    cd.width = dlgBase->column2WidthSpinBox->value();
    result.append(cd);

    return result;
}

#include "dlglanguage.moc"
