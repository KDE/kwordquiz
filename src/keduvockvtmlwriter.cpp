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

#include <kdebug.h>
#include <qdom.h>
#include <qtextstream.h>

#include "keduvockvtmlwriter.h"
#include "keduvocdocument.h"

KEduVocKvtmlWriter::KEduVocKvtmlWriter(QFile *file)
{
  // the file must be already open
  m_outputFile = file;
}

KEduVocKvtmlWriter::~KEduVocKvtmlWriter()
{
}

bool KEduVocKvtmlWriter::saveTypeNameKvtMl (QDomDocument &domDoc, QDomElement &domElementParent)
{
  if (m_doc->m_typeDescriptions.size() == 0)
    return true;

  QDomElement domElementType = domDoc.createElement(KV_TYPE_GRP);

  for (int lfn = 0; lfn < (int) m_doc->m_typeDescriptions.size(); lfn++)
  {
    if (!(m_doc->m_typeDescriptions[lfn].isNull()) )
    {
      QDomElement domElementDesc = domDoc.createElement(KV_TYPE_DESC);
      QDomText domTextDesc = domDoc.createTextNode(m_doc->m_typeDescriptions[lfn]);

      domElementDesc.setAttribute(KV_TYPE_NO, lfn+1);
      domElementDesc.appendChild(domTextDesc);
      domElementType.appendChild(domElementDesc);
    }
  }

  domElementParent.appendChild(domElementType);
  return true;
}


bool KEduVocKvtmlWriter::saveTenseNameKvtMl (QDomDocument &domDoc, QDomElement &domElementParent)
{
  if (m_doc->m_tenseDescriptions.size() == 0)
    return true;

  QDomElement domElementTense = domDoc.createElement(KV_TENSE_GRP);

  for (int lfn = 0; lfn < (int) m_doc->m_tenseDescriptions.size(); lfn++)
  {
    if (!(m_doc->m_tenseDescriptions[lfn].isNull()) ) {
      QDomElement domElementDesc = domDoc.createElement(KV_TENSE_DESC);
      QDomText domTextDesc = domDoc.createTextNode(m_doc->m_tenseDescriptions[lfn]);

      domElementDesc.setAttribute(KV_TENSE_NO, lfn+1);
      domElementDesc.appendChild(domTextDesc);
      domElementTense.appendChild(domElementDesc);
    }
  }

  domElementParent.appendChild(domElementTense);
  return true;
}


bool KEduVocKvtmlWriter::saveUsageNameKvtMl (QDomDocument &domDoc, QDomElement &domElementParent)
{
  if (m_doc->m_usageDescriptions.size() == 0)
    return true;

  QDomElement domElementUsage = domDoc.createElement(KV_USAGE_GRP);

  for (int lfn = 0; lfn < (int) m_doc->m_usageDescriptions.size(); lfn++)
  {
    if (!(m_doc->m_usageDescriptions[lfn].isNull()) )
    {
      QDomElement domElementDesc = domDoc.createElement(KV_USAGE_DESC);
      QDomText domTextDesc = domDoc.createTextNode(m_doc->m_usageDescriptions[lfn]);

      domElementDesc.setAttribute(KV_USAGE_NO, lfn+1);
      domElementDesc.appendChild(domTextDesc);
      domElementUsage.appendChild(domElementDesc);
    }
  }

  domElementParent.appendChild(domElementUsage);
  return true;
}


bool KEduVocKvtmlWriter::saveLessonKvtMl (QDomDocument &domDoc, QDomElement &domElementParent)
{
  if (m_doc->m_lessonDescriptions.size() == 0)
    return true;

  QDomElement domElementLesson = domDoc.createElement(KV_LESS_GRP);
  domElementLesson.setAttribute(KV_SIZEHINT, m_doc->sizeHint(-1));

  for (int lfn = 0; lfn < (int) m_doc->m_lessonDescriptions.size(); lfn++)
  {
    if (!(m_doc->m_lessonDescriptions[lfn].isNull()) )
    {
      QDomElement domElementDesc = domDoc.createElement(KV_LESS_DESC);
      QDomText domTextDesc = domDoc.createTextNode(m_doc->m_lessonDescriptions[lfn]);

      domElementDesc.setAttribute(KV_LESS_NO, lfn+1);
      if (m_doc->currentLesson() == lfn+1)
        domElementDesc.setAttribute (KV_LESS_CURR, 1);
      if (lfn < (int) m_doc->m_lessonsInQuery.size() && m_doc->m_lessonsInQuery[lfn])
        domElementDesc.setAttribute (KV_LESS_QUERY, 1);

      domElementDesc.appendChild(domTextDesc);
      domElementLesson.appendChild(domElementDesc);
    }
  }

  domElementParent.appendChild(domElementLesson);
  return true;
}


