//
// C++ Interface: dlglanguage
//
// Description: 
//
//
// Author: Peter Hedlund <peter@peterandlinda.com>, (C) 2004
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef DLGLANGUAGE_H
#define DLGLANGUAGE_H

#include <qwidget.h>
#include <kdialogbase.h>

#include "dlglanguagebase.h"

/**
@author Peter Hedlund
*/
class DlgLanguage : public KDialogBase {
    Q_OBJECT
  public:
    DlgLanguage(QWidget *parent = 0, const char *name = 0, bool modal=true);
    ~DlgLanguage();

    void setLanguage(int index, const QString &lang);
    QString Language(int index);

  private:
    DlgLanguageBase *dlgBase;

};

#endif
