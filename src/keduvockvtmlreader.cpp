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

#include <kapplication.h>
#include <kdebug.h>
#include <klocale.h>
#include <kmessagebox.h>

#include <qdom.h>
#include <qtextstream.h>

#include "keduvockvtmlreader.h"
#include "keduvocdocument.h"

KEduVocKvtmlReader::KEduVocKvtmlReader(QFile *file)
{
  // the file must be already open
  m_inputFile = file;
}

KEduVocKvtmlReader::~KEduVocKvtmlReader()
{
}


bool KEduVocKvtmlReader::readLesson(QDomElement &domElementParent)
{
  QString s;
  m_doc->lesson_descr.clear();

  //-------------------------------------------------------------------------
  // Attributes
  //-------------------------------------------------------------------------

  QDomAttr domAttrWidth = domElementParent.attributeNode(KV_SIZEHINT);
  if (!domAttrWidth.isNull())
    m_doc->setSizeHint(-1, domAttrWidth.value().toInt());

  //-------------------------------------------------------------------------
  // Children
  //-------------------------------------------------------------------------

  QDomElement domElementChild = domElementParent.firstChild().toElement();

  while (!domElementChild.isNull())
  {
    if (domElementChild.tagName() == KV_LESS_DESC)
    {
      //-----------
      // Attributes

      int no = 0;
      bool isCurr = false;

      QDomAttr domAttrNo = domElementChild.attributeNode(KV_LESS_NO);
      if (!domAttrNo.isNull())
        no = domAttrNo.value().toInt();

      QDomAttr domAttrCurrent = domElementChild.attributeNode(KV_LESS_CURR);
      if (!domAttrCurrent.isNull())
        isCurr = domAttrCurrent.value().toInt() != 0;

      if (isCurr && no != 0)
        m_doc->setCurrentLesson(no);

      QDomAttr domAttrQuery = domElementChild.attributeNode(KV_LESS_QUERY);
      if (!domAttrQuery.isNull())
        m_doc->lessons_in_query.push_back(domAttrQuery.value().toInt() != 0);
      else
        m_doc->lessons_in_query.push_back(false);

      //-----
      // Text

      s = domElementChild.text();
      if (s.isNull())
        s = "";
      m_doc->lesson_descr.push_back(s);
    }
    else
    {
      domErrorUnknownElement(domElementChild.tagName());
      return false;
    }

    domElementChild = domElementChild.nextSibling().toElement();
  }

  return true;
}


bool KEduVocKvtmlReader::readArticle(QDomElement &domElementParent)
/*
 <article>
  <e l="de">        lang determines also lang order in entries !!
   <fi>eine</fi>    which must NOT differ
   <fd>die</fd>
   <mi>ein</mi>
   <md>der</md>
   <ni>ein</ni>
   <nd>das</nd>
  </e>
 </article>
*/
{
  bool endOfGroup = false;
  bool inEntry = false;
  int count = 0;
  QString s;
  m_doc->articles.clear();

  QDomElement domElementEntry = domElementParent.firstChild().toElement();

  while (!domElementEntry.isNull())
  {
    if (domElementEntry.tagName() != KV_ART_ENTRY)
    {
      domError(i18n("expected tag <%1>").arg(KV_ART_ENTRY));
      return false;
    }

    //----------
    // Attribute

    QString lang;
    QDomAttr domAttrLang = domElementEntry.attributeNode(KV_LANG);

    if ((int)m_doc->langs.size() <= count)
    {
      // first entry
      if (!domAttrLang.isNull())         // no definition in first entry
        lang = domAttrLang.value();
      else
        lang = "original";
      m_doc->langs.push_back(lang);
    }
    else
    {
      if (!domAttrLang.isNull() && domAttrLang.value() != m_doc->langs[count])
      {
        // different originals ?
        domError(i18n("ambiguous definition of language code"));
        return false;
      }
    }

    //---------
    // Children

    QString fem_def = "";
    QString  mal_def = "";
    QString  nat_def = "";
    QString  fem_indef = "";
    QString  mal_indef = "";
    QString  nat_indef = "";

    QDomElement domElementEntryChild = domElementEntry.firstChild().toElement();
    while (!domElementEntryChild.isNull())
    {
      if (domElementEntryChild.tagName() == KV_ART_FD)
      {
        fem_def = domElementEntryChild.text();
        if (fem_def.isNull())
          fem_def = "";
      }
      else if (domElementEntryChild.tagName() == KV_ART_FI)
      {
        fem_indef = domElementEntryChild.text();
        if (fem_indef.isNull())
          fem_indef = "";
      }
      else if (domElementEntryChild.tagName() == KV_ART_MD)
      {
        mal_def = domElementEntryChild.text();
        if (mal_def.isNull())
          mal_def = "";
      }
      else if (domElementEntryChild.tagName() == KV_ART_MI)
      {
        mal_indef = domElementEntryChild.text();
        if (mal_indef.isNull())
          mal_indef = "";
      }
      else if (domElementEntryChild.tagName() == KV_ART_ND)
      {
        nat_def = domElementEntryChild.text();
        if (nat_def.isNull())
          nat_def = "";
      }
      else if (domElementEntryChild.tagName() == KV_ART_NI)
      {
        nat_indef = domElementEntryChild.text();
        if (nat_indef.isNull())
          nat_indef = "";
      }
      else
      {
        domErrorUnknownElement(domElementEntryChild.tagName());
        return false;
      }

      domElementEntryChild = domElementEntryChild.nextSibling().toElement();
    }

    m_doc->articles.push_back(Article(  fem_def, fem_indef,
                                        mal_def, mal_indef,
                                        nat_def, nat_indef));

    domElementEntry = domElementEntry.nextSibling().toElement();
    count++;
  }

  return true;
}


bool KEduVocKvtmlReader::readConjug(QDomElement &domElementParent,
                                    vector<Conjugation> &curr_conjug,
                                    const QString &entry_tag)
