//
// C++ Interface: wqprintdialogpage
//
// Description: 
//
//
// Author: Peter Hedlund <peter@peterandlinda.com>, (C) 2004
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef WQPRINTDIALOGPAGE_H
#define WQPRINTDIALOGPAGE_H


#include <kdeprint/kprintdialogpage.h>

/**
Print dialog page to provide specific print options for KWordQuiz

@author Peter Hedlund
*/
class WQPrintDialogPage : public KPrintDialogPage
{
Q_OBJECT
public:
    WQPrintDialogPage( QWidget *parent = 0, const char *name = 0 );

    ~WQPrintDialogPage();
   //reimplement virtual functions
   void getOptions( QMap<QString,QString>& opts, bool incldef = false );
   void setOptions( const QMap<QString,QString>& opts );
   bool isValid( QString& msg );

private:
  QButtonGroup * g;
  QRadioButton * rb0;
  QRadioButton * rb1;
  QRadioButton * rb2;       
};

#endif
