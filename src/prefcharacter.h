//
// C++ Interface: prefcharacter
//
// Description: 
//
//
// Author: Peter Hedlund <peter@peterandlinda.com>, (C) 2004
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef PREFCHARACTER_H
#define PREFCHARACTER_H

#include <prefcharacterbase.h>

/**
@author Peter Hedlund
*/
class PrefCharacter : public PrefCharacterBase
{
Q_OBJECT
public:
    PrefCharacter(QWidget *parent = 0, const char *name = 0);

    ~PrefCharacter();

};

#endif