/*
 <conjugation>        used in header for definiton of "prefix"
  <e l="de">          lang determines also lang order in entries !!
   <s1>I</s1>         which must NOT differ
   <s2>you<2>
   <s3f>he</s3f>
   <s3m>she</s3m>
   <s3n>it</s3n>
   <p1>we</p1>
   <p2>you</p2>
   <p3f>they</p3f>
   <p3m>they</p3m>
   <p3n>they</p3n>
  </e>
 </conjugation>

 <conjugation>        and in entry for definition of tenses of (irreg.) verbs
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
  QString s;

  curr_conjug.clear();

  bool    p3_common,
      s3_common;
  QString pers1_sing,
      pers2_sing,
      pers3_m_sing,
      pers3_f_sing,
      pers3_n_sing,
      pers1_plur,
      pers2_plur,
      pers3_m_plur,
      pers3_f_plur,
      pers3_n_plur;

  QString lang;
  QString type;
  int count = 0;
  curr_conjug.push_back(Conjugation());

  QDomElement domElementConjugChild = domElementParent.firstChild().toElement();
  while (!domElementConjugChild.isNull())
  {
    if (domElementConjugChild.tagName() == KV_CON_ENTRY)
    {
      type = CONJ_PREFIX;

      //----------
      // Attribute

      QString lang;
      QDomAttr domAttrLang = domElementConjugChild.attributeNode(KV_LANG);

      if ((int)m_doc->langs.size() <= count)
      {
        // first entry
        if (!domAttrLang.isNull())            // no definition in first entry
          lang = domAttrLang.value();
        else
          lang = "original";
        m_doc->langs.push_back(lang);
      }
      else
      {
        if (!domAttrLang.isNull() && domAttrLang.value() != m_doc->langs[count])
        {
          // different originals ?
          domError(i18n("ambiguous definition of language code"));
          return false;
        }
      }
    }
    else if (domElementConjugChild.tagName() == KV_CON_TYPE)
    {
      //----------
      // Attribute

      QDomAttr domAttrLang = domElementConjugChild.attributeNode(KV_CON_NAME);
      type = domAttrLang.value();
      if (type.isNull())
        type = "";

      if (type.length() != 0 && type.left(1) == UL_USER_TENSE)
      {
        int num = QMIN(type.mid (1, 40).toInt(), 1000); // paranoia check
        if( num > (int) m_doc->tense_descr.size() )
        {
          // description missing ?
          QString s;
          for (int i = m_doc->tense_descr.size(); i < num; i++)
          {
            s.setNum (i+1);
            s.insert (0, "#");  // invent descr according to number
            m_doc->tense_descr.push_back(s);
          }
        }
      }
    }

    pers1_sing = "";
    pers2_sing = "";
    pers3_m_sing = "";
    pers3_f_sing = "";
    pers3_n_sing = "";
    pers1_plur = "";
    pers2_plur = "";
    pers3_m_plur = "";
    pers3_f_plur = "";
    pers3_n_plur = "";
    p3_common = false;
    s3_common = false;

    QDomElement domElementConjugGrandChild = domElementConjugChild.firstChild().toElement();
    while (!domElementConjugGrandChild.isNull())
    {
      if (domElementConjugGrandChild.tagName() == KV_CON_P1S)
      {
        pers1_sing = domElementConjugGrandChild.text();
        if (pers1_sing.isNull())
          pers1_sing = "";
      }
      else if (domElementConjugGrandChild.tagName() == KV_CON_P2S)
      {
        pers2_sing = domElementConjugGrandChild.text();
        if (pers2_sing.isNull())
          pers2_sing = "";
      }
      else if (domElementConjugGrandChild.tagName() == KV_CON_P3SF)
      {
        QDomAttr domAttrCommon = domElementConjugGrandChild.attributeNode(KV_CONJ_COMMON);
        if (!domAttrCommon.isNull())
          s3_common = domAttrCommon.value().toInt();  // returns 0 if the conversion fails

        pers3_f_sing = domElementConjugGrandChild.text();
        if (pers3_f_sing.isNull())
          pers3_f_sing = "";
      }
      else if (domElementConjugGrandChild.tagName() == KV_CON_P3SM)
      {
        pers3_m_sing = domElementConjugGrandChild.text();
        if (pers3_m_sing.isNull())
          pers3_m_sing = "";
      }
      else if (domElementConjugGrandChild.tagName() == KV_CON_P3SN)
      {
        pers3_n_sing = domElementConjugGrandChild.text();
        if (pers3_n_sing.isNull())
          pers3_n_sing = "";
      }
      else if (domElementConjugGrandChild.tagName() == KV_CON_P1P)
      {
        pers1_plur = domElementConjugGrandChild.text();
        if (pers1_plur.isNull())
          pers1_plur = "";
      }
      else if (domElementConjugGrandChild.tagName() == KV_CON_P2P)
      {
        pers2_plur = domElementConjugGrandChild.text();
        if (pers2_plur.isNull())
          pers2_plur = "";
      }
      else if (domElementConjugGrandChild.tagName() == KV_CON_P3PF)
      {
        QDomAttr domAttrCommon = domElementConjugGrandChild.attributeNode(KV_CONJ_COMMON);
        if (!domAttrCommon.isNull())
          p3_common = domAttrCommon.value().toInt();  // returns 0 if the conversion fails

        pers3_f_plur = domElementConjugGrandChild.text();
        if (pers3_f_plur.isNull())
          pers3_f_plur = "";
      }
      else if (domElementConjugGrandChild.tagName() == KV_CON_P3PM)
      {
        pers3_m_plur = domElementConjugGrandChild.text();
        if (pers3_m_plur.isNull())
          pers3_m_plur = "";
      }
      else if (domElementConjugGrandChild.tagName() == KV_CON_P3PN)
      {
        pers3_n_plur = domElementConjugGrandChild.text();
        if (pers3_n_plur.isNull())
          pers3_n_plur = "";
      }
      else
      {
        domErrorUnknownElement(domElementConjugGrandChild.tagName());
        return false;
      }

      domElementConjugGrandChild = domElementConjugGrandChild.nextSibling().toElement();
    }

    if (domElementConjugChild.tagName() == KV_CON_ENTRY)
      while (count+1 > (int) curr_conjug.size() )
        curr_conjug.push_back(Conjugation());

    curr_conjug[count].setPers3SingularCommon(type, s3_common);
    curr_conjug[count].setPers3PluralCommon(type, p3_common);
    curr_conjug[count].setPers1Singular(type, pers1_sing);
    curr_conjug[count].setPers2Singular(type, pers2_sing);
    curr_conjug[count].setPers3FemaleSingular(type, pers3_f_sing);
    curr_conjug[count].setPers3MaleSingular(type, pers3_m_sing);
    curr_conjug[count].setPers3NaturalSingular(type, pers3_n_sing);
    curr_conjug[count].setPers1Plural(type, pers1_plur);
    curr_conjug[count].setPers2Plural(type, pers2_plur);
    curr_conjug[count].setPers3FemalePlural(type, pers3_f_plur);
    curr_conjug[count].setPers3MalePlural(type, pers3_m_plur);
    curr_conjug[count].setPers3NaturalPlural(type, pers3_n_plur);

    if (domElementConjugChild.tagName() == KV_CON_ENTRY)
      count++;

    domElementConjugChild = domElementConjugChild.nextSibling().toElement();
  }

  return true;
}


bool KEduVocKvtmlReader::readOptions(QDomElement &domElementParent)
{
  QDomElement domElementSort = domElementParent.firstChild().toElement();
  while (!domElementSort.isNull())
  {
    if (domElementSort.tagName() == KV_OPT_SORT)
    {
      m_doc->sort_allowed = true;
      QDomAttr domAttrOn = domElementSort.attributeNode(KV_BOOL_FLAG);
      if (!domAttrOn.isNull())
            {
                bool ok = true;
        m_doc->sort_allowed = domAttrOn.value().toInt(&ok);  // returns 0 if the conversion fails
          if (!ok)
              m_doc->sort_allowed = true;
            }
    }

    domElementSort = domElementSort.nextSibling().toElement();
  }

  return true;
}


bool KEduVocKvtmlReader::readType(QDomElement &domElementParent)
{
  QString s;
  m_doc->type_descr.clear();

  QDomElement domElementDesc = domElementParent.firstChild().toElement();

  while (!domElementDesc.isNull())
  {
    if (domElementDesc.tagName() == KV_TYPE_DESC)
    {
      //-----------
      // Attributes

      int no = 0;
      bool isCurr = false;

      QDomAttr domAttrNo = domElementDesc.attributeNode(KV_TYPE_NO);
      if (!domAttrNo.isNull())
        no = domAttrNo.value().toInt();

      // TODO use 'no' to sort types
      // but 'no' seems useless, since types are already ordered by their position in the XML doc

      s = domElementDesc.text();
      if (s.isNull())
        s = "";

      m_doc->type_descr.push_back (s);
    }
    else
    {
      domErrorUnknownElement(domElementDesc.tagName());
      return false;
    }

    domElementDesc = domElementDesc.nextSibling().toElement();
  }

  return true;
}


bool KEduVocKvtmlReader::readTense(QDomElement &domElementParent)
{
  QString s;
  m_doc->tense_descr.clear();

  QDomElement domElementDesc = domElementParent.firstChild().toElement();

  while (!domElementDesc.isNull())
  {
    if (domElementDesc.tagName() == KV_TENSE_DESC)
    {
      //-----------
      // Attributes

      int no = 0;
      bool isCurr = false;

      QDomAttr domAttrNo = domElementDesc.attributeNode(KV_TENSE_NO);
      if (!domAttrNo.isNull())
        no = domAttrNo.value().toInt();

      // TODO use 'no' to sort tenses
      // but 'no' seems useless, since tenses are already ordered by their position in the XML doc

      s = domElementDesc.text();
      if (s.isNull())
        s = "";

      m_doc->tense_descr.push_back (s);
    }
    else
    {
      domErrorUnknownElement(domElementDesc.tagName());
      return false;
    }

    domElementDesc = domElementDesc.nextSibling().toElement();
  }

  return true;
}


bool KEduVocKvtmlReader::readUsage(QDomElement &domElementParent)
{
  QString s;
  m_doc->usage_descr.clear();

  QDomElement domElementDesc = domElementParent.firstChild().toElement();

  while (!domElementDesc.isNull())
  {
    if (domElementDesc.tagName() == KV_USAGE_DESC)
    {
      //-----------
      // Attributes

      int no = 0;
      bool isCurr = false;

      QDomAttr domAttrNo = domElementDesc.attributeNode(KV_USAGE_NO);
      if (!domAttrNo.isNull())
        no = domAttrNo.value().toInt();

      // TODO use 'no' to sort usages
      // but 'no' seems useless, since usages are already ordered by their position in the XML doc

      s = domElementDesc.text();
      if (s.isNull())
        s = "";

      m_doc->usage_descr.push_back (s);
    }
    else
    {
      domErrorUnknownElement(domElementDesc.tagName());
      return false;
    }

    domElementDesc = domElementDesc.nextSibling().toElement();
  }

  return true;
}


bool KEduVocKvtmlReader::readComparison(QDomElement &domElementParent,
                                        Comparison &comp)
/*
 <comparison>
   <l1>good</l1>
   <l2>better</l2>
   <l3>best</l3>
 </comparison>
*/
{
  QString s;
  comp.clear();

  QDomElement domElementComparisonChild = domElementParent.firstChild().toElement();
  while (!domElementComparisonChild.isNull())
  {
    if (domElementComparisonChild.tagName() == KV_COMP_L1)
    {
      s = domElementComparisonChild.text();
      if (s.isNull())
        s = "";
      comp.setL1(s);
    }

    else if (domElementComparisonChild.tagName() == KV_COMP_L2)
    {
      s = domElementComparisonChild.text();
      if (s.isNull())
        s = "";
      comp.setL2(s);
    }

    else if (domElementComparisonChild.tagName() == KV_COMP_L3)
    {
      s = domElementComparisonChild.text();
      if (s.isNull())
        s = "";
      comp.setL3(s);
    }

    else
    {
      domErrorUnknownElement(domElementComparisonChild.tagName());
      return false;
    }

    domElementComparisonChild = domElementComparisonChild.nextSibling().toElement();
  }

  return true;
}


