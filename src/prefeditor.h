//
// C++ Interface: prefeditor
//
// Description: 
//
//
// Author: Peter Hedlund <peter@peterandlinda.com>, (C) 2003
//
// Copyright: See COPYING file that comes with this distribution

#ifndef PREFEDITOR_H
#define PREFEDITOR_H

#include <prefeditorbase.h>

/**
@author Peter Hedlund
*/
class PrefEditor : public PrefEditorBase
{
Q_OBJECT
public:

  PrefEditor(QWidget *parent, const char *name=0, WFlags f=0);
  ~PrefEditor();

};

#endif
