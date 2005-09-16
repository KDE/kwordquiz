/***************************************************************************
                        Vocabulary Expression for KDE Edu
    -----------------------------------------------------------------------
    copyright            : (C) 1999-2001 Ewald Arnold
                           (C) 2001 The KDE-EDU team
                           (C) 2005 Peter Hedlund
    email                : peter.hedlund@kdemail.net
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
  m_grades.append(KV_NORM_GRADE);
  m_reverseGrades.append(KV_NORM_GRADE);
  m_inQuery = false;
  m_active = true;
  m_queryCounts.append(0);
  m_reverseQueryCounts.append(0);
  m_badCounts.append(0);
  m_reverseBadCounts.append(0);
  QDateTime dt;
  m_queryDates.append(dt);
  m_reverseQueryDates.append(dt);
  m_lesson = 0;
}


KEduVocExpression::KEduVocExpression(const QString & expression, int lesson)
{
  Init();
  setOriginal(expression.stripWhiteSpace() );
  m_lesson = lesson;
}


KEduVocExpression::KEduVocExpression ()
{
  Init();
}


KEduVocExpression::KEduVocExpression(const QString &s, const QString & separator, int lesson)
{
  Init();
  QString se;
  QString expr = s;
  m_lesson = lesson;

  if (separator.length() ) {
    int pos = expr.find(separator);

    if (pos == -1) {
      setOriginal(expr.stripWhiteSpace());
    }
    else {
      se = expr.left(pos).stripWhiteSpace();
      setOriginal(se);
      expr.remove (0, pos + separator.length() );
//      s.stripWhiteSpace();

      // gather all translations
      while ((pos = expr.find(separator)) != -1) {
        se = expr.left(pos).stripWhiteSpace();
        addTranslation(se, KV_NORM_GRADE, KV_NORM_GRADE);
        expr.remove (0, pos + separator.length() );
//        s.stripWhiteSpace();
      }
      addTranslation(expr.stripWhiteSpace(), KV_NORM_GRADE, KV_NORM_GRADE);
    }
  }
}


KEduVocExpression::~KEduVocExpression()
{
}


int KEduVocExpression::numTranslations() const
{
  return m_translations.count();
}


QString KEduVocExpression::remark (int idx) const
{
  if (idx >= (int)m_remarks.count() || idx < 0) {
    return "";
  }
  else {
    return m_remarks[idx];
  }
}


void KEduVocExpression::setRemark (int idx, const QString & expr)
{
  if ( idx < 0) return;

  // extend remarks with empty strings if necessary
  if ((int)m_remarks.size() <= idx )
    for (int i = m_remarks.size(); i < idx+1; i++)
      m_remarks.push_back ("");

  m_remarks[idx] = expr.stripWhiteSpace();
}


void KEduVocExpression::setFauxAmi (int idx, const QString & expr, bool rev_ami)
{
  if (idx < 1) return;

  if (rev_ami) {
    // extend friend with empty strings if necessary
    if ((int)m_reverseFauxAmi.size() <= idx )
      for (int i = m_reverseFauxAmi.size(); i < idx+1; i++)
        m_reverseFauxAmi.push_back ("");

    m_reverseFauxAmi[idx] = expr.stripWhiteSpace();

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
    if (idx >= (int)m_reverseFauxAmi.size() || idx < 1 ) {
      return "";
    }

    return m_reverseFauxAmi[idx];
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
  if ((int)m_usageLabels.size() <= idx )
    for (int i = m_usageLabels.size(); i < idx+1; i++)
      m_usageLabels.push_back ("");

  m_usageLabels[idx] = expr.stripWhiteSpace();
}


QString KEduVocExpression::usageLabel (int idx) const
{
  if (idx >= (int)m_usageLabels.size() || idx < 0) {
    return "";
  }
  else {
    return m_usageLabels[idx];
  }
}


void KEduVocExpression::setParaphrase (int idx, const QString & expr)
{
  if ( idx < 0) return;

  // extend phrase with empty strings if necessary
  if ((int) m_paraphrases.size() <= idx )
    for (int i = m_paraphrases.size(); i < idx+1; i++)
      m_paraphrases.push_back ("");

  m_paraphrases[idx] = expr.stripWhiteSpace();
}


QString KEduVocExpression::paraphrase (int idx) const
{
  if (idx >= (int)m_paraphrases.size() || idx < 0) {
    return "";
  }
  else {
    return m_paraphrases[idx];
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
  if ((int)m_conjugations.size() <= idx )
    for (int i = m_conjugations.size(); i < idx+1; i++)
      m_conjugations.push_back (Conjugation());

  m_conjugations[idx] = con;
}


Conjugation KEduVocExpression::conjugation (int idx) const
{
  if (idx >= (int)m_conjugations.size() || idx < 0) {
    return Conjugation();
  }
  else {
    return m_conjugations[idx];
  }
}


void KEduVocExpression::setComparison (int idx, const Comparison &con)
{
  if ( idx < 0) return;

  // extend comparison with empty elements
  if ((int)m_comparisons.size() <= idx )
    for (int i = m_comparisons.size(); i < idx+1; i++)
      m_comparisons.push_back (Comparison());

  m_comparisons[idx] = con;
}


Comparison KEduVocExpression::comparison (int idx) const
{
  if (idx >= (int)m_comparisons.size() || idx < 0) {
    return Comparison();
  }
  else {
    return m_comparisons[idx];
  }
}


void KEduVocExpression::setMultipleChoice (int idx, const MultipleChoice &mc)
{
  if ( idx < 0) return;

  // extend comparison with empty elements
  if ((int)m_multipleChoices.size() <= idx )
    for (int i = m_multipleChoices.size(); i < idx+1; i++)
      m_multipleChoices.push_back (MultipleChoice());

  m_multipleChoices[idx] = mc;
}


MultipleChoice KEduVocExpression::multipleChoice (int idx) const
{
  if (idx >= (int)m_multipleChoices.size() || idx < 0) {
    return MultipleChoice();
  }
  else {
    return m_multipleChoices[idx];
  }
}


QString KEduVocExpression::pronounciation(int idx) const
{
  if (idx >= (int)m_pronounciations.size() || idx < 0) {
    return "";
  }
  else {
    return m_pronounciations[idx];
  }
}


void KEduVocExpression::setPronounciation(int idx, const QString & expr)
{
  if ( idx < 0) return;

  // extend with empty strings if necessary
  if ((int)m_pronounciations.size() <= idx )
    for (int i = m_pronounciations.size(); i < idx+1; i++)
      m_pronounciations.push_back ("");

  m_pronounciations[idx] = expr.stripWhiteSpace();
}


void KEduVocExpression::addTranslation(const QString & expr, grade_t grade, grade_t rev_grade)
{
  if (grade > KV_MAX_GRADE)
    grade = KV_MAX_GRADE;

  if (rev_grade > KV_MAX_GRADE)
    rev_grade = KV_MAX_GRADE;

  m_grades.push_back (grade);
  m_reverseGrades.push_back (rev_grade);
  m_translations.push_back (expr.stripWhiteSpace());
}


QString KEduVocExpression::translation (int idx) const
{
  if (idx > (int)m_translations.size() || idx < 1)
    return "";
  else
    return m_translations[idx-1];
}


void KEduVocExpression::removeTranslation (int idx)
{
   if (idx <= 0)
     return;

   if (idx <= numTranslations())
     m_translations.remove(m_translations.at(idx - 1));

   if (idx < (int)m_remarks.size() )
     m_remarks.remove(m_remarks.at(idx - 1));

   if (idx < (int)m_conjugations.size() )
     m_conjugations.remove(m_conjugations.at(idx -1));

   if (idx < (int)m_comparisons.size() )
     m_comparisons.remove(m_comparisons.at(idx - 1));

   if (idx < (int)m_fauxAmi.size() )
     m_fauxAmi.remove(m_fauxAmi.at(idx -  1));

   if (idx < (int)m_reverseFauxAmi.size() )
     m_reverseFauxAmi.remove(m_reverseFauxAmi.at(idx - 1));

   if (idx < (int)m_synonym.size() )
     m_synonym.remove(m_synonym.at(idx - 1));

   if (idx < (int)m_example.size() )
     m_example.remove(m_example.at(idx - 1));

   if (idx < (int)m_usageLabels.size() )
     m_usageLabels.remove(m_usageLabels.at(idx - 1));

   if (idx < (int)m_paraphrases.size() )
     m_paraphrases.remove(m_paraphrases.at(idx - 1));

   if (idx < (int)m_antonym.size() )
     m_antonym.remove(m_antonym.at(idx - 1));

   if (idx < (int)m_expressionTypes.size() )
     m_expressionTypes.remove(m_expressionTypes.at(idx - 1));

   if (idx < (int)m_pronounciations.size() )
     m_pronounciations.remove(m_pronounciations.at(idx - 1));

   if (idx < (int)m_grades.size() )
     m_grades.remove(m_grades.at(idx - 1));

   if (idx < (int)m_reverseGrades.size() )
     m_reverseGrades.remove(m_reverseGrades.at(idx - 1));

   if (idx < (int)m_queryCounts.size() )
     m_queryCounts.remove(m_queryCounts.at(idx - 1));

   if (idx < (int)m_reverseQueryCounts.size() )
     m_reverseQueryCounts.remove(m_reverseQueryCounts.at(idx - 1));

   if (idx < (int)m_badCounts.size() )
     m_badCounts.remove(m_badCounts.at(idx - 1));

   if (idx < (int)m_reverseBadCounts.size() )
     m_reverseBadCounts.remove(m_reverseBadCounts.at(idx - 1));

   if (idx < (int)m_queryDates.size() )
     m_queryDates.remove(m_queryDates.at(idx - 1));

   if (idx < (int)m_reverseQueryDates.size() )
     m_reverseQueryDates.remove(m_reverseQueryDates.at(idx - 1));
}


void KEduVocExpression::setTranslation (int idx, const QString & expr)
{
  if ( idx <= 0) return;

  // extend translations with empty strings if necessary
  if ((int)m_translations.size() < idx )
    for (int i = m_translations.size(); i < idx; i++)
      m_translations.push_back ("");

//  if (idx <= (int)translations.size())
  m_translations[idx-1] = expr.stripWhiteSpace();
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
    if (idx >= (int)m_reverseGrades.size() || idx < 1 ) {
      return KV_NORM_GRADE;
    }
    else if (m_reverseGrades[idx] > KV_MAX_GRADE) {
      return KV_MAX_GRADE;
    }

    return m_reverseGrades[idx];

  }
  else {
    if (idx >= (int)m_grades.size() || idx < 1 ) {
      return KV_NORM_GRADE;
    }
    else if (m_grades[idx] > KV_MAX_GRADE) {
      return KV_MAX_GRADE;
    }

    return m_grades[idx];
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
    if ((int)m_reverseGrades.size() <= idx )
      for (int i = m_reverseGrades.size(); i <= idx; i++) {
        m_reverseGrades.push_back (KV_NORM_GRADE);
    }
    m_reverseGrades[idx] = grade;
  }
  else {
    // extend grades with standard grade if necessary
    if ((int)m_grades.size() <= idx )
      for (int i = m_grades.size(); i <= idx; i++) {
        m_grades.push_back (KV_NORM_GRADE);
      }
    m_grades[idx] = grade;
  }
}


void KEduVocExpression::incGrade (int idx, bool rev_grade)
{
  if (idx < 1) return;

  if (rev_grade) {
    // extend rev grades with standard grade if necessary
    if ((int)m_reverseGrades.size() <= idx )
      for (int i = m_reverseGrades.size(); i <= idx; i++) {
        m_reverseGrades.push_back (KV_NORM_GRADE);
    }
    if (m_reverseGrades[idx] < KV_MAX_GRADE)
      m_reverseGrades[idx]++;
  }
  else {
    // extend grades with standard grade if necessary
    if ((int)m_grades.size() <= idx )
      for (int i = m_grades.size(); i <= idx; i++) {
        m_grades.push_back (KV_NORM_GRADE);
      }
    if (m_grades[idx] < KV_MAX_GRADE)
      m_grades[idx]++;
  }
}


void KEduVocExpression::decGrade (int idx, bool rev_grade)
{
  if (idx < 1) return;

  if (rev_grade) {
    // extend rev grades with standard grade if necessary
    if ((int)m_reverseGrades.size() <= idx )
      for (int i = m_reverseGrades.size(); i <= idx; i++) {
        m_reverseGrades.push_back (KV_NORM_GRADE);
    }
    if (m_reverseGrades[idx] > KV_MIN_GRADE)
      m_reverseGrades[idx]--;
  }
  else {
    // extend grades with standard grade if necessary
    if ((int)m_grades.size() <= idx )
      for (int i = m_grades.size(); i <= idx; i++) {
        m_grades.push_back (KV_NORM_GRADE);
      }
    if (m_grades[idx] > KV_MIN_GRADE)
      m_grades[idx]--;
  }
}


count_t KEduVocExpression::queryCount (int idx, bool rev_count)  const
{
  if (rev_count) {
    if (idx >= (int)m_reverseQueryCounts.size() || idx < 1 ) {
      return 0;
    }

    return m_reverseQueryCounts[idx];
  }

  if (idx >= (int)m_queryCounts.size() || idx < 1 ) {
    return 0;
  }

  return m_queryCounts[idx];
}


void KEduVocExpression::setQueryCount (int idx, count_t count, bool rev_count)
{
  if (idx < 1) return;

  if (rev_count) {
    // extend rev counts with 0 if necessary
    if ((int)m_reverseQueryCounts.size() <= idx )
      for (int i = m_reverseQueryCounts.size(); i <= idx; i++) {
        m_reverseQueryCounts.push_back (0);
    }

    m_reverseQueryCounts[idx] = count;

  }
  else {
    // extend counts with 0 if necessary
    if ((int)m_queryCounts.size() <= idx )
      for (int i = m_queryCounts.size(); i <= idx; i++) {
        m_queryCounts.push_back (0);
      }
    m_queryCounts[idx] = count;
  }
}


count_t KEduVocExpression::badCount (int idx, bool rev_count) const
{
  if (rev_count) {
    if (idx >= (int)m_reverseBadCounts.size() || idx < 1 ) {
      return 0;
    }

    return m_reverseBadCounts[idx];
  }

  if (idx >= (int)m_badCounts.size() || idx < 1 ) {
    return 0;
  }

  return m_badCounts[idx];
}


void KEduVocExpression::setBadCount (int idx, count_t count, bool rev_count)
{
  if (idx < 1) return;

  if (rev_count) {
    // extend rev counts with 0 if necessary
    if ((int)m_reverseBadCounts.size() <= idx )
      for (int i = m_reverseBadCounts.size(); i <= idx; i++) {
        m_reverseBadCounts.push_back (0);
    }

    m_reverseBadCounts[idx] = count;

  }
  else {
    // extend counts with 0 if necessary
    if ((int)m_badCounts.size() <= idx )
      for (int i = m_badCounts.size(); i <= idx; i++) {
        m_badCounts.push_back (0);
      }
    m_badCounts[idx] = count;
  }
}


QDateTime KEduVocExpression::queryDate (int idx, bool rev_date) const
{
  if (rev_date) {
    if (idx >= (int)m_reverseQueryDates.size() || idx < 1 )
    {
      QDateTime dt;
      dt.setTime_t(0);
      return dt;
    }

    return m_reverseQueryDates[idx];
  }

  if (idx >= (int)m_queryDates.size() || idx < 1 )
  {
    QDateTime dt;
    dt.setTime_t(0);
    return dt;
  }

  return m_queryDates[idx];
}


void KEduVocExpression::setQueryDate (int idx, const QDateTime & date, bool rev_date)
{
  if (idx < 1) return;

  if (rev_date) {
    // extend rev dates with 0 if necessary
    if ((int)m_reverseQueryDates.size() <= idx )
      for (int i = m_reverseQueryDates.size(); i <= idx; i++)
      {
        QDateTime dt;
        dt.setTime_t(0);
        m_reverseQueryDates.append(dt);
      }

    m_reverseQueryDates[idx] = date;

  }
  else {
    // extend dates with 0 if necessary
    if ((int)m_queryDates.size() <= idx )
      for (int i = m_queryDates.size(); i <= idx; i++)
      {
        QDateTime dt;
        dt.setTime_t(0);
        m_queryDates.append(dt);
      }
    m_queryDates[idx] = date;
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
  if (idx >= (int)m_expressionTypes.count() || idx < 0) {
    return "";
  }
  else {
    return m_expressionTypes[idx];
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
  m_original = expr;
}


QString KEduVocExpression::original () const
{
  return m_original;
}


void KEduVocExpression::setLesson (int l)
{
  m_lesson = l;
}


void KEduVocExpression::setType (int idx, const QString &type)
{
  if ( idx < 0) return;

  // extend types with empty strings if necessary
  if ((int)m_expressionTypes.count() <= idx )
    for (int i = m_expressionTypes.count(); i < idx + 1; i++)
      m_expressionTypes.append("");

  m_expressionTypes[idx] = type.stripWhiteSpace();
}

void KEduVocExpression::setLeitnerBox( const QString& box )
{
	m_leitnerBox = box;
}

QString& KEduVocExpression::leitnerBox()
{
	return m_leitnerBox;
}

