/***************************************************************************
                        Vocabulary Document for KDE Edu
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

#include "keduvocdocument.h"

#include <qfileinfo.h>

#include <kapplication.h>
#include <klocale.h>
#include <kdebug.h>
#include <kmessagebox.h>
#include <kio/netaccess.h>

using namespace std;

#include <iostream>

#include "keduvockvtmlwriter.h"
#include "keduvockvtmlreader.h"
#include "keduvocwqlwriter.h"
#include "keduvocwqlreader.h"
#include "leitnersystem.h"

//********************************************************
//  KEduVocDocument
//********************************************************

KEduVocDocument::KEduVocDocument(QObject */*parent*/)
{
  Init();
}


KEduVocDocument::~KEduVocDocument()
{
  delete m_font;
}


void KEduVocDocument::setVersion (const QString & vers)
{
  m_version = vers;
}


void KEduVocDocument::version(int &, int &, int &)
{
}


void KEduVocDocument::Init ()
{
  m_lessonDescriptions.clear();
  m_typeDescriptions.clear();
  m_tenseDescriptions.clear();
  m_identifiers.clear();
  m_sortIdentifier.clear();
  m_extraSizeHints.clear();
  m_sizeHints.clear();
  m_vocabulary.clear();
  m_dirty = false;
  m_enableSorting = true;
  m_unknownAttribute = false;
  m_unknownElement = false;
  m_sortLesson = false;
  setCurrentLesson (0);
  m_queryorg = "";
  m_querytrans = "";
  m_url.setFileName(i18n("Untitled"));
  m_title = "";
  m_author = "";
  m_remark = "";
  m_version = "";
  m_font = NULL;

  m_activeLeitnerSystem = false;
  m_leitnerSystem = NULL;
}


bool KEduVocDocument::open(const KURL& url, bool /*append*/)
{
  Init();
  if (!url.isEmpty())
    m_url = url;

  // TODO EPT  connect( this, SIGNAL(progressChanged(KEduVocDocument*,int)), parent, SLOT(slotProgress(KEduVocDocument*,int)) );

  QString tmpfile;
  if (KIO::NetAccess::download( url, tmpfile, 0 ))
  {
    QFile f(tmpfile);
    if (!f.open(IO_ReadOnly))
    {
      KMessageBox::error(0, i18n("<qt>Cannot open file<br><b>%1</b></qt>").arg(url.path()));
      return false;
    }

    FileType ft = detectFileType(url.path());

    bool read = false;
    while (!read) {

      QApplication::setOverrideCursor( waitCursor );
      switch (ft) {
        case kvtml:
        {
          KEduVocKvtmlReader kvtmlReader(&f);
          read = kvtmlReader.readDoc(this);
        }
        break;

        case wql:
        {
          KEduVocWqlReader wqlReader(&f);
          read = wqlReader.readDoc(this);
        }
        break;

        case vt_lex:
        {
          QTextStream is (&f);
          //TODO read = loadFromLex (is);
        }
        break;

        case vt_vcb:
        {
          QTextStream is (&f);
          //TODO read = loadFromVcb (is);
        }
        break;

        case csv:
        {
          QTextStream is(&f);
          //TODO read = loadFromCsv(is);
        }
        break;

        default:
        {
          KEduVocKvtmlReader kvtmlReader(&f);
          read = kvtmlReader.readDoc(this);
        }
      }

      QApplication::restoreOverrideCursor();

      if (!read) {
        if (m_unknownAttribute || m_unknownElement ) {
          Init();
          return false;
        }
        // TODO new readers provide an explicite error message
        // the two messages should be merged
        QString format = i18n("Could not load \"%1\"\nDo you want to try again?");
        QString msg = format.arg(url.path());
        int result = KMessageBox::warningContinueCancel(0, msg,
                                                        kapp->makeStdCaption(i18n("I/O Failure")),
                                                        i18n("&Retry"));
        if ( result == KMessageBox::Cancel ) {
          Init();
          return false;
        }
      }
    }
    f.close();
    KIO::NetAccess::removeTempFile( tmpfile );
  }
  return true;
}