bool KEduVocKvtmlWriter::saveConjug(QDomDocument &domDoc, QDomElement &domElementParent,
                                    const Conjugation &curr_conjug, QString type)
{
  if (!curr_conjug.pers1Singular(type).isEmpty() )
  {
    QDomElement domElementP1s = domDoc.createElement(KV_CON_P1S);
    QDomText domTextP1s = domDoc.createTextNode(curr_conjug.pers1Singular(type));

    domElementP1s.appendChild(domTextP1s);
    domElementParent.appendChild(domElementP1s);
  }

  if (!curr_conjug.pers2Singular(type).isEmpty() )
  {
    QDomElement domElementP2s = domDoc.createElement(KV_CON_P2S);
    QDomText domTextP2s = domDoc.createTextNode(curr_conjug.pers2Singular(type));

    domElementP2s.appendChild(domTextP2s);
    domElementParent.appendChild(domElementP2s);
  }

  if (!curr_conjug.pers3FemaleSingular(type).isEmpty() ||
    curr_conjug.pers3SingularCommon(type))
  {
    QDomElement domElementP3sf = domDoc.createElement(KV_CON_P3SF);
    QDomText domTextP3sf = domDoc.createTextNode(curr_conjug.pers3FemaleSingular(type));

    if (curr_conjug.pers3SingularCommon(type))
      domElementP3sf.setAttribute(KV_CONJ_COMMON, 1);

    domElementP3sf.appendChild(domTextP3sf);
    domElementParent.appendChild(domElementP3sf);
  }

  if (!curr_conjug.pers3MaleSingular(type).isEmpty() )
  {
    QDomElement domElementP3sm = domDoc.createElement(KV_CON_P3SM);
    QDomText domTextP3sm = domDoc.createTextNode(curr_conjug.pers3MaleSingular(type));

    domElementP3sm.appendChild(domTextP3sm);
    domElementParent.appendChild(domElementP3sm);
  }

  if (!curr_conjug.pers3NaturalSingular(type).isEmpty() )
  {
    QDomElement domElementP3sn = domDoc.createElement(KV_CON_P3SN);
    QDomText domTextP3sn = domDoc.createTextNode(curr_conjug.pers3NaturalSingular(type));

    domElementP3sn.appendChild(domTextP3sn);
    domElementParent.appendChild(domElementP3sn);
  }

  if (!curr_conjug.pers1Plural(type).isEmpty() )
  {
    QDomElement domElementP1p = domDoc.createElement(KV_CON_P1P);
    QDomText domTextP1p = domDoc.createTextNode(curr_conjug.pers1Plural(type));

    domElementP1p.appendChild(domTextP1p);
    domElementParent.appendChild(domElementP1p);
  }

  if (!curr_conjug.pers2Plural(type).isEmpty() )
  {
    QDomElement domElementP2p = domDoc.createElement(KV_CON_P2P);
    QDomText domTextP2p = domDoc.createTextNode(curr_conjug.pers2Plural(type));

    domElementP2p.appendChild(domTextP2p);
    domElementParent.appendChild(domElementP2p);
  }

  if (!curr_conjug.pers3FemalePlural(type).isEmpty() ||
    curr_conjug.pers3PluralCommon(type))
  {
    QDomElement domElementP3pf = domDoc.createElement(KV_CON_P3PF);
    QDomText domTextP3pf = domDoc.createTextNode(curr_conjug.pers3FemalePlural(type));

    if (curr_conjug.pers3PluralCommon(type))
      domElementP3pf.setAttribute(KV_CONJ_COMMON, 1);

    domElementP3pf.appendChild(domTextP3pf);
    domElementParent.appendChild(domElementP3pf);
  }

  if (!curr_conjug.pers3MalePlural(type).isEmpty() )
  {
    QDomElement domElementP3pm = domDoc.createElement(KV_CON_P3PM);
    QDomText domTextP3pm = domDoc.createTextNode(curr_conjug.pers3MalePlural(type));

    domElementP3pm.appendChild(domTextP3pm);
    domElementParent.appendChild(domElementP3pm);
  }

  if (!curr_conjug.pers3NaturalPlural(type).isEmpty() )
  {
    QDomElement domElementP3pn = domDoc.createElement(KV_CON_P3PN);
    QDomText domTextP3pn = domDoc.createTextNode(curr_conjug.pers3NaturalPlural(type));

    domElementP3pn.appendChild(domTextP3pn);
    domElementParent.appendChild(domElementP3pn);
  }

  return true;
}

