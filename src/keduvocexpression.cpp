/***************************************************************************
                        Vocabulary Expression for KDE Edu
    -----------------------------------------------------------------------
    copyright            : (C) 1999-2001 Ewald Arnold
                           (C) 2001 The KDE-EDU team
                           (C) 2005 Peter Hedlung
    email                : peter@peterandlinda.com
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "keduvocexpression.h"

void KEduVocExpression::Init()
{
  grades.push_back(KV_NORM_GRADE);
  rev_grades.push_back(KV_NORM_GRADE);
  inquery = false;
  active = true;
  qcounts.push_back(0);
  rev_qcounts.push_back(0);
  bcounts.push_back(0);
  rev_bcounts.push_back(0);
  qdates.push_back(0);
  rev_qdates.push_back(0);
  m_lesson = 0;
}


KEduVocExpression::KEduVocExpression (QString &expr, int _lesson)
{
  Init();
  setOriginal(expr.stripWhiteSpace() );
  m_lesson = _lesson;
}


KEduVocExpression::KEduVocExpression ()
{
  Init();
}


KEduVocExpression::KEduVocExpression (QString &s, QString separator, int _lesson)
{
  Init();
  QString se;
  m_lesson = _lesson;

  if (separator.length() ) {
    int pos = s.find(separator);

    if (pos == -1) {
      setOriginal(s.stripWhiteSpace());
    }
    else {
      se = s.left(pos).stripWhiteSpace();
      setOriginal(se);
      s.remove (0, pos+separator.length() );
//      s.stripWhiteSpace();

      // gather all translations
      while ((pos = s.find(separator)) != -1) {
        se = s.left(pos).stripWhiteSpace();
        addTranslation(se, KV_NORM_GRADE, KV_NORM_GRADE);
        s.remove (0, pos+separator.length() );
//        s.stripWhiteSpace();
      }
      addTranslation(s.stripWhiteSpace(), KV_NORM_GRADE, KV_NORM_GRADE);
    }
  }
}


KEduVocExpression::~KEduVocExpression()
{
}


int KEduVocExpression::numTranslations() const
{
  return translations.size();
}


QString KEduVocExpression::remark (int idx) const
{
  if (idx >= (int)remarks.size() || idx < 0) {
    return "";
  }
  else {
    return remarks[idx];
  }
}


void KEduVocExpression::setRemark (int idx, const QString & expr)
{
  if ( idx < 0) return;

  // extend remarks with empty strings if necessary
  if ((int)remarks.size() <= idx )
    for (int i = remarks.size(); i < idx+1; i++)
      remarks.push_back ("");

  remarks[idx] = expr.stripWhiteSpace();
}


void KEduVocExpression::setFauxAmi (int idx, const QString & expr, bool rev_ami)
{
  if (idx < 1) return;

  if (rev_ami) {
    // extend friend with empty strings if necessary
    if ((int)rev_fauxAmi.size() <= idx )
      for (int i = rev_fauxAmi.size(); i < idx+1; i++)
        rev_fauxAmi.push_back ("");

    rev_fauxAmi[idx] = expr.stripWhiteSpace();

  }
  else {
    // extend friend with empty strings if necessary
    if ((int)m_fauxAmi.size() <= idx )
      for (int i = m_fauxAmi.size(); i < idx+1; i++)
        m_fauxAmi.push_back ("");

    m_fauxAmi[idx] = expr.stripWhiteSpace();
  }
}


QString KEduVocExpression::fauxAmi (int idx, bool rev_ami) const
{
  if (rev_ami) {
    if (idx >= (int)rev_fauxAmi.size() || idx < 1 ) {
      return "";
    }

    return rev_fauxAmi[idx];
  }

  if (idx >= (int)m_fauxAmi.size() || idx < 1 ) {
    return "";
  }

  return m_fauxAmi[idx];
}


void KEduVocExpression::setSynonym (int idx, const QString & expr)
{
  if ( idx < 0) return;

  // extend synonym with empty strings if necessary
  if ((int)m_synonym.size() <= idx )
    for (int i = m_synonym.size(); i < idx+1; i++)
      m_synonym.push_back ("-");

  m_synonym[idx] = expr.stripWhiteSpace();
}


QString KEduVocExpression::synonym (int idx) const
{
  if (idx >= (int)m_synonym.size() || idx < 0) {
    return "";
  }
  else {
    return m_synonym[idx];
  }
}


void KEduVocExpression::setExample (int idx, const QString & expr)
{
  if ( idx < 0) return;

  // extend exampls with empty strings if necessary
  if ((int)m_example.size() <= idx )
    for (int i = m_example.size(); i < idx+1; i++)
      m_example.push_back ("");

  m_example[idx] = expr.stripWhiteSpace();
}


QString KEduVocExpression::example (int idx) const
{
  if (idx >= (int)m_example.size() || idx < 0) {
    return "";
  }
  else {
    return m_example[idx];
  }
}


void KEduVocExpression::setUsageLabel (int idx, const QString & expr)
{
  if ( idx < 0) return;

  // extend labels with empty strings if necessary
  if ((int)usageLabels.size() <= idx )
    for (int i = usageLabels.size(); i < idx+1; i++)
      usageLabels.push_back ("");

  usageLabels[idx] = expr.stripWhiteSpace();
}


QString KEduVocExpression::usageLabel (int idx) const
{
  if (idx >= (int)usageLabels.size() || idx < 0) {
    return "";
  }
  else {
    return usageLabels[idx];
  }
}


void KEduVocExpression::setParaphrase (int idx, const QString & expr)
{
  if ( idx < 0) return;

  // extend phrase with empty strings if necessary
  if ((int) paraphrases.size() <= idx )
    for (int i = paraphrases.size(); i < idx+1; i++)
      paraphrases.push_back ("");

  paraphrases[idx] = expr.stripWhiteSpace();
}


QString KEduVocExpression::paraphrase (int idx) const
{
  if (idx >= (int)paraphrases.size() || idx < 0) {
    return "";
  }
  else {
    return paraphrases[idx];
  }
}


void KEduVocExpression::setAntonym (int idx, const QString & expr)
{
  if ( idx < 0) return;

  // extend antonym with empty strings if necessary
  if ((int)m_antonym.size() <= idx )
    for (int i = m_antonym.size(); i < idx+1; i++)
      m_antonym.push_back ("");

  m_antonym[idx] = expr.stripWhiteSpace();
}


QString KEduVocExpression::antonym (int idx) const
{
  if (idx >= (int)m_antonym.size() || idx < 0) {
    return "";
  }
  else {
    return m_antonym[idx];
  }
}


void KEduVocExpression::setConjugation (int idx, const Conjugation &con)
{
  if ( idx < 0) return;

  // extend conjugation with empty elements
  if ((int)conjugations.size() <= idx )
    for (int i = conjugations.size(); i < idx+1; i++)
      conjugations.push_back (Conjugation());

  conjugations[idx] = con;
}


Conjugation KEduVocExpression::conjugation (int idx) const
{
  if (idx >= (int)conjugations.size() || idx < 0) {
    return Conjugation();
  }
  else {
    return conjugations[idx];
  }
}


void KEduVocExpression::setComparison (int idx, const Comparison &con)
{
  if ( idx < 0) return;

  // extend comparison with empty elements
  if ((int)comparisons.size() <= idx )
    for (int i = comparisons.size(); i < idx+1; i++)
      comparisons.push_back (Comparison());

  comparisons[idx] = con;
}


Comparison KEduVocExpression::comparison (int idx) const
{
  if (idx >= (int)comparisons.size() || idx < 0) {
    return Comparison();
  }
  else {
    return comparisons[idx];
  }
}


void KEduVocExpression::setMultipleChoice (int idx, const MultipleChoice &mc)
{
  if ( idx < 0) return;

  // extend comparison with empty elements
  if ((int)mcs.size() <= idx )
    for (int i = mcs.size(); i < idx+1; i++)
      mcs.push_back (MultipleChoice());

  mcs[idx] = mc;
}


MultipleChoice KEduVocExpression::multipleChoice (int idx) const
{
  if (idx >= (int)mcs.size() || idx < 0) {
    return MultipleChoice();
  }
  else {
    return mcs[idx];
  }
}


QString KEduVocExpression::pronounce (int idx) const
{
  if (idx >= (int)pronunces.size() || idx < 0) {
    return "";
  }
  else {
    return pronunces[idx];
  }
}


void KEduVocExpression::setPronunce (int idx, const QString & expr)
{
  if ( idx < 0) return;

  // extend with empty strings if necessary
  if ((int)pronunces.size() <= idx )
    for (int i = pronunces.size(); i < idx+1; i++)
      pronunces.push_back ("");

  pronunces[idx] = expr.stripWhiteSpace();
}


void KEduVocExpression::addTranslation (QString expr,
                                    grade_t grade, grade_t rev_grade)
{
  if (grade > KV_MAX_GRADE)
    grade = KV_MAX_GRADE;

  if (rev_grade > KV_MAX_GRADE)
    rev_grade = KV_MAX_GRADE;

  grades.push_back (grade);
  rev_grades.push_back (rev_grade);
  translations.push_back (expr.stripWhiteSpace());
}


QString KEduVocExpression::translation (int idx) const
{
  if (idx > (int)translations.size() || idx < 1)
    return "";
  else
    return translations[idx-1];
}


void KEduVocExpression::removeTranslation (int idx)
{
   if (idx <= 0)
     return;

   if (idx <= numTranslations())
     translations.remove(translations.at(idx-1));

   if (idx < (int)remarks.size() )
     remarks.erase (remarks.begin() + idx);

   if (idx < (int)conjugations.size() )
     conjugations.erase (conjugations.begin() + idx);

   if (idx < (int)comparisons.size() )
     comparisons.erase (comparisons.begin() + idx);

   if (idx < (int)m_fauxAmi.size() )
     m_fauxAmi.erase (m_fauxAmi.begin() + idx);

   if (idx < (int)rev_fauxAmi.size() )
     rev_fauxAmi.erase (rev_fauxAmi.begin() + idx);

   if (idx < (int)m_synonym.size() )
     m_synonym.erase (m_synonym.begin() + idx);

   if (idx < (int)m_example.size() )
     m_example.erase (m_example.begin() + idx);

   if (idx < (int)usageLabels.size() )
     usageLabels.erase (usageLabels.begin() + idx);

   if (idx < (int)paraphrases.size() )
     paraphrases.erase (paraphrases.begin() + idx);

   if (idx < (int)m_antonym.size() )
     m_antonym.erase (m_antonym.begin() + idx);

   if (idx < (int)exprtypes.size() )
     exprtypes.erase (exprtypes.begin() + idx);

   if (idx < (int)pronunces.size() )
     pronunces.erase (pronunces.begin() + idx);

   if (idx < (int)grades.size() )
     grades.erase (grades.begin() + idx);

   if (idx < (int)rev_grades.size() )
     rev_grades.erase (rev_grades.begin() + idx);

   if (idx < (int)qcounts.size() )
     qcounts.erase (qcounts.begin() + idx);

   if (idx < (int)rev_qcounts.size() )
     rev_qcounts.erase (rev_qcounts.begin() + idx);

   if (idx < (int)bcounts.size() )
     bcounts.erase (bcounts.begin() + idx);

   if (idx < (int)rev_bcounts.size() )
     rev_bcounts.erase (rev_bcounts.begin() + idx);

   if (idx < (int)qdates.size() )
     qdates.erase (qdates.begin() + idx);

   if (idx < (int)rev_qdates.size() )
     rev_qdates.erase (rev_qdates.begin() + idx);
}


void KEduVocExpression::setTranslation (int idx, const QString & expr)
{
  if ( idx <= 0) return;

  // extend translations with empty strings if necessary
  if ((int)translations.size() < idx )
    for (int i = translations.size(); i < idx; i++)
      translations.push_back ("");

//  if (idx <= (int)translations.size())
  translations[idx-1] = expr.stripWhiteSpace();
}


QString KEduVocExpression::gradeStr (int idx, bool rev_grade) const
{
  QString s;
  s.setNum(grade(idx, rev_grade));
  return s;
}


grade_t KEduVocExpression::grade (int idx, bool rev_grade) const
{
  if (rev_grade) {
    if (idx >= (int)rev_grades.size() || idx < 1 ) {
      return KV_NORM_GRADE;
    }
    else if (rev_grades[idx] > KV_MAX_GRADE) {
      return KV_MAX_GRADE;
    }

    return rev_grades[idx];

  }
  else {
    if (idx >= (int)grades.size() || idx < 1 ) {
      return KV_NORM_GRADE;
    }
    else if (grades[idx] > KV_MAX_GRADE) {
      return KV_MAX_GRADE;
    }

    return grades[idx];
  }
}


void KEduVocExpression::setGrade (int idx, grade_t grade, bool rev_grade)
{
  if (idx < 1) return;

  if (grade > KV_MAX_GRADE)
    grade = KV_MAX_GRADE;
  if (grade < KV_MIN_GRADE)
    grade = KV_MIN_GRADE;

  if (rev_grade) {
    // extend rev grades with standard grade if necessary
    if ((int)rev_grades.size() <= idx )
      for (int i = rev_grades.size(); i <= idx; i++) {
        rev_grades.push_back (KV_NORM_GRADE);
    }
    rev_grades[idx] = grade;
  }
  else {
    // extend grades with standard grade if necessary
    if ((int)grades.size() <= idx )
      for (int i = grades.size(); i <= idx; i++) {
        grades.push_back (KV_NORM_GRADE);
      }
    grades[idx] = grade;
  }
}


void KEduVocExpression::incGrade (int idx, bool rev_grade)
{
  if (idx < 1) return;

  if (rev_grade) {
    // extend rev grades with standard grade if necessary
    if ((int)rev_grades.size() <= idx )
      for (int i = rev_grades.size(); i <= idx; i++) {
        rev_grades.push_back (KV_NORM_GRADE);
    }
    if (rev_grades[idx] < KV_MAX_GRADE)
      rev_grades[idx]++;
  }
  else {
    // extend grades with standard grade if necessary
    if ((int)grades.size() <= idx )
      for (int i = grades.size(); i <= idx; i++) {
        grades.push_back (KV_NORM_GRADE);
      }
    if (grades[idx] < KV_MAX_GRADE)
      grades[idx]++;
  }
}


void KEduVocExpression::decGrade (int idx, bool rev_grade)
{
  if (idx < 1) return;

  if (rev_grade) {
    // extend rev grades with standard grade if necessary
    if ((int)rev_grades.size() <= idx )
      for (int i = rev_grades.size(); i <= idx; i++) {
        rev_grades.push_back (KV_NORM_GRADE);
    }
    if (rev_grades[idx] > KV_MIN_GRADE)
      rev_grades[idx]--;
  }
  else {
    // extend grades with standard grade if necessary
    if ((int)grades.size() <= idx )
      for (int i = grades.size(); i <= idx; i++) {
        grades.push_back (KV_NORM_GRADE);
      }
    if (grades[idx] > KV_MIN_GRADE)
      grades[idx]--;
  }
}


count_t KEduVocExpression::queryCount (int idx, bool rev_count)  const
{
  if (rev_count) {
    if (idx >= (int)rev_qcounts.size() || idx < 1 ) {
      return 0;
    }

    return rev_qcounts[idx];
  }

  if (idx >= (int)qcounts.size() || idx < 1 ) {
    return 0;
  }

  return qcounts[idx];
}


void KEduVocExpression::setQueryCount (int idx, count_t count, bool rev_count)
{
  if (idx < 1) return;

  if (rev_count) {
    // extend rev counts with 0 if necessary
    if ((int)rev_qcounts.size() <= idx )
      for (int i = rev_qcounts.size(); i <= idx; i++) {
        rev_qcounts.push_back (0);
    }

    rev_qcounts[idx] = count;

  }
  else {
    // extend counts with 0 if necessary
    if ((int)qcounts.size() <= idx )
      for (int i = qcounts.size(); i <= idx; i++) {
        qcounts.push_back (0);
      }
    qcounts[idx] = count;
  }
}


count_t KEduVocExpression::badCount (int idx, bool rev_count) const
{
  if (rev_count) {
    if (idx >= (int)rev_bcounts.size() || idx < 1 ) {
      return 0;
    }

    return rev_bcounts[idx];
  }

  if (idx >= (int)bcounts.size() || idx < 1 ) {
    return 0;
  }

  return bcounts[idx];
}


void KEduVocExpression::setBadCount (int idx, count_t count, bool rev_count)
{
  if (idx < 1) return;

  if (rev_count) {
    // extend rev counts with 0 if necessary
    if ((int)rev_bcounts.size() <= idx )
      for (int i = rev_bcounts.size(); i <= idx; i++) {
        rev_bcounts.push_back (0);
    }

    rev_bcounts[idx] = count;

  }
  else {
    // extend counts with 0 if necessary
    if ((int)bcounts.size() <= idx )
      for (int i = bcounts.size(); i <= idx; i++) {
        bcounts.push_back (0);
      }
    bcounts[idx] = count;
  }
}


time_t KEduVocExpression::queryDate (int idx, bool rev_date) const
{
  if (rev_date) {
    if (idx >= (int)rev_qdates.size() || idx < 1 ) {
      return 0;
    }

    return rev_qdates[idx];
  }

  if (idx >= (int)qdates.size() || idx < 1 ) {
    return 0;
  }

  return qdates[idx];
}


void KEduVocExpression::setQueryDate (int idx, time_t date, bool rev_date)
{
  if (idx < 1) return;

  if (rev_date) {
    // extend rev dates with 0 if necessary
    if ((int)rev_qdates.size() <= idx )
      for (int i = rev_qdates.size(); i <= idx; i++) {
        rev_qdates.push_back (0);
    }

    rev_qdates[idx] = date;

  }
  else {
    // extend dates with 0 if necessary
    if ((int)qdates.size() <= idx )
      for (int i = qdates.size(); i <= idx; i++) {
        qdates.push_back (0);
      }
    qdates[idx] = date;
  }
}


bool KEduVocExpression::uniqueType () const
{
  bool unique = true;
  QString type0 = type(0);
  for (int i = 1; i < numTranslations(); i++)
    if (type0 != type(i) )
      unique = false;
  return unique;
}


QString KEduVocExpression::type (int idx) const
{
  if (idx >= (int)exprtypes.size() || idx < 0) {
    return "";
  }
  else {
    return exprtypes[idx];
  }
}


int KEduVocExpression::lesson () const
{
  return m_lesson;
}

void KEduVocExpression::incQueryCount (int index, bool rev_count)
{
  setQueryCount (index, queryCount(index, rev_count)+1, rev_count);
}


void KEduVocExpression::incBadCount (int index, bool rev_count)
{
  setBadCount (index, badCount(index, rev_count)+1, rev_count);
}


void KEduVocExpression::setOriginal ( const QString & expr)
{
  origin = expr;
}


QString KEduVocExpression::original () const
{
  return origin;
}


void KEduVocExpression::setLesson (int l)
{
  m_lesson = l;
}


void KEduVocExpression::setType (int idx, const QString &type)
{
  if ( idx < 0) return;

  // extend types with empty strings if necessary
  if ((int)exprtypes.size() <= idx )
    for (int i = exprtypes.size(); i < idx+1; i++)
      exprtypes.push_back ("");

  exprtypes[idx] = type.stripWhiteSpace();
}

void KEduVocExpression::setLeitnerBox( const QString& box )
{
	m_leitnerBox = box;
}

QString& KEduVocExpression::leitnerBox()
{
	return m_leitnerBox;
}