bool KEduVocDocument::saveAs(QObject *parent, const KURL & url, FileType ft, const QString & generator)
{
  connect( this, SIGNAL(progressChanged(KEduVocDocument*,int)), parent, SLOT(slotProgress(KEduVocDocument*,int)) );

  KURL tmp (url);

  if (ft == automatic)
  {
    if (tmp.path().right(strlen("." KVTML_EXT)) == "." KVTML_EXT)
      ft = kvtml;
    else if (tmp.path().right(strlen("." WQL_EXT)) == "." WQL_EXT)
      ft = wql;
    else if (tmp.path().right(strlen("." VT5_LEX_EXT)) == "." VT5_LEX_EXT)
      ft = vt_lex;
    else if (tmp.path().right(strlen("." VCB_EXT)) == "." VCB_EXT)
      ft = vt_vcb;
    else if (tmp.path().right(strlen("." CSV_EXT)) == "." CSV_EXT)
      ft = csv;
    else
    {
      tmp.setFileName(tmp.path() + "." KVTML_EXT);
      ft = kvtml;
    }
  }

  bool saved = false;
  while (!saved)
  {

    QFile f(tmp.path());

    if (!f.open(IO_WriteOnly))
    {
      KMessageBox::error(0, i18n("<qt>Cannot write to file<br><b>%1</b></qt>").arg(tmp.path()));
      return false;
    }

    QApplication::setOverrideCursor( waitCursor );
    switch (ft) {
      case kvtml: {
        KEduVocKvtmlWriter kvtmlWriter(&f);
        saved = kvtmlWriter.writeDoc(this, generator);
      }
      break;

      case wql: {
        KEduVocWqlWriter wqlWriter(&f);
        saved = wqlWriter.writeDoc(this);
      }
      break;

      case vt_lex: {
        QTextStream os( &f );                       // serialize using f
        //TODO saved = saveToLex(os, title);
      }
      break;

      case vt_vcb: {
        QTextStream os( &f );                       // serialize using f
        //TODO saved = saveToVcb(os, title);
      }
      break;

      case csv: {
        QTextStream os( &f );                       // serialize using f
        //TODO saved = saveToCsv(os, title);
      }
      break;

      default: {
        kdError() << "kvcotrainDoc::saveAs(): unknown filetype" << endl;
      }
      break;
    }
    f.close();
    QApplication::restoreOverrideCursor();

    if (!saved) {
      // TODO new writers provide an explicite error message
      // the two messages should be merged
      QString format = i18n("Could not save \"%1\"\nDo you want to try again?");
      QString msg = format.arg(tmp.path());
      int result = KMessageBox::warningContinueCancel(0, msg,
                                                      kapp->makeStdCaption(i18n("I/O Failure")),
                                                      i18n("&Retry"));
      if ( result == KMessageBox::Cancel ) return false;
    }
  }
  m_url = tmp;
  m_dirty = false;
  emit docModified(false);
  return true;
}


KEduVocExpression *KEduVocDocument::entry(int index)
{
  if (index < 0 || index >= (int)m_vocabulary.size() )
    return 0;
  else
    return &m_vocabulary[index];
}


void KEduVocDocument::removeEntry(int index)
{
  if (index >= 0 && index < (int)m_vocabulary.size() )
    m_vocabulary.erase(m_vocabulary.at(index));
}


int KEduVocDocument::findIdentifier(const QString &lang) const
{
  QStringList::const_iterator first = m_identifiers.begin();
  int count = 0;
  while (first != m_identifiers.end()) {
    if ( *first == lang)
      return count;
    first++;
    count++;
  }
  return -1;
}


QString KEduVocDocument::identifier(int index) const
{
  if (index >= (int)m_identifiers.size() || index < 1 )
    return "";
  else
    return m_identifiers[index];
}


void KEduVocDocument::setIdentifier(int idx, const QString &id)
{
  if (idx < (int)m_identifiers.size() && idx >= 1 ) {
    m_identifiers[idx] = id;
  }
}


QString KEduVocDocument::typeName (int index) const
{
  if (index >= (int)m_typeDescriptions.count())
    return "";
  else
    return m_typeDescriptions[index];
}


void KEduVocDocument::setTypeName(int idx, QString &id)
{
  if (idx >= (int)m_typeDescriptions.size())
    for (int i = (int)m_typeDescriptions.size(); i <= idx; i++)
      m_typeDescriptions.push_back ("");

  m_typeDescriptions[idx] = id;
}