bool KEduVocKvtmlWriter::saveConjugHeader(QDomDocument &domDoc, QDomElement &domElementParent,
                                          QValueList<Conjugation> &curr_conjug)
{
/*
 <conjugation>    used in header for definiton of "prefix"
  <e l="de">      lang determines also lang order in entries !!
   <s1>I</s1>     which must NOT differ
   <s2>you<2>
   <s3f common="0">he</s3f>
   <s3m>she</s3m>
   <s3n>it</s3n>
   <p1>we</p1>
   <p2>you</p2>
   <p3f common="1">they</p3f>
   <p3m>they</p3m>
   <p3n>they</p3n>
  </e>
 </conjugation>

*/
  if (curr_conjug.size() == 0)
    return true;

  QDomElement domElementConjug = domDoc.createElement(KV_CONJUG_GRP);
  QString s;

  for (int ent = 0; ent < QMIN((int) curr_conjug.size(), m_doc->numLanguages()); ent++)
  {
    QDomElement domElementEntry = domDoc.createElement(KV_CON_ENTRY);

    if (ent == 0)
    {
      s = m_doc->originalIdentifier().stripWhiteSpace();  //EPT le Ident doit �re superflu
      if (s.isEmpty() )
        s = "original";
    }
    else
    {
      s = m_doc->identifier(ent).stripWhiteSpace();
      if (s.isEmpty() )
      {
        s.setNum(ent);
        s.insert(0, "translation ");
      }
    }
    domElementEntry.setAttribute(KV_LANG, s);

    if (!saveConjug (domDoc, domElementEntry, curr_conjug[ent], CONJ_PREFIX))
      return false;

    domElementConjug.appendChild(domElementEntry);
  }

  domElementParent.appendChild(domElementConjug);
  return true;
}


bool KEduVocKvtmlWriter::saveComparison(QDomDocument &domDoc, QDomElement &domElementParent,
                                        const Comparison &comp)
/*
 <comparison>
   <l1>good</l1>
   <l2>better</l2>
   <l3>best</l3>
 </comparison>
*/
{
  if (comp.isEmpty())
    return true;

  QDomElement domElementComparison = domDoc.createElement(KV_COMPARISON_GRP);

  if (!comp.l1().isEmpty() )
  {
    QDomElement domElementL1 = domDoc.createElement(KV_COMP_L1);
    QDomText domTextL1 = domDoc.createTextNode(comp.l1());

    domElementL1.appendChild(domTextL1);
    domElementComparison.appendChild(domElementL1);
  }

  if (!comp.l2().isEmpty() )
  {
    QDomElement domElementL2 = domDoc.createElement(KV_COMP_L2);
    QDomText domTextL2 = domDoc.createTextNode(comp.l2());

    domElementL2.appendChild(domTextL2);
    domElementComparison.appendChild(domElementL2);
  }

  if (!comp.l3().isEmpty() )
  {
    QDomElement domElementL3 = domDoc.createElement(KV_COMP_L3);
    QDomText domTextL3 = domDoc.createTextNode(comp.l3());

    domElementL3.appendChild(domTextL3);
    domElementComparison.appendChild(domElementL3);
  }

  domElementParent.appendChild(domElementComparison);
  return true;
}


bool KEduVocKvtmlWriter::saveMultipleChoice(QDomDocument &domDoc, QDomElement &domElementParent,
                                            const MultipleChoice &mc)
