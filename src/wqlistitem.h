//
// C++ Interface: wqlistitem
//
// Description: 
//
//
// Author: Peter Hedlund <peter@peterandlinda.com>, (C) 2003
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef WQLISTITEM_H
#define WQLISTITEM_H


/**
@author Peter Hedlund
*/

class WQListItem{
public:
    WQListItem();

    ~WQListItem();
  void setQuestion(int i);
  void setCorrect(int c);
  void setOneOp(int i);
  void setTwoOp(int i);
  void setThreeOp(int i);

  int question() {return m_question;}
  int correct() const {return m_correct;}
  int oneOp() {return m_oneOp;}
  int twoOp() {return m_twoOp;}
  int threeOp() {return m_threeOp;}

private:
  int m_question; //the column containing the question
  int m_correct;
  int m_oneOp; //the row for correct or option 1
  int m_twoOp; //the row for option 2
  int m_threeOp; //the row for option 3

};

#endif
