/***************************************************************************
                          kwordquizprefs.h  -  description
                             -------------------
    begin                : Sun Aug 18 2002
    copyright            : (C) 2002-2003 by Peter Hedlund
    email                : peter@peterandlinda.com
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef KWORDQUIZPREFS_H
#define KWORDQUIZPREFS_H

#include <kdialogbase.h>


/**
  *@author Peter Hedlund
*/

class PrefEditor;
class PrefQuiz; 
class PrefCharacter;
class DlgSpecChar;
  
class KWordQuizPrefs : public KDialogBase  {
   Q_OBJECT
public: 
  KWordQuizPrefs(QWidget *parent=0, const char *name=0, bool modal=true);
  ~KWordQuizPrefs();

  /** Write settings */
  void updateDialog();
  /** Read settings */
  void updateConfiguration();

public slots:
  /// Will be called when the "Default" button has been clicked.
  void slotDefault();
  /// Will be called when the "Apply" button has been clicked.
  void slotApply();
  /// Will be called whenever a setting was changed.
  void enableApply();
  /// Will be called when user selects a character
  void slotSpecChar(QChar);

signals:
  /// Will be emitted when the new settings should be applied.
  void settingsChanged();

private slots:
  void slotCharListSelectionChanged();  
  void slotDlgSpecCharClosed();  
  void slotSelectSpecChar();

  
private:
  PrefEditor *m_prefEditor;
  PrefQuiz *m_prefQuiz;
  PrefCharacter* m_prefCharacter;
  DlgSpecChar* m_dlgSpecChar;
};

#endif
