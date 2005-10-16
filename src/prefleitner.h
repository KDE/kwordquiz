//
// C++ Interface: prefleitner
//
// Description: the part of the preferences to change the settings for the LeitnerSystem
//
//
// Author: Martin Pfeiffer <martin-pfeiffer-bensheim@web.de>, (C) 2005
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef PREFLEITNER_H
#define PREFLEITNER_H

#include <prefleitnerbase.h>

class LeitnerSystemView;
class LeitnerSystem;
class LeitnerBox;

/**
@author Martin Pfeiffer
*/
class PrefLeitner : public QDialog, public Ui::PrefLeitnerBase
{
	Q_OBJECT
	
public:
	PrefLeitner(QWidget * parent, LeitnerSystem* system);

	LeitnerSystem* getSystem();

public slots:
	void slotCorrectWord( const QString& newBox );
	void slotWrongWord( const QString& newBox );
	void slotBoxName( const QString& newName );
	void slotAddBox();
	void slotDeleteBox();
	void slotDiscard();
	void slotApply();

private slots:
	void slotBoxClicked(int);		//catches the signal from the view if user clicks on box

private:
	LeitnerSystemView* m_leitnerSystemView;	//the LeitnerSystemView which shows the selected system
	LeitnerSystem* m_selectedSystem;	//the currently selected system to be changed
	LeitnerBox* m_selectedBox;		//the currently selected box

	void refreshSystemView();		//refresh the LeitnerSystemView
	void newSystem();
};

#endif