/*
 <multiplechoice>
   <mc1>good</mc1>
   <mc2>better</mc2>
   <mc3>best</mc3>
   <mc4>best 2</mc4>
   <mc5>best 3</mc5>
 </multiplechoice>
*/
{
  if (mc.isEmpty())
    return true;

  QDomElement domElementMC = domDoc.createElement(KV_MULTIPLECHOICE_GRP);

  if (!mc.mc1().isEmpty() )
  {
    QDomElement domElementMC1 = domDoc.createElement(KV_MC_1);
    QDomText domTextMC1 = domDoc.createTextNode(mc.mc1());

    domElementMC1.appendChild(domTextMC1);
    domElementMC.appendChild(domElementMC1);
  }

  if (!mc.mc2().isEmpty() )
  {
    QDomElement domElementMC2 = domDoc.createElement(KV_MC_2);
    QDomText domTextMC2 = domDoc.createTextNode(mc.mc2());

    domElementMC2.appendChild(domTextMC2);
    domElementMC.appendChild(domElementMC2);
  }

  if (!mc.mc3().isEmpty() )
  {
    QDomElement domElementMC3 = domDoc.createElement(KV_MC_3);
    QDomText domTextMC3 = domDoc.createTextNode(mc.mc3());

    domElementMC3.appendChild(domTextMC3);
    domElementMC.appendChild(domElementMC3);
  }

  if (!mc.mc4().isEmpty() )
  {
    QDomElement domElementMC4 = domDoc.createElement(KV_MC_4);
    QDomText domTextMC4 = domDoc.createTextNode(mc.mc4());

    domElementMC4.appendChild(domTextMC4);
    domElementMC.appendChild(domElementMC4);
  }

  if (!mc.mc5().isEmpty() )
  {
    QDomElement domElementMC5 = domDoc.createElement(KV_MC_5);
    QDomText domTextMC5 = domDoc.createTextNode(mc.mc5());

    domElementMC5.appendChild(domTextMC5);
    domElementMC.appendChild(domElementMC5);
  }

  domElementParent.appendChild(domElementMC);
  return true;
}


bool KEduVocKvtmlWriter::saveConjugEntry( QDomDocument &domDoc, QDomElement &domElementParent,
                                          Conjugation &curr_conjug)
/*
 <conjugation>    in entry for definition of tenses of (irreg.) verbs
  <t n="sipa">
   <s1>go</s1>
   <s2>go</s2>
   <s3f>goes</s3f>
   <s3m>goes</s3m>
   <s3n>goes</s3n>
   <p1>go</p1>
   <p2>go</p2>
   <p3f>go</p3f>
   <p3m>go</p3m>
   <p3n>go</p3n>
  </t>
 </conjugation>
*/
{
  curr_conjug.cleanUp();
  if (curr_conjug.numEntries() == 0 )
    return true;

  QDomElement domElementConjug = domDoc.createElement(KV_CONJUG_GRP);
  QString type;

  for (int lfn = 0; lfn < (int) curr_conjug.numEntries(); lfn++)
  {
    QDomElement domElementType = domDoc.createElement(KV_CON_TYPE);

    type = curr_conjug.getType(lfn);
    domElementType.setAttribute(KV_CON_NAME, type);

    if (!saveConjug (domDoc, domElementType, curr_conjug, curr_conjug.getType(lfn)) )
      return false;

    domElementConjug.appendChild(domElementType);
  }

  domElementParent.appendChild(domElementConjug);
  return true;
}