QString KEduVocDocument::tenseName(int index) const
{
  if (index >= (int)m_tenseDescriptions.size())
    return "";
  else
    return m_tenseDescriptions[index];
}


void KEduVocDocument::setTenseName(int idx, QString &id)
{
  if (idx >= (int)m_tenseDescriptions.size())
    for (int i = (int)m_tenseDescriptions.size(); i <= idx; i++)
      m_tenseDescriptions.push_back ("");

  m_tenseDescriptions[idx] = id;
}


QString KEduVocDocument::usageName(int index) const
{
  if (index >= (int)m_usageDescriptions.size())
    return "";
  else
    return m_usageDescriptions[index];
}


void KEduVocDocument::setUsageName(int idx, QString &id)
{
  if (idx >= (int)m_usageDescriptions.size())
    for (int i = (int)m_usageDescriptions.size(); i <= idx; i++)
      m_usageDescriptions.push_back ("");

  m_usageDescriptions[idx] = id;
}


void KEduVocDocument::setConjugation(int idx, const KEduVocConjugation &con)
{
  if ( idx < 0) return;

  // extend conjugation with empty elements
  if ((int)m_conjugations.size() <= idx )
    for (int i = m_conjugations.size(); i < idx+1; i++)
      m_conjugations.push_back (KEduVocConjugation());

  m_conjugations[idx] = con;
}


KEduVocConjugation KEduVocDocument::conjugation (int idx) const
{
  if (idx >= (int)m_conjugations.size() || idx < 0) {
    return KEduVocConjugation();
  }
  else {
    return m_conjugations[idx];
  }
}


void KEduVocDocument::setArticle(int idx, const KEduVocArticle &art)
{
  if ( idx < 0) return;

  // extend conjugation with empty elements
  if ((int)m_articles.size() <= idx )
    for (int i = m_articles.size(); i < idx+1; i++)
      m_articles.push_back (KEduVocArticle());

  m_articles[idx] = art;
}


KEduVocArticle KEduVocDocument::article (int idx) const
{
  if (idx >= (int)m_articles.size() || idx < 0) {
    return KEduVocArticle();
  }
  else {
    return m_articles[idx];
  }
}


int KEduVocDocument::sizeHint (int idx) const
{
  if (idx < 0) {
    idx = -idx;
    if (idx >= (int)m_extraSizeHints.size() )
      return 80; // make a good guess about column size
    else {
//      cout << "gsh " << idx << "  " << extraSizehints[idx] << endl;
      return m_extraSizeHints[idx];
    }
  }
  else {
    if (idx >= (int)m_sizeHints.size() )
      return 150; // make a good guess about column size
    else {
//      cout << "gsh " << idx << "  " << sizehints[idx] << endl;
      return m_sizeHints[idx];
    }
  }
}


void KEduVocDocument::setSizeHint (int idx, const int width)
{
//  cout << "ssh " << idx << "  " << width << endl;
  if (idx < 0) {
    idx = -idx;
    if (idx >= (int)m_extraSizeHints.size()) {
      for (int i = (int)m_extraSizeHints.size(); i <= idx; i++)
        m_extraSizeHints.push_back (80);
    }
    m_extraSizeHints[idx] = width;

  }
  else {
    if (idx >= (int)m_sizeHints.size()) {
      for (int i = (int)m_sizeHints.size(); i <= idx; i++)
        m_sizeHints.push_back (150);
    }
    m_sizeHints[idx] = width;
  }
}


class eraseTrans : public unary_function<KEduVocExpression, void>
{

public:

  eraseTrans (int idx)
    : index (idx) {}

  void operator() (KEduVocExpression& x) const
    {
      x.removeTranslation(index);
    }

 private:
    int index;
};


void KEduVocDocument::removeIdentifier(int index)
{
  if (index < (int)m_identifiers.size() && index >= 1 ) {
    m_identifiers.erase(m_identifiers.at(index));
    for_each (m_vocabulary.begin(), m_vocabulary.end(), eraseTrans(index));
  }
}


QString KEduVocDocument::originalIdentifier() const
{
  if (m_identifiers.size() > 0)
    return m_identifiers[0];
  else
    return "";
}


