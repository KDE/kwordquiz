/***************************************************************************
                     read a KEduVocDocument from a KVTML file
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

#ifndef KEDUVOCKVTMLREADER_H
#define KEDUVOCKVTMLREADER_H

#include <qfile.h>
#include <qdom.h>

#include "keduvocdocument.h"
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

// usage delimiters (also declared in UsageManager.h)

#define UL_USER_USAGE  "#"   // designates number of user type

/**
@author Eric Pignet
*/
class KEduVocKvtmlReader : public QObject
{
public:
	KEduVocKvtmlReader(QFile *file);
	~KEduVocKvtmlReader();

	bool readDoc(KEduVocDocument *doc);

	bool readLesson(QDomElement &domElementParent);
	bool readArticle(QDomElement &domElementParent);
	bool readConjug(QDomElement &domElementParent,
									vector<Conjugation> &curr_conjug,
                  const QString &entry_tag);
	bool readOptions(QDomElement &domElementParent);
	bool readType(QDomElement &domElementParent);
	bool readTense(QDomElement &domElementParent);
	bool readUsage(QDomElement &domElementParent);
	bool readComparison(QDomElement &domElementParent,
                      Comparison &comp);
	bool readMultipleChoice(QDomElement &domElementParent,
                          MultipleChoice &mc);
	bool readExpressionChildAttributes(	QDomElement &domElementExpressionChild,
														          QString &lang,
														          grade_t &grade, grade_t &rev_grade,
														          int &count, int &rev_count,
														          time_t &date, time_t &rev_date,
														          QString &remark,
														          int &bcount, int &rev_bcount,
														          QString &query_id,
														          QString &pronunce,
														          int &width,
														          QString &type,
														          QString &faux_ami_f,
														          QString &faux_ami_t,
														          QString &synonym,
														          QString &example,
														          QString &antonym,
														          QString &usage,
														          QString &paraphrase);
	bool readExpression(QDomElement &domElementParent);
	bool readBody(QDomElement &domElementParent);

	void domErrorUnknownElement(const QString &elem);
	void domError(const QString &text );

private:
  QFile *m_inputFile;
  KEduVocDocument *m_doc;
};

#endif