bool KEduVocKvtmlReader::readMultipleChoice(QDomElement &domElementParent,
                                            MultipleChoice &mc)
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
  QString s;
  mc.clear();

  QDomElement domElementChild = domElementParent.firstChild().toElement();
  while (!domElementChild.isNull())
  {
    if (domElementChild.tagName() == KV_MC_1)
    {
      s = domElementChild.text();
      if (s.isNull())
        s = "";
      mc.setMC1(s);
    }

    else if (domElementChild.tagName() == KV_MC_2)
    {
      s = domElementChild.text();
      if (s.isNull())
        s = "";
      mc.setMC2(s);
    }

    else if (domElementChild.tagName() == KV_MC_3)
    {
      s = domElementChild.text();
      if (s.isNull())
        s = "";
      mc.setMC3(s);
    }

    else if (domElementChild.tagName() == KV_MC_4)
    {
      s = domElementChild.text();
      if (s.isNull())
        s = "";
      mc.setMC4(s);
    }

    else if (domElementChild.tagName() == KV_MC_5)
    {
      s = domElementChild.text();
      if (s.isNull())
        s = "";
      mc.setMC5(s);
    }

    else
    {
      domErrorUnknownElement(domElementChild.tagName());
      return false;
    }

    domElementChild = domElementChild.nextSibling().toElement();
  }

  mc.normalize();
  return true;
}


