//
// C++ Interface: configuration
//
// Description: 
//
//
// Author: Peter Hedlund <peter@peterandlinda.com>, (C) 2003
//
// Copyright: See COPYING file that comes with this distribution
//
#ifndef CONFIGURATION_H
#define CONFIGURATION_H

/**
@author Peter Hedlund
*/
class Configuration {
  public:
    /// Reads the configuration data from the application config file.
    /// If a property does not already exist in the config file it will be
    /// set to a default value.
    void read();
    /// Writes the configuration data to the application config file.
    void write() const;

    int m_enterMove;      ///< Direction of the enter key in the editor.
    bool m_enableBlanks;  ///< Enable fill-in-the-blank.
    bool m_autoFlip;      ///< Automatically flip card.
    int m_flipDelay;      ///< Time delay to flip card.
    bool m_keepDiscard;   ///< Keep or discard card.
    bool m_autoCheck;     ///< Automatically check multiple choice selection.
    bool m_hintError;     ///< Treat hint usage as error.
    bool m_percent;       ///< Show score as percent.

    static const int m_defaultEnterMove;
    static const bool m_defaultEnableBlanks;
    static const bool m_defaultAutoFlip;
    static const int m_defaultFlipDelay;
    static const bool m_defaultKeepDiscard;
    static const bool m_defaultAutoCheck;
    static const bool m_defaultHintError;
    static const bool m_defaultPercent;

  private:
    Configuration();
    Configuration(const Configuration&);

    // allow this function to create one instance
    friend Configuration& Config();
};

// use this function to access the settings
Configuration& Config();

#endif
