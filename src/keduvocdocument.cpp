/***************************************************************************
                        Vocabulary Document for KDE Edu
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

#include "keduvocdocument.h"

#include <kapplication.h>
#include <klocale.h>
#include <kdebug.h>
#include <kmessagebox.h>
#include <kio/netaccess.h>

#include <qfileinfo.h>

#include <algorithm>
#include <functional>
#include <vector>
using namespace std;

#include <iostream>

#include <float.h>

#include "keduvockvtmlwriter.h"
#include "keduvockvtmlreader.h"
//#include "prefs.h"

//********************************************************
//  KEduVocDocument
//********************************************************

KEduVocDocument::KEduVocDocument(QObject *parent)
{
  Init();
}


KEduVocDocument::~KEduVocDocument()
{
}


void KEduVocDocument::setVersion (const QString & vers)
{
  doc_version = vers;
}


void KEduVocDocument::getVersion(int &, int &, int &)
{
}


void KEduVocDocument::Init ()
{

//TODO setVersion (QString::fromUtf8(KVOCTRAIN_VERSION_STRING));
  lesson_descr.clear();
  type_descr.clear();
  tense_descr.clear();
  langs.clear();
  extraSizehints.clear();
  sizehints.clear();
  vocabulary.clear();
  dirty = false;
  sort_allowed = true;
  unknown_attr = false;
  unknown_elem = false;
  sort_lesson = false;
  for (int i = 0; i < (int) langs.size(); i++)
    sort_lang.push_back(false);
  setCurrentLesson (0);
  queryorg = "";
  querytrans = "";
  doc_url.setFileName(i18n("Untitled"));
  doctitle = "";
  author = "";
}


bool KEduVocDocument::open(const KURL& url, bool append)
{
  Init();
  if (!url.isEmpty())
    doc_url = url;

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

    FileType ft = detectFT(url.path());

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
        if (unknown_attr || unknown_elem ) {
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


bool KEduVocDocument::saveAs(QObject *parent, const KURL & url, FileType ft)
{
  connect( this, SIGNAL(progressChanged(KEduVocDocument*,int)), parent, SLOT(slotProgress(KEduVocDocument*,int)) );

  KURL tmp (url);

  if (ft == automatic)
  {
    if (tmp.path().right(strlen("." KVTML_EXT)) == "." KVTML_EXT)
      ft = kvtml;
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
        saved = kvtmlWriter.writeDoc(this);
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
  doc_url = tmp;
  dirty = false;
  emit docModified(false);
  return true;
}


KEduVocExpression *KEduVocDocument::getEntry(int index)
{
  if (index < 0 || index >= (int)vocabulary.size() )
    return 0;
  else
    return &vocabulary[index];
}


void KEduVocDocument::removeEntry(int index)
{
  if (index >= 0 && index < (int)vocabulary.size() )
    vocabulary.erase (vocabulary.begin() + index);
}


int KEduVocDocument::findIdent (const QString &lang) const
{
  vector<QString>::const_iterator first = langs.begin();
  int count = 0;
  while (first != langs.end()) {
    if ( *first == lang)
      return count;
    first++;
    count++;
  }
  return -1;
}


QString KEduVocDocument::getIdent (int index) const
{
  if (index >= (int)langs.size() || index < 1 )
    return "";
  else
    return langs[index];
}


void KEduVocDocument::setIdent (int idx, const QString &id)
{
  if (idx < (int)langs.size() && idx >= 1 ) {
    langs[idx] = id;
  }
}


QString KEduVocDocument::getTypeName (int index) const
{
  if (index >= (int)type_descr.size())
    return "";
  else
    return type_descr[index];
}


void KEduVocDocument::setTypeName (int idx, QString &id)
{
  if (idx >= (int)type_descr.size())
    for (int i = (int)type_descr.size(); i <= idx; i++)
      type_descr.push_back ("");

  type_descr[idx] = id;
}


QString KEduVocDocument::getTenseName (int index) const
{
  if (index >= (int)tense_descr.size())
    return "";
  else
    return tense_descr[index];
}


void KEduVocDocument::setTenseName (int idx, QString &id)
{
  if (idx >= (int)tense_descr.size())
    for (int i = (int)tense_descr.size(); i <= idx; i++)
      tense_descr.push_back ("");

  tense_descr[idx] = id;
}


QString KEduVocDocument::getUsageName (int index) const
{
  if (index >= (int)usage_descr.size())
    return "";
  else
    return usage_descr[index];
}


void KEduVocDocument::setUsageName (int idx, QString &id)
{
  if (idx >= (int)usage_descr.size())
    for (int i = (int)usage_descr.size(); i <= idx; i++)
      usage_descr.push_back ("");

  usage_descr[idx] = id;
}


void KEduVocDocument::setConjugation (int idx, const Conjugation &con)
{
  if ( idx < 0) return;

  // extend conjugation with empty elements
  if ((int)conjugations.size() <= idx )
    for (int i = conjugations.size(); i < idx+1; i++)
      conjugations.push_back (Conjugation());

  conjugations[idx] = con;
}


Conjugation KEduVocDocument::getConjugation (int idx) const
{
  if (idx >= (int)conjugations.size() || idx < 0) {
    return Conjugation();
  }
  else {
    return conjugations[idx];
  }
}


void KEduVocDocument::setArticle (int idx, const Article &art)
{
  if ( idx < 0) return;

  // extend conjugation with empty elements
  if ((int)articles.size() <= idx )
    for (int i = articles.size(); i < idx+1; i++)
      articles.push_back (Article());

  articles[idx] = art;
}


Article KEduVocDocument::getArticle (int idx) const
{
  if (idx >= (int)articles.size() || idx < 0) {
    return Article();
  }
  else {
    return articles[idx];
  }
}


int KEduVocDocument::getSizeHint (int idx) const
{
  if (idx < 0) {
    idx = -idx;
    if (idx >= (int)extraSizehints.size() )
      return 80; // make a good guess about column size
    else {
//      cout << "gsh " << idx << "  " << extraSizehints[idx] << endl;
      return extraSizehints[idx];
    }
  }
  else {
    if (idx >= (int)sizehints.size() )
      return 150; // make a good guess about column size
    else {
//      cout << "gsh " << idx << "  " << sizehints[idx] << endl;
      return sizehints[idx];
    }
  }
}


void KEduVocDocument::setSizeHint (int idx, const int width)
{
//  cout << "ssh " << idx << "  " << width << endl;
  if (idx < 0) {
    idx = -idx;
    if (idx >= (int)extraSizehints.size()) {
      for (int i = (int)extraSizehints.size(); i <= idx; i++)
        extraSizehints.push_back (80);
    }
    extraSizehints[idx] = width;

  }
  else {
    if (idx >= (int)sizehints.size()) {
      for (int i = (int)sizehints.size(); i <= idx; i++)
        sizehints.push_back (150);
    }
    sizehints[idx] = width;
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


void KEduVocDocument::removeIdent (int index)
{
  if (index < (int)langs.size() && index >= 1 ) {
    langs.erase(langs.begin() + index);
    for_each (vocabulary.begin(), vocabulary.end(), eraseTrans(index));
  }
}


QString KEduVocDocument::getOriginalIdent () const
{
  if (langs.size() > 0)
    return langs[0];
  else
    return "";
}


void KEduVocDocument::setOriginalIdent (const QString &id)
{
  if (langs.size() > 0) {
    langs[0] = id;
  }
}


class sortByOrg : public binary_function<KEduVocExpression, KEduVocExpression, bool>
{

public:

  sortByOrg (bool _dir)
    : dir (_dir) {}

  bool operator() (const KEduVocExpression& x, const KEduVocExpression& y) const
    {
      return
        !dir
        ? (QString::compare(x.getOriginal().upper(),
                            y.getOriginal().upper() ) < 0)
        : (QString::compare(x.getOriginal().upper(),
                            y.getOriginal().upper() ) > 0);
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
      if (x.getLesson() != y.getLesson() )
        return
          !dir
          ? (QString::compare(doc.getLessonDescr(x.getLesson()).upper(),
                              doc.getLessonDescr(y.getLesson()).upper() ) < 0)
          : (QString::compare(doc.getLessonDescr(x.getLesson()).upper(),
                              doc.getLessonDescr(y.getLesson()).upper() ) > 0);
      else
        return
          !dir
          ? (QString::compare(x.getOriginal().upper(),
                              y.getOriginal().upper() ) < 0)
          : (QString::compare(x.getOriginal().upper(),
                              y.getOriginal().upper() ) > 0);
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
      if (x.getLesson() != y.getLesson() )
        return
          !dir
          ? x.getLesson() < y.getLesson()
          : y.getLesson() < x.getLesson();
      else
        return
          !dir
          ? (QString::compare(x.getOriginal().upper(),
                              y.getOriginal().upper() ) < 0)
          : (QString::compare(x.getOriginal().upper(),
                              y.getOriginal().upper() ) > 0);
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
        ? (QString::compare(x.getTranslation(index).upper(),
                            y.getTranslation(index).upper() ) < 0)
        : (QString::compare(x.getTranslation(index).upper(),
                            y.getTranslation(index).upper() ) > 0);
    }

 private:
  int  index;
  bool dir;
};


bool KEduVocDocument::sort (int index)
{
  if (!sort_allowed)
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
  return sort_lang[index];
}


bool KEduVocDocument::sortByLesson_alpha ()
{
  if (!sort_allowed)
    return false;

  std::sort (vocabulary.begin(), vocabulary.end(), sortByLessonAndOrg_alpha(sort_lesson, *this ));
  sort_lesson = !sort_lesson;
  return sort_lesson;
}


bool KEduVocDocument::sortByLesson_index ()
{
  if (!sort_allowed)
    return false;

  if (sort_lang.size() < langs.size())
    for (int i = sort_lang.size(); i < (int) langs.size(); i++)
      sort_lang.push_back(false);

  std::sort (vocabulary.begin(), vocabulary.end(), sortByLessonAndOrg_index(sort_lesson, *this ));
  sort_lesson = !sort_lesson;
  sort_lang[0] = sort_lesson;
  return sort_lesson;
}


class resetAll : public unary_function<KEduVocExpression, void>
{

public:

  resetAll (int less)
    : lesson(less) {}

  void operator() (KEduVocExpression& x)
    {
       for (int i = 0; i <= x.numTranslations(); i++) {
         if (lesson == 0 || lesson == x.getLesson() ) {
            x.setGrade(i, KV_NORM_GRADE, false);
            x.setGrade(i, KV_NORM_GRADE, true);
            x.setQueryCount (i, 0, true);
            x.setQueryCount (i, 0, false);
            x.setBadCount (i, 0, true);
            x.setBadCount (i, 0, false);
            x.setQueryDate (i, 0, true);
            x.setQueryDate (i, 0, false);
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
       if (lesson == 0 || lesson == x.getLesson() ) {
         x.setGrade(index, KV_NORM_GRADE, false);
         x.setGrade(index, KV_NORM_GRADE, true);
         x.setQueryCount (index, 0, true);
         x.setQueryCount (index, 0, false);
         x.setBadCount (index, 0, true);
         x.setBadCount (index, 0, false);
         x.setQueryDate (index, 0, true);
         x.setQueryDate (index, 0, false);
       }
    }

 private:
  int index;
  int lesson;
};


void KEduVocDocument::resetEntry (int index, int lesson)
{
  if (index < 0)
    for_each (vocabulary.begin(), vocabulary.end(), resetAll(lesson) );
  else
    for_each (vocabulary.begin(), vocabulary.end(), resetOne(index, lesson) );
}


QString KEduVocDocument::getLessonDescr(int idx) const
{
  if (idx == 0)
    return i18n("<no lesson>");

  if (idx <= 0 || idx > (int) lesson_descr.size() )
    return "";

  return lesson_descr[idx-1];
}


vector<int> KEduVocDocument::getLessonsInQuery() const
{
  vector<int> iqvec;
  for (unsigned i = 0; i < lessons_in_query.size(); i++)
    if (lessons_in_query[i]) {
      iqvec.push_back(i+1);   // Offset <no lesson>
//      cout << "getliq: " << i+1 << endl;
    }
  return iqvec;
}


void KEduVocDocument::setLessonsInQuery(vector<int> lesson_iq)
{
  lessons_in_query.clear();
  for (unsigned i = 0; i < lesson_descr.size(); i++)
    lessons_in_query.push_back(false);

  for (unsigned i = 0; i < lesson_iq.size(); i++)
    if (lesson_iq[i] <= (int) lessons_in_query.size() ) {
      lessons_in_query[lesson_iq[i]-1] = true;    // Offset <no lesson>
//      cout << "setliq: " << lesson_iq[i] << " " << i << endl;
    }
}


QString KEduVocDocument::getTitle() const
{
  if (doctitle.isEmpty())
    return doc_url.fileName();
  else
    return doctitle;
}


QString KEduVocDocument::getAuthor() const
{
  return author;
}


QString KEduVocDocument::getLicense() const
{
  return license;
}


QString KEduVocDocument::getDocRemark() const
{
  return doc_remark;
}


void KEduVocDocument::setTitle(const QString & title)
{
  doctitle = title.stripWhiteSpace();
}


void KEduVocDocument::setAuthor(const QString & s)
{
  author = s.stripWhiteSpace();
}


void KEduVocDocument::setLicense(const QString & s)
{
  license = s.stripWhiteSpace();
}


void KEduVocDocument::setDocRemark(const QString & s)
{
  doc_remark = s.stripWhiteSpace();
}


int KEduVocDocument::search(QString substr, int id,
                         int first, int last,
                         bool word_start,
                         bool)
{
   if (last >= numEntries()
       || last < 0 )
     last = numEntries();

   if (first < 0)
     first = 0;

   if (id >= numLangs()
      || last < first
      )
     return -1;

   if (id == 0) {
     for (int i = first; i < last; i++) {
       if (word_start) {
         if (getEntry(i)->getOriginal().find (substr, 0, false) == 0)  // case insensitive
           return i;
       }
       else {
         if (getEntry(i)->getOriginal().find (substr, 0, false) > -1)  // case insensitive
           return i;
       }
     }
   }
   else {
     for (int i = first; i < last; i++) {
       if (word_start) {
         if (getEntry(i)->getTranslation(id).find (substr, 0, false) == 0) // case insensitive
           return i;
       }
       else {
         if (getEntry(i)->getTranslation(id).find (substr, 0, false) > -1) // case insensitive
           return i;
       }
     }
   }
   return -1;
}

#define _OFFSET     0x40
#define _BITMASK    0x3F
#define _BITUSED    6

QString KEduVocDocument::compressDate(unsigned long l) const
{
   if (l == 0)
     return "";

   QString res;
   if (l <= KVD_ZERO_TIME)
     l = 1;
   else
     l -= KVD_ZERO_TIME;
   while (l != 0) {
     char c = _OFFSET + (l & _BITMASK);
     res.insert (0, c);
     l >>= _BITUSED;
   }
   return res;
}


unsigned long KEduVocDocument::decompressDate(QString s) const
{
   if (s.isEmpty())
     return 0;

   long res = 0;
   unsigned incr = 0;
   for (int i = s.length()-1; i >= 0; i--) {
     char c = s.local8Bit()[i];
     res += ((c - _OFFSET) & _BITMASK) << incr ;
     incr += _BITUSED;
   }
   return res > 48 ? res+KVD_ZERO_TIME : 0;  // early bug with "0"
}


KEduVocDocument::FileType KEduVocDocument::detectFT(const QString &filename)
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
      QString s1 = exp->getOriginal();
      QString s2 = y.exp->getOriginal();
      int cmp = QString::compare(s1.upper(), s2.upper());
      if (cmp != 0)
        return cmp < 0;

      for (int i = 1; i < (int) exp->numTranslations(); i++) {

        s1 = exp->getTranslation(i);
        s2 = y.exp->getTranslation(i);
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

  for (int i = 0; i < (int) vocabulary.size(); i++)
    shadow.push_back (expRef (getEntry(i), i));
  std::sort(shadow.begin(), shadow.end());

#ifdef CLEAN_BUG
  ofstream sso ("shadow.out");
  for (int i = shadow.size()-1; i > 0; i--) {
    kve1 = shadow[i].exp;
    sso << kve1->getOriginal() << "  ";
    for (int l = 1; l < (int) numLangs(); l++ )
      sso << kve1->getTranslation(l)  << "  ";
    sso << endl;
  }
#endif

  int ent_no = 0;
  int ent_percent = vocabulary.size () / 100;
  float f_ent_percent = vocabulary.size () / 100.0;
  emit progressChanged(this, 0);

  for (int i = shadow.size()-1; i > 0; i--) {
    kve1 = shadow[i].exp;
    kve2 = shadow[i-1].exp;

    ent_no++;
    if (ent_percent != 0 && (ent_no % ent_percent) == 0 )
      emit progressChanged(this, (int)((ent_no / f_ent_percent) / 2.0));

    bool equal = true;
    if (kve1->getOriginal() == kve2->getOriginal() ) {
      for (int l = 1; equal && l < (int) numLangs(); l++ )
        if (kve1->getTranslation(l) != kve2->getTranslation(l))
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

  std::sort (to_delete.begin(), to_delete.end() );
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
