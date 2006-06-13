/***************************************************************************
                          dlgspecchar.h  -  description
                             -------------------
   copyright       : (C) 2003-2005 Peter Hedlund <peter.hedlund@kdemail.net>

 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef DLGSPECCHAR_H
#define DLGSPECCHAR_H

#include <kdialog.h>

class QWidget;
class QGridLayout;
class QPushButton;
class KCharSelect;
class KButtonBox;

/**
@author Peter Hedlund
*/
class DlgSpecChar : public KDialog
{
Q_OBJECT
public:
    //constructor when you want to insert multi char
    DlgSpecChar( QWidget *parent, const char *name, const QString &_font,
                     const QChar &_chr, bool _modal=true );

    // internal
    QChar chr();

    void closeDialog();

private:
    void initDialog(const QChar &_chr, const QString &_font, bool _enableFont);

private slots:
    void slotUser1();
    void slotDoubleClicked();

protected:
    // dialog objects
    QGridLayout *grid;
    KButtonBox  *bbox;
    QPushButton *bOk, *bCancel;
    KCharSelect *charSelect;

 signals:
    void insertChar(QChar);
};

#endif