void KEduVocDocument::setOriginalIdentifier(const QString &id)
{
  if (m_identifiers.size() > 0) {
    m_identifiers[0] = id;
  }
}

/*
class sortByOrg : public binary_function<KEduVocExpression, KEduVocExpression, bool>
{

public:

  sortByOrg (bool _dir)
    : dir (_dir) {}

  bool operator() (const KEduVocExpression& x, const KEduVocExpression& y) const
    {
      return
        !dir
        ? (QString::compare(x.original().upper(),
                            y.original().upper() ) < 0)
        : (QString::compare(x.original().upper(),
                            y.original().upper() ) > 0);
    }

 private:
  bool          dir;
};


class sortByLessonAndOrg_alpha
  : public binary_function<KEduVocExpression, KEduVocExpression, bool>
{

public:

  sortByLessonAndOrg_alpha (bool _dir, KEduVocDocument &_doc)
    : dir (_dir), doc(_doc) {}

  bool operator() (const KEduVocExpression& x, const KEduVocExpression& y) const
    {
      if (x.lesson() != y.lesson() )
        return
          !dir
          ? (QString::compare(doc.lessonDescription(x.lesson()).upper(),
                              doc.lessonDescription(y.lesson()).upper() ) < 0)
          : (QString::compare(doc.lessonDescription(x.lesson()).upper(),
                              doc.lessonDescription(y.lesson()).upper() ) > 0);
      else
        return
          !dir
          ? (QString::compare(x.original().upper(),
                              y.original().upper() ) < 0)
          : (QString::compare(x.original().upper(),
                              y.original().upper() ) > 0);
    }

 private:
  bool          dir;
  KEduVocDocument &doc;
};


class sortByLessonAndOrg_index
  : public binary_function<KEduVocExpression, KEduVocExpression, bool>
{

public:

  sortByLessonAndOrg_index (bool _dir, KEduVocDocument &_doc)
    : dir (_dir), doc(_doc) {}

  bool operator() (const KEduVocExpression& x, const KEduVocExpression& y) const
    {
      if (x.lesson() != y.lesson() )
        return
          !dir
          ? x.lesson() < y.lesson()
          : y.lesson() < x.lesson();
      else
        return
          !dir
          ? (QString::compare(x.original().upper(),
                              y.original().upper() ) < 0)
          : (QString::compare(x.original().upper(),
                              y.original().upper() ) > 0);
    }

 private:
  bool          dir;
  KEduVocDocument &doc;
};


class sortByTrans : public binary_function<KEduVocExpression, KEduVocExpression, bool>
{

public:

  sortByTrans (int i, bool _dir)
    : index(i), dir (_dir) {}

  bool operator() (const KEduVocExpression& x, const KEduVocExpression& y) const
    {
      return
        !dir
        ? (QString::compare(x.translation(index).upper(),
                            y.translation(index).upper() ) < 0)
        : (QString::compare(x.translation(index).upper(),
                            y.translation(index).upper() ) > 0);
    }

 private:
  int  index;
  bool dir;
};
*/
/**@todo implement sorting based on lesson index and name.
  * Will be done when KVocTrain is ported to this class
*/
int sortIndex;
bool sortAscending;

bool operator< (const KEduVocExpression &e1, const KEduVocExpression &e2)
{
  if (sortAscending)
    if (sortIndex == 0)
      return ! (e1.original() > e2.original());
    else
      return ! (e1.translation(sortIndex) > e2.translation(sortIndex));
  else
    if (sortIndex == 0)
      return ! (e1.original() < e2.original());
    else
      return ! (e1.translation(sortIndex) < e2.translation(sortIndex));
}

bool KEduVocDocument::sort(int index)
{
  bool result = false;
  if (m_enableSorting && index < numIdentifiers())
  {
    if (m_sortIdentifier.count() < m_identifiers.count())
      for (int i = m_sortIdentifier.count(); i < (int) m_identifiers.count(); i++)
          m_sortIdentifier.append(false);

    sortAscending = m_sortIdentifier[index];
    sortIndex = index;
    qHeapSort(m_vocabulary);
    m_sortIdentifier[index] = !m_sortIdentifier[index];
    result = m_sortIdentifier[index];
  }
  return result;

  /*if (!sort_allowed)
    return false;

  if (index >= numLangs())
    return false;

  if (sort_lang.size() < langs.size())
    for (int i = sort_lang.size(); i < (int) langs.size(); i++)
      sort_lang.push_back(false);

  if (index == 0)
    std::sort (vocabulary.begin(), vocabulary.end(), sortByOrg(sort_lang[0]));
  else
    std::sort (vocabulary.begin(), vocabulary.end(), sortByTrans(index, sort_lang[index]));
  sort_lang[index] = !sort_lang[index];
  return sort_lang[index];*/
}