bool KEduVocKvtmlReader::readExpressionChildAttributes( QDomElement &domElementExpressionChild,
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
                                                        QString &paraphrase)
{
  int pos;

  lang = "";
  QDomAttr domAttrLang = domElementExpressionChild.attributeNode(KV_LANG);
  if (!domAttrLang.isNull())
    lang = domAttrLang.value();

  width = -1;
  QDomAttr domAttrWidth = domElementExpressionChild.attributeNode(KV_SIZEHINT);
  if (!domAttrWidth.isNull())
    width = domAttrWidth.value().toInt();

  grade = KV_NORM_GRADE;
  rev_grade = KV_NORM_GRADE;
  QDomAttr domAttrGrade = domElementExpressionChild.attributeNode(KV_LANG);
  if (!domAttrGrade.isNull())
  {
    QString s = domAttrGrade.value();
    if ((pos = s.find(';')) >= 1)
    {
      grade = s.left(pos).toInt();
      rev_grade = s.mid(pos+1, s.length()).toInt();
    }
    else
      grade = s.toInt();
  }

  count = 0;
  rev_count = 0;
  QDomAttr domAttrCount = domElementExpressionChild.attributeNode(KV_COUNT);
  if (!domAttrCount.isNull())
  {
    QString s = domAttrCount.value();
    if ((pos = s.find(';')) >= 1)
    {
      count = s.left(pos).toInt();
      rev_count = s.mid(pos+1, s.length()).toInt();
    }
    else
      count = s.toInt();
  }

  bcount = 0;
  rev_bcount = 0;
  QDomAttr domAttrBad = domElementExpressionChild.attributeNode(KV_BAD);
  if (!domAttrBad.isNull())
  {
    QString s = domAttrBad.value();
    if ((pos = s.find(';')) >= 1)
    {
      bcount = s.left(pos).toInt();
      rev_bcount = s.mid(pos+1, s.length()).toInt();
    }
    else
      bcount = s.toInt();
  }

  date = 0;
  rev_date = 0;
  QDomAttr domAttrDate = domElementExpressionChild.attributeNode(KV_DATE);
  if (!domAttrDate.isNull())
  {
    QString s = domAttrDate.value();
    if ((pos = s.find(';')) >= 1)
    {
      date = s.left(pos).toInt();
      rev_date = s.mid(pos+1, s.length()).toInt();
    }
    else
      date = s.toInt();
  }

  QDomAttr domAttrDate2 = domElementExpressionChild.attributeNode(KV_DATE2);
  if (!domAttrDate2.isNull())
  {
    QString s = domAttrDate2.value();
    if ((pos = s.find(';')) >= 1)
    {
      date = m_doc->decompressDate(s.left(pos));
      rev_date = m_doc->decompressDate(s.mid(pos+1, s.length()));
    }
    else
      date = m_doc->decompressDate(s);
  }

  remark = "";
  QDomAttr domAttrRemark = domElementExpressionChild.attributeNode(KV_REMARK);
  if (!domAttrRemark.isNull())
    remark = domAttrRemark.value();

  faux_ami_f = "";
  QDomAttr domAttrFauxAmiF = domElementExpressionChild.attributeNode(KV_FAUX_AMI_F);
  if (!domAttrFauxAmiF.isNull())
    faux_ami_f = domAttrFauxAmiF.value();

  faux_ami_t = "";
  QDomAttr domAttrFauxAmiT = domElementExpressionChild.attributeNode(KV_FAUX_AMI_T);
  if (!domAttrFauxAmiT.isNull())
    faux_ami_t = domAttrFauxAmiT.value();

  synonym = "";
  QDomAttr domAttrSynonym = domElementExpressionChild.attributeNode(KV_SYNONYM);
  if (!domAttrSynonym.isNull())
    synonym = domAttrSynonym.value();

  example = "";
  QDomAttr domAttrExample = domElementExpressionChild.attributeNode(KV_EXAMPLE);
  if (!domAttrExample.isNull())
    example = domAttrExample.value();

  usage = "";
  QDomAttr domAttrUsage = domElementExpressionChild.attributeNode(KV_USAGE);
  if (!domAttrUsage.isNull())
  {
    usage = domAttrUsage.value();
    if (usage.length() != 0 && usage.left(1) == UL_USER_USAGE)
    {
      int num = QMIN(usage.mid (1, 40).toInt(), 1000); // paranioa check
      if( num > (int) m_doc->usage_descr.size() )
      {
        // description missing ?
        QString s;
        for (int i = m_doc->usage_descr.size(); i < num; i++)
        {
          s.setNum (i+1);
          s.insert (0, "#");  // invent descr according to number
          m_doc->usage_descr.push_back (s);
        }
      }
    }
  }

  paraphrase = "";
  QDomAttr domAttrParaphrase = domElementExpressionChild.attributeNode(KV_PARAPHRASE);
  if (!domAttrParaphrase.isNull())
    paraphrase = domAttrParaphrase.value();

  antonym = "";
  QDomAttr domAttrAntonym = domElementExpressionChild.attributeNode(KV_ANTONYM);
  if (!domAttrAntonym.isNull())
    antonym = domAttrAntonym.value();

  QDomAttr domAttrExprType = domElementExpressionChild.attributeNode(KV_EXPRTYPE);
  if (!domAttrExprType.isNull())
  {
    type = domAttrExprType.value();
    if (type == "1")
      type = QM_VERB;
    else if (type == "2")  // convert from pre-0.5 versions
      type = QM_NOUN;
    else if (type == "3")
      type = QM_NAME;

    if (type.length() != 0 && type.left(1) == QM_USER_TYPE)
    {
      int num = QMIN(type.mid (1, 40).toInt(), 1000); // paranoia check
      if( num > (int) m_doc->type_descr.size() )
      {
        // description missing ?
        QString s;
        for (int i = m_doc->type_descr.size(); i < num; i++)
        {
          s.setNum (i+1);
          s.insert (0, "#");  // invent descr according to number
          m_doc->type_descr.push_back (s);
        }
      }
    }
  }

  pronunce = "";
  QDomAttr domAttrPronunce = domElementExpressionChild.attributeNode(KV_PRONUNCE);
  if (!domAttrPronunce.isNull())
    pronunce = domAttrPronunce.value();

  query_id = "";
  QDomAttr domAttrQuery = domElementExpressionChild.attributeNode(KV_QUERY);
  if (!domAttrQuery.isNull())
    query_id = domAttrQuery.value();

  return true;
}


