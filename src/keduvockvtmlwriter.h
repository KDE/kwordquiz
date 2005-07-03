/***************************************************************************
                     export a KEduVocDocument to a KVTML file
    -----------------------------------------------------------------------
    copyright            : (C) 1999-2001 Ewald Arnold
                           (C) 2001 The KDE-EDU team
                           (C) 2005 Eric Pignet
    email                : eric at erixpage.com
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef KEDUVOCKVTMLWRITER_H
#define KEDUVOCKVTMLWRITER_H

#include <qfile.h>
#include <qdom.h>

//#include "keduvocdocument.h"
#include "grammarmanager.h"
#include "multiplechoice.h"

class KEduVocDocument;

// internal types, indented are subtypes

#define QM_VERB           "v"    // go
#define   QM_VERB_IRR     "ir"
#define   QM_VERB_REG     "re"
#define QM_NOUN           "n"    // table, coffee
#define   QM_NOUN_F       "f"
#define   QM_NOUN_M       "m"
#define   QM_NOUN_S       "s"
#define QM_NAME           "nm"
#define QM_ART            "ar"   // article
#define   QM_ART_DEF      "def"  // definite    a/an
#define   QM_ART_IND      "ind"  // indefinite  the
#define QM_ADJ            "aj"   // adjective   expensive, good
#define QM_ADV            "av"   // adverb      today, strongly
#define QM_PRON           "pr"   // pronoun     you, she
#define   QM_PRON_POS     "pos"  // possessive  my, your
#define   QM_PRON_PER     "per"  // personal
#define QM_PHRASE         "ph"
#define QM_NUM            "num"  // numeral
#define   QM_NUM_ORD      "ord"  // ordinal     first, second
#define   QM_NUM_CARD     "crd"  // cardinal    one, two
#define QM_INFORMAL       "ifm"
#define QM_FIG            "fig"
#define QM_CON            "con"  // conjuncton  and, but
#define QM_PREP           "pre"  // preposition behind, between
#define QM_QUEST          "qu"   // question    who, what

// type delimiters

#define QM_USER_TYPE  "#"   // designates number of user type
#define QM_TYPE_DIV   ":"   // divide main from subtype

/**
@author Eric Pignet
*/
class KEduVocKvtmlWriter
{
public:
  KEduVocKvtmlWriter(QFile *file);
  ~KEduVocKvtmlWriter();

  bool writeDoc(KEduVocDocument *doc, const QString &generator);

  bool saveLessonKvtMl (QDomDocument &domDoc, QDomElement &domElementParent);
  bool saveTypeNameKvtMl (QDomDocument &domDoc, QDomElement &domElementParent);
  bool saveTenseNameKvtMl (QDomDocument &domDoc, QDomElement &domElementParent);
  bool saveUsageNameKvtMl (QDomDocument &domDoc, QDomElement &domElementParent);
  bool saveOptionsKvtMl  (QDomDocument &domDoc, QDomElement &domElementParent);
  bool saveArticleKvtMl  (QDomDocument &domDoc, QDomElement &domElementParent);
  bool saveConjugHeader  (QDomDocument &domDoc, QDomElement &domElementParent,
                          vector<Conjugation> &curr_conjug);
  bool saveConjug        (QDomDocument &domDoc, QDomElement &domElementParent,
                          const Conjugation &curr_conjug, QString type);
  bool saveConjugEntry   (QDomDocument &domDoc, QDomElement &domElementParent,
                          Conjugation &curr_conjug);

  bool saveComparison    (QDomDocument &domDoc, QDomElement &domElementParent,
                          const Comparison &comp);

  bool saveMultipleChoice(QDomDocument &domDoc, QDomElement &domElementParent,
                          const MultipleChoice &mc);

private:
  QFile *m_outputFile;
  KEduVocDocument *m_doc;
};

#endif
