//
// C++ Implementation: configuration
//
// Description: 
//
//
// Author: Peter Hedlund <peter@peterandlinda.com>, (C) 2003
//
// Copyright: See COPYING file that comes with this distribution
//
#include "configuration.h"

#include <kapplication.h>       // for 'kapp'
#include <kconfig.h>            // for KConfig

const int Configuration::m_defaultEnterMove = 1;
const bool Configuration::m_defaultEnableBlanks = false;
const bool Configuration::m_defaultAutoFlip = false;
const int Configuration::m_defaultFlipDelay = 3;
const bool Configuration::m_defaultKeepDiscard = false;
const bool Configuration::m_defaultAutoCheck = true;
const bool Configuration::m_defaultHintError = false;
const bool Configuration::m_defaultPercent = false;


Configuration::Configuration() {
    read(); // read the settings or set them to the default values
};

void Configuration::read() {
    KConfig *conf=kapp->config();
    
    conf->setGroup("Editor");
    m_enterMove = conf->readNumEntry("EnterMove", m_defaultEnterMove);
    m_enableBlanks = conf->readBoolEntry("EnableBlanks", m_defaultEnableBlanks);

    conf->setGroup("Quiz");
    m_autoFlip = conf->readBoolEntry("AutoFlip", m_defaultAutoFlip);
    m_flipDelay = conf->readNumEntry("FlipDelay", m_defaultFlipDelay);
    m_keepDiscard = conf->readBoolEntry("KeepDiscard", m_defaultKeepDiscard);
    m_autoCheck = conf->readBoolEntry("AutoCheck", m_defaultAutoCheck);
    m_hintError = conf->readBoolEntry("HintError", m_defaultHintError);
    m_percent = conf->readBoolEntry("Percent", m_defaultPercent);
};

void Configuration::write() const {
    KConfig *conf=kapp->config();
    
    conf->setGroup("Editor");
    conf->writeEntry("EnterMove", m_enterMove);
    conf->writeEntry("EnableBlanks", m_enableBlanks);

    conf->setGroup("Quiz");
    conf->writeEntry("AutoFlip", m_autoFlip);
    conf->writeEntry("FlipDelay", m_flipDelay);
    conf->writeEntry("KeepDiscard", m_keepDiscard);
    conf->writeEntry("AutoCheck", m_autoCheck);
    conf->writeEntry("HintError", m_hintError);
    conf->writeEntry("Percent", m_percent);
};

Configuration& Config() {
    static Configuration conf;
    return conf;
};