bool KEduVocDocument::sortByLessonAlpha ()
{
 /* if (!sort_allowed)
    return false;

  std::sort (vocabulary.begin(), vocabulary.end(), sortByLessonAndOrg_alpha(sort_lesson, *this ));
  sort_lesson = !sort_lesson;
  return sort_lesson;*/
}


bool KEduVocDocument::sortByLessonIndex ()
{
 /* if (!sort_allowed)
    return false;

  if (sort_lang.size() < langs.size())
    for (int i = sort_lang.size(); i < (int) langs.size(); i++)
      sort_lang.push_back(false);

  std::sort (vocabulary.begin(), vocabulary.end(), sortByLessonAndOrg_index(sort_lesson, *this ));
  sort_lesson = !sort_lesson;
  sort_lang[0] = sort_lesson;
  return sort_lesson;*/
}

bool KEduVocDocument::leitnerSystemActive()
{
	return m_activeLeitnerSystem;
}

void KEduVocDocument::setLeitnerSystemActive( bool yes )
{
	if( yes )
	{
		if (m_leitnerSystem == 0)
			createStandardLeitnerSystem(); //if nothing is loaded yet

		m_activeLeitnerSystem = true;
	}
	else if( !yes )
		m_activeLeitnerSystem = false;
}

void KEduVocDocument::createStandardLeitnerSystem()
{
	LeitnerSystem* tmpSystem = new LeitnerSystem();
	QString name = "Standard";

	tmpSystem->setSystemName( name );
	tmpSystem->insertBox( "Box 1" );
	tmpSystem->insertBox( "Box 2" );
	tmpSystem->insertBox( "Box 3" );
	tmpSystem->insertBox( "Box 4" );
	tmpSystem->insertBox( "Box 5" );

	tmpSystem->setCorrectBox( "Box 1", "Box 2" );
	tmpSystem->setWrongBox( "Box 1", "Box 1" );

	tmpSystem->setCorrectBox( "Box 2", "Box 3" );
	tmpSystem->setWrongBox( "Box 2", "Box 1" );

	tmpSystem->setCorrectBox( "Box 3", "Box 4" );
	tmpSystem->setWrongBox( "Box 3", "Box 1" );

	tmpSystem->setCorrectBox( "Box 4", "Box 5" );
	tmpSystem->setWrongBox( "Box 4", "Box 1" );

	tmpSystem->setCorrectBox( "Box 5", "Box 1" );
	tmpSystem->setWrongBox( "Box 5", "Box 1" );

	m_leitnerSystem = tmpSystem;
}

void KEduVocDocument::setLeitnerSystem( LeitnerSystem* system )
{
	m_leitnerSystem = system;

	/*KWordQuizApp* app = (KWordQuizApp*) parent();
	app->slotLeitnerSystem();*/
}

LeitnerSystem* KEduVocDocument::leitnerSystem()
{
	return m_leitnerSystem;
}


class resetAll : public unary_function<KEduVocExpression, void>
{

public:

  resetAll (int less)
    : lesson(less) {}

  void operator() (KEduVocExpression& x)
    {
       for (int i = 0; i <= x.numTranslations(); i++) {
         if (lesson == 0 || lesson == x.lesson())
         {
            x.setGrade(i, KV_NORM_GRADE, false);
            x.setGrade(i, KV_NORM_GRADE, true);
            x.setQueryCount (i, 0, true);
            x.setQueryCount (i, 0, false);
            x.setBadCount (i, 0, true);
            x.setBadCount (i, 0, false);
            QDateTime dt;
            dt.setTime_t(0);
            x.setQueryDate (i, dt, true);
            x.setQueryDate (i, dt, false);
         }
       }
    }
 private:
  int lesson;
};


