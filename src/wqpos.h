//
// C++ Interface: wqpos
//
// Description: 
//
//
// Author: Peter Hedlund <peter@peterandlinda.com>, (C) 2003
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef WQPOS_H
#define WQPOS_H

/**
@author Peter Hedlund
*/
class Q_EXPORT WQPos
{
public:
  WQPos();
	WQPos(int c, int r);

	int c() const {return m_c;}
	int r() const {return m_r;}
private:
	int m_c;
  int m_r;
};



#endif