bool KEduVocKvtmlReader::readExpression(QDomElement &domElementParent)
{
  grade_t       grade,
                r_grade;
  int           qcount,
                r_qcount;
  int           bcount,
                 r_bcount;
  QString       remark;
  QString       pronunce;
  time_t        qdate,
                r_qdate;
  bool          inquery;
  bool          active;
  QString       lang;
  QString       textstr;
  QString       exprtype;
  bool          org_found = false;
  QString       q_org,
                q_trans;
  QString       query_id;
  KEduVocExpression expr;
  int           lesson = 0;
  int           width;
  QString       type;
  QString       faux_ami_f;
  QString       faux_ami_t;
  QString       synonym;
  QString       example;
  QString       antonym;
  QString       usage;
  QString       paraphrase;
  vector<Conjugation> conjug;
  Comparison     comparison;
  MultipleChoice mc;

  //-------------------------------------------------------------------------
  // Attributes
  //-------------------------------------------------------------------------

  QDomAttr domAttrMember = domElementParent.attributeNode(KV_LESS_MEMBER);
  if (!domAttrMember.isNull())
    lesson = domAttrMember.value().toInt();

  QDomAttr domAttrSelected = domElementParent.attributeNode(KV_SELECTED);
  if (!domAttrSelected.isNull())
    inquery = domAttrSelected.value().toInt();

  QDomAttr domAttrInactive = domElementParent.attributeNode(KV_INACTIVE);
  if (!domAttrInactive.isNull())
    active = !domAttrInactive.value().toInt();

  QDomAttr domAttrType = domElementParent.attributeNode(KV_EXPRTYPE);
  if (!domAttrType.isNull())
  {
    exprtype = !domAttrType.value().toInt();
    if (exprtype == "1")
      exprtype = QM_VERB;
    else if (exprtype == "2")  // convert from pre-0.5 versions
      exprtype = QM_NOUN;
    else if (exprtype == "3")
      exprtype = QM_NAME;

    if (exprtype.length() != 0 && exprtype.left(1) == QM_USER_TYPE)
    {
      int num = QMIN(exprtype.mid (1, 40).toInt(), 1000); // paranoia check
      if( num > (int) m_doc->type_descr.size() )
      {
        // description missing ?
        QString s;
        for (int i = m_doc->type_descr.size(); i < num; i++)
        {
          s.setNum (i+1);
          s.insert (0, "#");  // invent descr according to number
          m_doc->type_descr.push_back (s);
        }
      }
    }
  }

  if (lesson && lesson > (int) m_doc->lesson_descr.size() )
  {
    // description missing ?
    QString s;
    for (int i = m_doc->lesson_descr.size(); i < lesson; i++)
    {
      s.setNum (i+1);
      s.insert (0, "#");  // invent descr according to number
      m_doc->lesson_descr.push_back (s);
    }
  }

  //-------------------------------------------------------------------------
  // Child 'Original'
  //-------------------------------------------------------------------------

  // now want "original" and one or more "translations"

  QDomElement domElementExpressionChild = domElementParent.firstChild().toElement();

  unsigned int count = 0;
  org_found = false;

  if (domElementExpressionChild.tagName() != KV_ORG)
  {
    // must be preceded by "original"
    domError(i18n("starting tag <%1> is missing").arg(KV_ORG));
    return false;
  }

  // found original <o>

  org_found = true;

  type = exprtype;

  //-----------
  // Attributes

    if (!readExpressionChildAttributes( domElementExpressionChild,
                                        lang,
                                        grade,  r_grade,
                                        qcount, r_qcount,
                                        qdate,  r_qdate,
                                        remark,
                                        bcount, r_bcount,
                                        query_id,
                                        pronunce,
                                        width,
                                        type,
                                        faux_ami_t,
                                        faux_ami_f,
                                        synonym,
                                        example,
                                        antonym,
                                        usage,
                                        paraphrase))
    return false;


  if (m_doc->vocabulary.size() == 0)
  {
    // only accept in first entry
    if (width >= 0)
      m_doc->setSizeHint (count, width);

    if (query_id == KV_O)
      q_org = lang;

    if (query_id == KV_T)
      q_trans = lang;
  }

  if (m_doc->langs.size() == 0)
  {
    // first entry
    if (lang.isEmpty())                 // no definition in first entry
      lang = "original";
    m_doc->langs.push_back(lang);

  }
  else
  {
    if (lang != m_doc->langs[0] && !lang.isEmpty())
    {
      // different originals ?
      domError(i18n("ambiguous definition of language code"));
      return false;
    }
  }
  count = 0;

  //---------
  // Children

  bool bConjug = false;
  bool bComparison = false;
  bool bMultipleChoice = false;

  QDomElement domElementOriginalChild = domElementExpressionChild.firstChild().toElement();
  while (!domElementOriginalChild.isNull())
  {
    if (domElementOriginalChild.tagName() == KV_CONJUG_GRP)
    {
      if (bConjug)
      {
        domError(i18n("repeated occurrence of tag <%1>").arg(domElementOriginalChild.tagName()));
        return false;
      }
      bConjug = true;
      conjug.clear();
      if (!readConjug(domElementOriginalChild, conjug, (QString) KV_CON_TYPE))
        return false;
    }

    if (domElementOriginalChild.tagName() == KV_COMPARISON_GRP)
    {
      if (bComparison)
      {
        domError(i18n("repeated occurrence of tag <%1>").arg(domElementOriginalChild.tagName()));
        return false;
      }
      bComparison = true;
      comparison.clear();
      if (!readComparison(domElementOriginalChild, comparison))
        return false;
    }

    if (domElementOriginalChild.tagName() == KV_MULTIPLECHOICE_GRP)
    {
      if (bMultipleChoice)
      {
        domError(i18n("repeated occurrence of tag <%1>").arg(domElementOriginalChild.tagName()));
        return false;
      }
      bMultipleChoice = true;
      mc.clear();
      if (!readMultipleChoice(domElementOriginalChild, mc))
        return false;
    }

    else
    {
      domErrorUnknownElement(domElementOriginalChild.tagName());
      return false;
    }

    domElementOriginalChild = domElementOriginalChild.nextSibling().toElement();
  }

  textstr = domElementExpressionChild.text();
  if (textstr.isNull())
    textstr = "";

  expr = KEduVocExpression(textstr);
  expr.setLesson (lesson);
  expr.setInQuery(inquery);
  expr.setActive(active);

  if (conjug.size() > 0)
  {
    expr.setConjugation(0, conjug[0]);
    conjug.clear();
  }
  if (!comparison.isEmpty())
  {
    expr.setComparison(0, comparison);
    comparison.clear();
  }
  if (!mc.isEmpty())
  {
    expr.setMultipleChoice(0, mc);
    mc.clear();
  }
  if (!remark.isEmpty() )
    expr.setRemark (0, remark);
  if (!pronunce.isEmpty() )
    expr.setPronunce (0, pronunce);
  if (!type.isEmpty() )
    expr.setType(0, type);
  if (!synonym.isEmpty() )
    expr.setSynonym(0, synonym);
  if (!example.isEmpty() )
    expr.setExample(0, example);
  if (!usage.isEmpty() )
    expr.setUsageLabel(0, usage);
  if (!paraphrase.isEmpty() )
    expr.setParaphrase(0, paraphrase);
  if (!antonym.isEmpty() )
    expr.setAntonym(0, antonym);


  //-------------------------------------------------------------------------
  // Children 'Translation'
  //-------------------------------------------------------------------------

  domElementExpressionChild = domElementExpressionChild.nextSibling().toElement();

  while (!domElementExpressionChild.isNull())
  {
    if (domElementExpressionChild.tagName() != KV_TRANS)
    {
      // "original" must be followed by "translations"
      domError(i18n("starting tag <%1> is missing").arg(KV_TRANS));
      return false;
    }

    // found translation <t>

    count++;
    type = exprtype;

    //-----------
    // Attributes

    if (!readExpressionChildAttributes( domElementExpressionChild,
                                        lang,
                                        grade,  r_grade,
                                        qcount, r_qcount,
                                        qdate,  r_qdate,
                                        remark,
                                        bcount, r_bcount,
                                        query_id,
                                        pronunce,
                                        width,
                                        type,
                                        faux_ami_f,
                                        faux_ami_t,
                                        synonym,
                                        example,
                                        antonym,
                                        usage,
                                        paraphrase))
      return false;

    if (m_doc->vocabulary.size() == 0)
    {
      // only accept in first entry
      if (width >= 0)
        m_doc->setSizeHint (count, width);

      if (query_id == KV_O)
        q_org = lang;

      if (query_id == KV_T)
        q_trans = lang;

    }

    if (m_doc->langs.size() <= count)
    {
      // new translation
      if (lang.isEmpty())
      {
        // no definition in first entry ?
        lang.setNum (m_doc->langs.size() );
        lang.insert (0, "translation ");
      }
      m_doc->langs.push_back(lang);

    }
    else
    {
      if (lang != m_doc->langs[count] && !lang.isEmpty())
      { // different language ?
        domError(i18n("ambiguous definition of language code"));
        return false;
      }
    }

    //---------
    // Children

    bool bConjug = false;
    bool bComparison = false;
    bool bMultipleChoice = false;

    QDomElement domElementOriginalChild = domElementExpressionChild.firstChild().toElement();
    while (!domElementOriginalChild.isNull())
    {
      if (domElementOriginalChild.tagName() == KV_CONJUG_GRP)
      {
        if (bConjug)
        {
          domError(i18n("repeated occurrence of tag <%1>").arg(domElementOriginalChild.tagName()));
          return false;
        }
        bConjug = true;
        conjug.clear();
        if (!readConjug(domElementOriginalChild, conjug, (QString) KV_CON_TYPE))
          return false;
      }

      if (domElementOriginalChild.tagName() == KV_COMPARISON_GRP)
      {
        if (bComparison)
        {
          domError(i18n("repeated occurrence of tag <%1>").arg(domElementOriginalChild.tagName()));
          return false;
        }
        bComparison = true;
        comparison.clear();
        if (!readComparison(domElementOriginalChild, comparison))
          return false;
      }

      if (domElementOriginalChild.tagName() == KV_MULTIPLECHOICE_GRP)
      {
        if (bMultipleChoice)
        {
          domError(i18n("repeated occurrence of tag <%1>").arg(domElementOriginalChild.tagName()));
          return false;
        }
        bMultipleChoice = true;
        mc.clear();
        if (!readMultipleChoice(domElementOriginalChild, mc))
          return false;
      }

      else
      {
        domErrorUnknownElement(domElementOriginalChild.tagName());
        return false;
      }

      domElementOriginalChild = domElementOriginalChild.nextSibling().toElement();
    }

    textstr = domElementExpressionChild.text();
    if (textstr.isNull())
      textstr = "";

    /*
      if (qcount == 0)
      {
      grade = KV_NORM_GRADE;
      }

      if (r_qcount == 0)
      {
      r_grade = KV_NORM_GRADE;
      }
    */
    expr.addTranslation (textstr, grade, r_grade);
    expr.setQueryCount  (count, qcount, false);
    expr.setQueryCount  (count, r_qcount, true);
    expr.setBadCount    (count, bcount, false);
    expr.setBadCount    (count, r_bcount, true);
    expr.setQueryDate   (count, qdate, false);
    expr.setQueryDate   (count, r_qdate, true);

    if (conjug.size() > 0)
    {
      expr.setConjugation(count, conjug[0]);
      conjug.clear();
    }
    if (!comparison.isEmpty())
    {
      expr.setComparison(count, comparison);
      comparison.clear();
    }
    if (!mc.isEmpty())
    {
      expr.setMultipleChoice(count, mc);
      mc.clear();
    }
    if (!type.isEmpty() )
      expr.setType (count, type);
    if (!remark.isEmpty() )
      expr.setRemark (count, remark);
    if (!pronunce.isEmpty() )
      expr.setPronunce (count, pronunce);
    if (!faux_ami_f.isEmpty() )
      expr.setFauxAmi (count, faux_ami_f, false);
    if (!faux_ami_t.isEmpty() )
      expr.setFauxAmi (count, faux_ami_t, true);
    if (!synonym.isEmpty() )
      expr.setSynonym (count, synonym);
    if (!example.isEmpty() )
      expr.setExample (count, example);
    if (!usage.isEmpty() )
      expr.setUsageLabel (count, usage);
    if (!paraphrase.isEmpty() )
      expr.setParaphrase (count, paraphrase);
    if (!antonym.isEmpty() )
      expr.setAntonym (count, antonym);

    domElementExpressionChild = domElementExpressionChild.nextSibling().toElement();
  }
  if (m_doc->numEntries() == 0)
    m_doc->setQueryLang(q_org, q_trans);
  m_doc->vocabulary.push_back(expr);

  return true;
}