class resetOne : public unary_function<KEduVocExpression, void>
{

public:

  resetOne (int idx, int less)
    : index (idx), lesson(less) {}

  void operator() (KEduVocExpression& x)
    {
       if (lesson == 0 || lesson == x.lesson())
       {
         x.setGrade(index, KV_NORM_GRADE, false);
         x.setGrade(index, KV_NORM_GRADE, true);
         x.setQueryCount (index, 0, true);
         x.setQueryCount (index, 0, false);
         x.setBadCount (index, 0, true);
         x.setBadCount (index, 0, false);
         QDateTime dt;
         dt.setTime_t(0);
         x.setQueryDate (index, dt, true);
         x.setQueryDate (index, dt, false);
       }
    }

 private:
  int index;
  int lesson;
};


void KEduVocDocument::resetEntry (int index, int lesson)
{
  if (index < 0)
    for_each (m_vocabulary.begin(), m_vocabulary.end(), resetAll(lesson) );
  else
    for_each (m_vocabulary.begin(), m_vocabulary.end(), resetOne(index, lesson) );
}


QString KEduVocDocument::lessonDescription(int idx) const
{
  if (idx == 0)
    return i18n("<no lesson>");

  if (idx <= 0 || idx > (int) m_lessonDescriptions.size() )
    return "";

  return m_lessonDescriptions[idx-1];
}


QValueList<int> KEduVocDocument::lessonsInQuery() const
{
  QValueList<int> iqvec;
  for (unsigned i = 0; i < m_lessonsInQuery.size(); i++)
    if (m_lessonsInQuery[i]) {
      iqvec.push_back(i+1);   // Offset <no lesson>
//      cout << "getliq: " << i+1 << endl;
    }
  return iqvec;
}


void KEduVocDocument::setLessonsInQuery(QValueList<int> lesson_iq)
{
  m_lessonsInQuery.clear();
  for (unsigned i = 0; i < m_lessonDescriptions.size(); i++)
    m_lessonsInQuery.push_back(false);

  for (unsigned i = 0; i < lesson_iq.size(); i++)
    if (lesson_iq[i] <= (int) m_lessonsInQuery.size() ) {
      m_lessonsInQuery[lesson_iq[i]-1] = true;    // Offset <no lesson>
//      cout << "setliq: " << lesson_iq[i] << " " << i << endl;
    }
}


QString KEduVocDocument::title() const
{
  if (m_title.isEmpty())
    return m_url.fileName();
  else
    return m_title;
}


QString KEduVocDocument::author() const
{
  return m_author;
}


QString KEduVocDocument::license() const
{
  return m_license;
}


QString KEduVocDocument::docRemark() const
{
  return m_remark;
}


QFont* KEduVocDocument::font() const
{
  return m_font;
}


void KEduVocDocument::setTitle(const QString & title)
{
  m_title = title.stripWhiteSpace();
}


void KEduVocDocument::setAuthor(const QString & s)
{
  m_author = s.stripWhiteSpace();
}


void KEduVocDocument::setLicense(const QString & s)
{
  m_license = s.stripWhiteSpace();
}


void KEduVocDocument::setDocRemark(const QString & s)
{
  m_remark = s.stripWhiteSpace();
}


void KEduVocDocument::setFont(QFont* font)
{
  delete this->m_font;
  this->m_font = font;
}


int KEduVocDocument::search(QString substr, int id, int first, int last, bool word_start)
{
  if (last >= numEntries() || last < 0)
    last = numEntries();

  if (first < 0)
    first = 0;

  if (id >= numIdentifiers() || last < first)
    return -1;

  if (id == 0) {
    for (int i = first; i < last; i++) {
      if (word_start) {
        if (entry(i)->original().find(substr, 0, false) == 0)  // case insensitive
          return i;
      }
      else {
        if (entry(i)->original().find(substr, 0, false) > -1)  // case insensitive
          return i;
      }
    }
  }
  else {
    for (int i = first; i < last; i++) {
      if (word_start) {
        if (entry(i)->translation(id).find(substr, 0, false) == 0) // case insensitive
          return i;
      }
      else {
        if (entry(i)->translation(id).find(substr, 0, false) > -1) // case insensitive
          return i;
      }
    }
  }
  return -1;
}


