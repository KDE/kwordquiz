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
#include <kglobalsettings.h>

const bool Configuration::m_defaultFirstRun = true;
const int Configuration::m_defaultEnterMove = 1;
const bool Configuration::m_defaultEnableBlanks = false;
const bool Configuration::m_defaultAutoFlip = false;
const int Configuration::m_defaultFlipDelay = 3;
const bool Configuration::m_defaultKeepDiscard = false;
const bool Configuration::m_defaultAutoCheck = true;
const bool Configuration::m_defaultHintError = false;
const bool Configuration::m_defaultPercent = false;
const int Configuration::m_defaultMode = 1;

Configuration::Configuration() {
  m_defaultEditorFont = KGlobalSettings::generalFont();
  read(); // read the settings or set them to the default values
};

void Configuration::read() {
    KConfig *conf=kapp->config();
    conf->setGroup("General");
    m_firstRun = conf->readBoolEntry("FirstRun", m_defaultFirstRun);
        
    conf->setGroup("Editor");
    m_enterMove = conf->readNumEntry("EnterMove", m_defaultEnterMove);
    m_enableBlanks = conf->readBoolEntry("EnableBlanks", m_defaultEnableBlanks);
    m_editorFont = conf->readFontEntry("EditorFont", &m_defaultEditorFont);
    
    conf->setGroup("Quiz");
    m_autoFlip = conf->readBoolEntry("AutoFlip", m_defaultAutoFlip);
    m_flipDelay = conf->readNumEntry("FlipDelay", m_defaultFlipDelay);
    m_keepDiscard = conf->readBoolEntry("KeepDiscard", m_defaultKeepDiscard);
    m_autoCheck = conf->readBoolEntry("AutoCheck", m_defaultAutoCheck);
    m_hintError = conf->readBoolEntry("HintError", m_defaultHintError);
    m_percent = conf->readBoolEntry("Percent", m_defaultPercent);
    m_mode = conf->readNumEntry("Mode", m_defaultMode);
    m_flashFont = conf->readFontEntry("FlashFont", &m_editorFont);
};

void Configuration::write() const {
    KConfig *conf=kapp->config();
    conf->setGroup("General");
    conf->writeEntry("FirstRun", false);
        
    conf->setGroup("Editor");
    conf->writeEntry("EnterMove", m_enterMove);
    conf->writeEntry("EnableBlanks", m_enableBlanks);
    conf->writeEntry("EditorFont", m_editorFont);
    
    conf->setGroup("Quiz");
    conf->writeEntry("AutoFlip", m_autoFlip);
    conf->writeEntry("FlipDelay", m_flipDelay);
    conf->writeEntry("KeepDiscard", m_keepDiscard);
    conf->writeEntry("AutoCheck", m_autoCheck);
    conf->writeEntry("HintError", m_hintError);
    conf->writeEntry("Percent", m_percent);
    conf->writeEntry("Mode", m_mode);
    conf->writeEntry("FlashFont", m_flashFont);
};

Configuration& Config() {
    static Configuration conf;
    return conf;
};