bool KEduVocKvtmlWriter::saveArticleKvtMl(QDomDocument &domDoc, QDomElement &domElementParent)
/*
 <article>
  <e l="de">    lang determines also lang order in entries !!
   <fi>eine</fi>  which must NOT differ
   <fd>die</fd>
   <mi>ein</mi>
   <md>der</md>
   <ni>ein</ni>
   <nd>das</nd>
  </e>
 </article>
*/
{
  if (m_doc->m_articles.size() == 0)
    return true;

  QDomElement domElementArticle = domDoc.createElement(KV_ARTICLE_GRP);
  QString def, indef, s;

  for (int lfn = 0; lfn < QMIN((int) m_doc->m_articles.size(), m_doc->numLanguages()); lfn++)
  {
    QDomElement domElementEntry = domDoc.createElement(KV_ART_ENTRY);
    if (lfn == 0)
    {
      s = m_doc->originalIdentifier().stripWhiteSpace();
      if (s.isEmpty() )
        s = "original";
    }
    else
    {
      s = m_doc->identifier(lfn).stripWhiteSpace();
      if (s.isEmpty() )
      {
        s.setNum(lfn);
        s.insert(0, "translation ");
      }
    }
    domElementEntry.setAttribute(KV_LANG, s);

    m_doc->m_articles[lfn].female(def, indef);
    if (!def.isEmpty() )
    {
      QDomElement domElementFD = domDoc.createElement(KV_ART_FD);
      QDomText domTextFD = domDoc.createTextNode(def);

      domElementFD.appendChild(domTextFD);
      domElementEntry.appendChild(domElementFD);
    }
    if (!indef.isEmpty() )
    {
      QDomElement domElementFI = domDoc.createElement(KV_ART_FI);
      QDomText domTextFI = domDoc.createTextNode(indef);

      domElementFI.appendChild(domTextFI);
      domElementEntry.appendChild(domElementFI);
    }

    m_doc->m_articles[lfn].male(def, indef);
    if (!def.isEmpty() )
    {
      QDomElement domElementMD = domDoc.createElement(KV_ART_MD);
      QDomText domTextMD = domDoc.createTextNode(def);

      domElementMD.appendChild(domTextMD);
      domElementEntry.appendChild(domElementMD);
    }
    if (!indef.isEmpty() )
    {
      QDomElement domElementMI = domDoc.createElement(KV_ART_MI);
      QDomText domTextMI = domDoc.createTextNode(indef);

      domElementMI.appendChild(domTextMI);
      domElementEntry.appendChild(domElementMI);
    }

    m_doc->m_articles[lfn].natural(def, indef);
    if (!def.isEmpty() )
    {
      QDomElement domElementND = domDoc.createElement(KV_ART_ND);
      QDomText domTextND = domDoc.createTextNode(def);

      domElementND.appendChild(domTextND);
      domElementEntry.appendChild(domElementND);
    }
    if (!indef.isEmpty() )
    {
      QDomElement domElementNI = domDoc.createElement(KV_ART_NI);
      QDomText domTextNI = domDoc.createTextNode(indef);

      domElementNI.appendChild(domTextNI);
      domElementEntry.appendChild(domElementNI);
    }

    domElementArticle.appendChild(domElementEntry);
  }

  domElementParent.appendChild(domElementArticle);
  return true;
}


bool KEduVocKvtmlWriter::saveOptionsKvtMl(QDomDocument &domDoc, QDomElement &domElementParent)
{
  QDomElement domElementOption = domDoc.createElement(KV_OPTION_GRP);
  QDomElement domElementSort = domDoc.createElement(KV_OPT_SORT);

  domElementSort.setAttribute(KV_BOOL_FLAG, (m_doc->m_enableSorting?1:0));
  domElementOption.appendChild(domElementSort);

  domElementParent.appendChild(domElementOption);
  return true;
}