KEduVocDocument::FileType KEduVocDocument::detectFileType(const QString &filename)
{
   QFile f( filename );
   if (!f.open( IO_ReadOnly ))
     return csv;

   QDataStream is( &f );

   Q_INT8 c1, c2, c3, c4, c5;
   is >> c1
      >> c2
      >> c3
      >> c4
      >> c5;  // guess filetype by first x bytes

   QTextStream ts (&f);
   QString line;
   line = ts.readLine();
   line.insert (0, c5);
   line.insert (0, c4);
   line.insert (0, c3);
   line.insert (0, c2);
   line.insert (0, c1);
   f.close();

   bool stat = is.device()->status();
   if (stat != IO_Ok)
     return kvd_none;
   if (c1 == '<' && c2 == '?' && c3 == 'x' && c4 == 'm' && c5 == 'l')
     return kvtml;

   if (line == WQL_IDENT)
     return wql;

   if (line.find (VCB_SEPARATOR) >= 0)
     return vt_vcb;

   if (line == LEX_IDENT_50)
     return vt_lex;

   return csv;
}


class expRef {

public:

  expRef (KEduVocExpression *_exp, int _idx)
   {
      idx    = _idx;
      exp    = _exp;
   }

  bool operator< (const expRef& y) const
    {
      QString s1 = exp->original();
      QString s2 = y.exp->original();
      int cmp = QString::compare(s1.upper(), s2.upper());
      if (cmp != 0)
        return cmp < 0;

      for (int i = 1; i < (int) exp->numTranslations(); i++) {

        s1 = exp->translation(i);
        s2 = y.exp->translation(i);
        cmp = QString::compare(s1.upper(), s2.upper() );
        if (cmp != 0)
          return cmp < 0;
      }
      return cmp < 0;
    }

  int            idx;
  KEduVocExpression *exp;
};


int KEduVocDocument::cleanUp()
{
  int count = 0;
  KEduVocExpression *kve1, *kve2;
  vector<expRef> shadow;
  vector<int> to_delete;

  for (int i = 0; i < (int) m_vocabulary.size(); i++)
    shadow.push_back (expRef (entry(i), i));
  std::sort(shadow.begin(), shadow.end());

#ifdef CLEAN_BUG
  ofstream sso ("shadow.out");
  for (int i = shadow.size()-1; i > 0; i--) {
    kve1 = shadow[i].exp;
    sso << kve1->original() << "  ";
    for (int l = 1; l < (int) numLangs(); l++ )
      sso << kve1->translation(l)  << "  ";
    sso << endl;
  }
#endif

  int ent_no = 0;
  int ent_percent = m_vocabulary.size () / 100;
  float f_ent_percent = m_vocabulary.size () / 100.0;
  emit progressChanged(this, 0);

  for (int i = shadow.size()-1; i > 0; i--) {
    kve1 = shadow[i].exp;
    kve2 = shadow[i-1].exp;

    ent_no++;
    if (ent_percent != 0 && (ent_no % ent_percent) == 0 )
      emit progressChanged(this, (int)((ent_no / f_ent_percent) / 2.0));

    bool equal = true;
    if (kve1->original() == kve2->original() ) {
      for (int l = 1; equal && l < (int) numIdentifiers(); l++ )
        if (kve1->translation(l) != kve2->translation(l))
          equal = false;

      if (equal) {
        to_delete.push_back(shadow[i-1].idx);
        count++;
      }
    }
  }

  // removing might take very long
  ent_no = 0;
  ent_percent = to_delete.size () / 100;
  f_ent_percent = to_delete.size () / 100.0;
  emit progressChanged(this, 0);

  qHeapSort(to_delete.begin(), to_delete.end());
  //std::sort (to_delete.begin(), to_delete.end() );
  for (int i = (int) to_delete.size()-1; i >= 0; i--) {
    ent_no++;
    if (ent_percent != 0 && (ent_no % ent_percent) == 0 )
      emit progressChanged(this, (int)(50 + ent_no / f_ent_percent / 2.0));
#ifdef CLEAN_BUG
    sso << getEntry(to_delete[i])->getOriginal() << endl;
#endif
    removeEntry (to_delete[i]);
    setModified();
  }

  return count;
}