bool KEduVocKvtmlReader::readBody(QDomElement &domElementParent)
{
  bool lessgroup = false;
  bool optgroup = false;
  bool attrgroup = false;
  bool tensegroup = false;
  bool usagegroup = false;
  bool articlegroup = false;
  bool conjuggroup = false;

  int ent_no = 0;
  int ent_percent = (int) m_doc->lines / 100;
  float f_ent_percent = (int) m_doc->lines / 100.0;
/* TODO EPT
if (lines != 0)
    emit progressChanged(this, 0);
*/

  QDomElement domElementChild = domElementParent.firstChild().toElement();

  while (!domElementChild.isNull())
  {
    if (domElementChild.tagName() == KV_LESS_GRP)
    {
      if (lessgroup)
      {
        domError(i18n("repeated occurrence of tag <%1>").arg(domElementChild.tagName()));
        return false;
      }
      lessgroup = true;
      if (!readLesson(domElementChild))
        return false;
    }

    else if (domElementChild.tagName() == KV_ARTICLE_GRP)
    {
      if (articlegroup)
      {
        domError(i18n("repeated occurrence of tag <%1>").arg(domElementChild.tagName()));
        return false;
      }
      articlegroup = true;
      if (!readArticle(domElementChild))
        return false;
    }

    else if (domElementChild.tagName() == KV_CONJUG_GRP)
    {
      if (conjuggroup)
      {
        domError(i18n("repeated occurrence of tag <%1>").arg(domElementChild.tagName()));
        return false;
      }
      conjuggroup = true;
      if (!readConjug(domElementChild, m_doc->conjugations, KV_CON_ENTRY))
        return false;
    }

    else if (domElementChild.tagName() == KV_OPTION_GRP)
    {
      if (optgroup)
      {
        domError(i18n("repeated occurrence of tag <%1>").arg(domElementChild.tagName()));
        return false;
      }
      optgroup = true;
      if (!readOptions(domElementChild))
        return false;
    }

    else if (domElementChild.tagName() == KV_TYPE_GRP)
    {
      if (attrgroup)
      {
        domError(i18n("repeated occurrence of tag <%1>").arg(domElementChild.tagName()));
        return false;
      }
      attrgroup = true;
      if (!readType(domElementChild))
        return false;
    }

    else if (domElementChild.tagName() == KV_TENSE_GRP)
    {
      if (tensegroup)
      {
        domError(i18n("repeated occurrence of tag <%1>").arg(domElementChild.tagName()));
        return false;
      }
      tensegroup = true;
      if (!readTense(domElementChild))
        return false;
    }

    else if (domElementChild.tagName() == KV_USAGE_GRP)
    {
      if (usagegroup)
      {
        domError(i18n("repeated occurrence of tag <%1>").arg(domElementChild.tagName()));
        return false;
      }
      usagegroup = true;
      if (!readUsage(domElementChild))
        return false;
    }

    else if (domElementChild.tagName() == KV_EXPR)
    {
      /* TODO EPT
      if (lines != 0)
      {
        ent_no++;
        if (ent_percent != 0 && (ent_no % ent_percent) == 0 )
          emit progressChanged(this, int(ent_no / f_ent_percent));
      }*/
      if (!readExpression(domElementChild))
        return false;
    }

    else
    {
      domErrorUnknownElement(domElementChild.tagName());
      return false;
    }

    domElementChild = domElementChild.nextSibling().toElement();
  }

  return true;
}