bool KEduVocKvtmlWriter::writeDoc(KEduVocDocument *doc, const QString &generator)
{
  bool first_expr = true;

  m_doc = doc;

  QDomDocument domDoc( "KEduVocDocument" );
  QDomElement domElementKvtml = domDoc.createElement( "kvtml" );

  QString head( "<?xml version='1.0' encoding='UTF-8' ?><!DOCTYPE kvtml SYSTEM \"kvoctrain.dtd\">" );
  domDoc.setContent( head );

  QDomComment domComment = domDoc.createComment(QString(
         "\nThis is a machine generated file.\n"
         "Be careful when editing here.\n"
         "\n"
         "Short definition:\n"
         "\n"
         "lesson     lesson group\n"
         " desc    name\n"
         "   %no     its index\n"
         "   %query  is in query selection\n"
         "   %current  is current lesson\n"
         "type     type group\n"
         " desc    name\n"
         "   %no     its index\n"
         "e      entry of dictionary\n"
         "  %s     is selected\n"
         "  %m     lesson member\n"
         "  %t     common expression type\n"
         " o       original\n"
         "   %q    in query (\"o\" is given, \"t\" is wanted)\n"
         "   %l    language code\n"
         "   %r    remark\n"
         "   %p    pronunciation\n"
         "   %width  column width\n"
         "   %t    expression type (see QueryManager.h)\n"
         "   %tf     false friend from\n"
         "   %ff     false friend to\n"
         "   %a    antonym\n"
         "   %y    synonym\n"
         "   %x    example\n"
         "   %u    usage label\n"
         "   %h    paraphrase\n"
         " t       translation ..\n"
         "   %q    in query (\"t\" is given, \"o\" is wanted)\n"
         "   %l    language code\n"
         "   %r    remark\n"
         "   %p    pronunciation\n"
         "   %width  column width\n"
         "   %t    expression type\n"
         "   %tf     false friend from\n"
         "   %ff     false friend to\n"
         "   %a    antonym\n"
         "   %y    synonym\n"
         "   %x    example\n"
         "   %u    usage label\n"
         "   %h    paraphrase\n"
         "\n"
         "   %d    last query date (from;to)\n"
         "   %w    dito, compressed and deprecated\n"
         "   %g    grade (from;to)\n"
         "   %c    count (from;to)\n"
         "   %b    bad count (from;to)\n"
         "\n"
         "\nValid xml means:\n"
         " - Close all tags\n"
         " - Keep proper hierarchy\n"
         " - All attributes are quoted\n"));

  domDoc.appendChild(domComment);

  domElementKvtml.setAttribute(KV_ENCODING, (QString)"UTF-8");

  domElementKvtml.setAttribute(KV_GENERATOR, generator);
  domElementKvtml.setAttribute(KV_COLS, m_doc->numLanguages() );
  domElementKvtml.setAttribute(KV_LINES, m_doc->numEntries() );

  if (!m_doc->m_title.isEmpty())
    domElementKvtml.setAttribute(KV_TITLE, m_doc->m_title);

  if (!m_doc->m_author.isEmpty())
    domElementKvtml.setAttribute(KV_AUTHOR, m_doc->author() );

  if (!m_doc->m_license.isEmpty())
    domElementKvtml.setAttribute(KV_LICENSE, m_doc->license() );

  if (!m_doc->m_remark.isEmpty())
    domElementKvtml.setAttribute(KV_DOC_REM, m_doc->docRemark() );

  if (!saveLessonKvtMl(domDoc, domElementKvtml))
    return false;

  if (!saveArticleKvtMl(domDoc, domElementKvtml))
    return false;

  if (!saveConjugHeader(domDoc, domElementKvtml, m_doc->m_conjugations))
    return false;

  if (!saveOptionsKvtMl(domDoc, domElementKvtml))
    return false;

  if (!saveTypeNameKvtMl(domDoc, domElementKvtml))
    return false;

  if (!saveTenseNameKvtMl(domDoc, domElementKvtml))
    return false;

  if (!saveUsageNameKvtMl(domDoc, domElementKvtml))
    return false;

  QString q_org, q_trans;
  QValueList<KEduVocExpression>::const_iterator first =  m_doc->m_vocabulary.begin ();
  m_doc->queryLang(q_org, q_trans);

  int ent_no = 0;
  int ent_percent = (int) m_doc->m_vocabulary.size () / 100;
  float f_ent_percent = (int) m_doc->m_vocabulary.size () / 100.0;
//TODO emit progressChanged(this, 0);

  while (first != m_doc->m_vocabulary.end ())
  {
    QDomElement domElementExpression = domDoc.createElement(KV_EXPR);

    ent_no++;
    if (ent_percent != 0 && (ent_no % ent_percent) == 0 )
    {
      //TODO emit progressChanged(this, ent_no / (int) f_ent_percent);
    }
    if ((*first).lesson() != 0)
    {
      // entry belongs to lesson x
      QString ls;
      int lm = (*first).lesson();
      if (lm > (int) m_doc->m_lessonDescriptions.size() )
      {
        // should not be
        kdError() << "index of lesson member too high: " << lm << endl;
        lm = 0;
      }
      ls.setNum (lm);
      domElementExpression.setAttribute (KV_LESS_MEMBER, ls);
    }

    if ((*first).isInQuery())
    {
      // entry was selected for query
      domElementExpression.setAttribute (KV_SELECTED, (QString) "1");
    }

    if (!(*first).isActive())
    {
      // entry was inactive
      domElementExpression.setAttribute (KV_INACTIVE, (QString) "1");
    }

    if ((*first).uniqueType() && !(*first).type(0).isEmpty())
    {
      domElementExpression.setAttribute (KV_EXPRTYPE, (*first).type(0));
    }

    QDomElement domElementOriginal = domDoc.createElement(KV_ORG);
    if (first_expr)
    {
      // save space, only tell language in first entry
      QString s;
      s.setNum (m_doc->sizeHint (0));
      domElementOriginal.setAttribute(KV_SIZEHINT, s);

      s = m_doc->originalIdentifier().stripWhiteSpace();
      if (s.isEmpty() )
        s = "original";
      domElementOriginal.setAttribute (KV_LANG, s);
      if (s == q_org)
        domElementOriginal.setAttribute(KV_QUERY, (QString) KV_O);
      else if (s == q_trans)
        domElementOriginal.setAttribute(KV_QUERY, (QString) KV_T);

    }

    if (!(*first).remark(0).isEmpty() )
      domElementOriginal.setAttribute(KV_REMARK, (*first).remark(0));

    if (!(*first).synonym(0).isEmpty() )
      domElementOriginal.setAttribute(KV_SYNONYM, (*first).synonym(0));

    if (!(*first).example(0).isEmpty() )
      domElementOriginal.setAttribute(KV_EXAMPLE, (*first).example(0));

    if (!(*first).usageLabel(0).isEmpty() )
      domElementOriginal.setAttribute(KV_USAGE, (*first).usageLabel(0));

    if (!(*first).paraphrase(0).isEmpty() )
      domElementOriginal.setAttribute(KV_PARAPHRASE, (*first).paraphrase(0));

    if (!(*first).antonym(0).isEmpty() )
      domElementOriginal.setAttribute(KV_ANTONYM, (*first).antonym(0));

    if (!(*first).pronounciation(0).isEmpty() )
      domElementOriginal.setAttribute(KV_PRONUNCE, (*first).pronounciation(0));

    if (!(*first).uniqueType() && !(*first).type(0).isEmpty())
      domElementOriginal.setAttribute(KV_EXPRTYPE, (*first).type(0));

    if (!saveMultipleChoice(domDoc, domElementOriginal, (*first).multipleChoice(0)))
      return false;

    QString s;
    QString entype = s = (*first).type(0);
    int pos = s.find (QM_TYPE_DIV);
    if (pos >= 0)
      entype = s.left (pos);
    else
      entype = s;

    if (entype == QM_VERB
        && (*first).conjugation(0).numEntries() > 0)
    {
      Conjugation conj = (*first).conjugation(0);
      if (!saveConjugEntry(domDoc, domElementOriginal, conj))
        return false;
    }
    else if (entype == QM_ADJ
             && !(*first).comparison(0).isEmpty())
    {
      Comparison comp = (*first).comparison(0);
      if (!saveComparison(domDoc, domElementOriginal, comp))
        return false;
    }

    QDomText domTextOriginal = domDoc.createTextNode((*first).original());
    domElementOriginal.appendChild(domTextOriginal);
    domElementExpression.appendChild(domElementOriginal);

    int trans = 1;
    while (trans < (int)m_doc->m_languages.size())
    {
      QDomElement domElementTranslation = domDoc.createElement(KV_TRANS);
      if (first_expr)
      {
        // save space, only tell language in first entry
        QString s;
        s.setNum (m_doc->sizeHint (trans));
        domElementTranslation.setAttribute(KV_SIZEHINT, s);

        s = m_doc->identifier(trans).stripWhiteSpace();
        if (s.isEmpty() )
        {
          s.setNum (trans);
          s.insert (0, "translation ");
        }
        domElementTranslation.setAttribute(KV_LANG, s);
        if (s == q_org)
          domElementTranslation.setAttribute(KV_QUERY, (QString) KV_O);
        else if (s == q_trans)
          domElementTranslation.setAttribute(KV_QUERY, (QString) KV_T);
      }

      QString s1, s2;

      if ((*first).grade(trans, false) != 0
        ||(*first).grade(trans, true) != 0)
      {
        domElementTranslation.setAttribute(KV_GRADE, (*first).gradeStr(trans, false)
                  +';'
                  +(*first).gradeStr(trans, true));
      }

      if ((*first).queryCount(trans, false) != 0
        ||(*first).queryCount(trans, true) != 0)
      {
        s1.setNum((*first).queryCount(trans, false));
        s2.setNum((*first).queryCount(trans, true));
        domElementTranslation.setAttribute(KV_COUNT, s1 +';' +s2);
      }

      if ((*first).badCount(trans, false) != 0
        ||(*first).badCount(trans, true) != 0)
      {
        s1.setNum((*first).badCount(trans, false));
        s2.setNum((*first).badCount(trans, true));
        domElementTranslation.setAttribute(KV_BAD, s1 +';' +s2);
      }

      if ((*first).queryDate(trans, false).toTime_t() != 0
        ||(*first).queryDate(trans, true).toTime_t() != 0)
      {
        s1.setNum((*first).queryDate(trans, false).toTime_t());
        s2.setNum((*first).queryDate(trans, true).toTime_t());
        domElementTranslation.setAttribute(KV_DATE, s1 +';' +s2);
      }

      if (!(*first).remark(trans).isEmpty() )
        domElementTranslation.setAttribute(KV_REMARK, (*first).remark(trans));

      if (!(*first).fauxAmi(trans, false).isEmpty() )
        domElementTranslation.setAttribute(KV_FAUX_AMI_F, (*first).fauxAmi(trans, false));

      if (!(*first).fauxAmi(trans, true).isEmpty() )
        domElementTranslation.setAttribute(KV_FAUX_AMI_T, (*first).fauxAmi(trans, true));

      if (!(*first).synonym(trans).isEmpty() )
        domElementTranslation.setAttribute(KV_SYNONYM, (*first).synonym(trans));

      if (!(*first).example(trans).isEmpty() )
        domElementTranslation.setAttribute(KV_EXAMPLE, (*first).example(trans));

      if (!(*first).usageLabel(trans).isEmpty() )
        domElementTranslation.setAttribute(KV_USAGE, (*first).usageLabel(trans));

      if (!(*first).paraphrase(trans).isEmpty() )
        domElementTranslation.setAttribute(KV_PARAPHRASE, (*first).paraphrase(trans));

      if (!(*first).antonym(trans).isEmpty() )
        domElementTranslation.setAttribute(KV_ANTONYM, (*first).antonym(trans));

      if (!(*first).pronounciation(trans).isEmpty() )
        domElementTranslation.setAttribute(KV_PRONUNCE, (*first).pronounciation(trans));

      if (!(*first).uniqueType() && !(*first).type(trans).isEmpty())
        domElementTranslation.setAttribute(KV_EXPRTYPE, (*first).type(trans));

      // only save conjugations when type == verb

      if (!saveMultipleChoice(domDoc, domElementTranslation, (*first).multipleChoice(trans)))
        return false;

      QString s;
      QString entype = s = (*first).type(0);
      int pos = s.find (QM_TYPE_DIV);
      if (pos >= 0)
        entype = s.left (pos);
      else
        entype = s;

      if (entype == QM_VERB
          && (*first).conjugation(trans).numEntries() > 0)
      {
        Conjugation conj = (*first).conjugation(trans);
        if (!saveConjugEntry(domDoc, domElementTranslation, conj))
          return false;
      }

      if (entype == QM_ADJ
          && !(*first).comparison(trans).isEmpty())
      {
        Comparison comp = (*first).comparison(trans);
        if (!saveComparison(domDoc, domElementTranslation, comp))
          return false;
      }

      QDomText domTextTranslation = domDoc.createTextNode((*first).translation(trans));
      domElementTranslation.appendChild(domTextTranslation);
      domElementExpression.appendChild(domElementTranslation);

      trans++;
    }

    domElementKvtml.appendChild(domElementExpression);

    first++;
    first_expr = false;
  }

  domDoc.appendChild(domElementKvtml);

  QTextStream ts( m_outputFile );
  ts << domDoc.toString();

// TODO setModified (false);
}
