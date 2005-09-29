/* This file is part of KWordQuiz
  Copyright (C) 2003 Peter Hedlund <peter.hedlund@kdemail.net>

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Library General Public
  License version 2 as published by the Free Software Foundation.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Library General Public License for more details.

  You should have received a copy of the GNU Library General Public License
  along with this library; see the file COPYING.LIB.  If not, write to
  the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
  Boston, MA 02110-1301, USA.
*/

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