bool KEduVocKvtmlReader::readDoc(KEduVocDocument *doc)
{
  m_doc = doc;

  QDomDocument domDoc("Kvtml" );
  QString errorMsg;
  if( !domDoc.setContent( m_inputFile, &errorMsg ) )
  {
    domError(errorMsg);
    return false;
  }

  m_doc->langs.clear();
  m_doc->vocabulary.clear();

  m_doc->generator = "";
  m_doc->cols = 0;
  m_doc->lines = 0;
  m_doc->doctitle = "";
  m_doc->author = "";
  m_doc->license = "";
  m_doc->doc_remark = "";


  QDomElement domElementKvtml = domDoc.documentElement();
  if( domElementKvtml.tagName() != KV_DOCTYPE )
  {
    domError(i18n("Tag <%1> was expected "
            "but tag <%2> was read." ).arg(KV_DOCTYPE).arg(domElementKvtml.tagName()));
    return false;
  }

  //-------------------------------------------------------------------------
  // Attributes
  //-------------------------------------------------------------------------

  QDomAttr domAttrEncoding = domElementKvtml.attributeNode(KV_ENCODING);
  if (!domAttrEncoding.isNull())
  {
    // TODO handle old encodings
    // Qt DOM API autodetects encoding, so is there anything to do ?
  }

  QDomAttr domAttrTitle = domElementKvtml.attributeNode(KV_TITLE);
  if (!domAttrTitle.isNull())
  {
    m_doc->doctitle = domAttrTitle.value();
  }

  QDomAttr domAttrAuthor = domElementKvtml.attributeNode(KV_AUTHOR);
  if (!domAttrAuthor.isNull())
  {
    m_doc->author = domAttrAuthor.value();
  }

  QDomAttr domAttrLicence = domElementKvtml.attributeNode(KV_LICENSE);
  if (!domAttrLicence.isNull())
  {
    m_doc->license = domAttrLicence.value();
  }

  QDomAttr domAttrRemark = domElementKvtml.attributeNode(KV_DOC_REM);
  if (!domAttrRemark.isNull())
  {
    m_doc->doc_remark = domAttrRemark.value();
  }

  QDomAttr domAttrGenerator = domElementKvtml.attributeNode(KV_GENERATOR);
  if (!domAttrGenerator.isNull())
  {
    m_doc->generator = domAttrGenerator.value();
    int pos = m_doc->generator.findRev (KVD_VERS_PREFIX);
    if (pos >= 0)
    {
      m_doc->doc_version = m_doc->generator;
      m_doc->doc_version.remove (0, pos+2);
    }
  }

  QDomAttr domAttrCols = domElementKvtml.attributeNode(KV_COLS);
  if (!domAttrCols.isNull())
  {
    m_doc->cols = domAttrCols.value().toInt();
  }

  QDomAttr domAttrLines = domElementKvtml.attributeNode(KV_LINES);
  if (!domAttrLines.isNull())
  {
    m_doc->lines = domAttrLines.value().toInt();
  }


  //-------------------------------------------------------------------------
  // Children
  //-------------------------------------------------------------------------

  bool result = readBody(domElementKvtml);  // read vocabulary

  // TODO EPT setModified (false);
  return result;
}


void KEduVocKvtmlReader::domErrorUnknownElement(const QString &elem)
{
  QString ln = i18n("File:\t%1\n").arg(m_doc->URL().path());

  QString format = i18n(
      "Your document contains an unknown tag <%1>.  "  // keep trailing space
      "Maybe your version of KVocTrain is too old, "
      "or the document is damaged.\n"
      "Loading is aborted because KVocTrain cannot "
      "read documents with unknown elements.\n"
     );
  QString msg = format.arg(elem);
  QApplication::setOverrideCursor( arrowCursor, true );
  QString s = kapp->makeStdCaption(i18n("Unknown element"));
  KMessageBox::sorry(0, ln+msg, s);
  QApplication::restoreOverrideCursor();
}

void KEduVocKvtmlReader::domError(const QString &text )
{
  QApplication::setOverrideCursor( arrowCursor, true );
  QString s = kapp->makeStdCaption(i18n("Error"));
  QString ln = i18n("File:\t%1\n").arg(m_doc->URL().path());
  QString msg = text;
  KMessageBox::error(0, ln+msg, s);
  QApplication::restoreOverrideCursor();
}

//#include "KEduVocKvtmlReader.moc"
