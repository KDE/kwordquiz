/***************************************************************************
                          kwordquizdoc.cpp  -  description
                             -------------------
    begin                : Wed Jul 24 20:12:30 PDT 2002
    copyright            : (C) 2002 by Peter Hedlund
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

// include files for Qt
#include <qdir.h>
#include <qwidget.h>
#include <qfile.h>
#include <qtextstream.h>
#include <qtable.h>
#include <qtabwidget.h>

// include files for KDE
#include <klocale.h>
#include <kmessagebox.h>
#include <kio/job.h>
#include <kio/netaccess.h>
#include <kstandarddirs.h> //locate
#include <keduvocdata.h>
#include <kdirwatch.h>

// application specific includes
#include "kwordquizdoc.h"
#include "kwordquiz.h"
#include "kwordquizview.h"
#include "kvtmlwriter.h"
#include "version.h"

QList<KWordQuizView> *KWordQuizDoc::pViewList = 0L;
//KWordQuizView *KWordQuizDoc::m_view;

KWordQuizDoc::KWordQuizDoc(QWidget *parent, const char *name) : QObject(parent, name)
{
  connect(KDirWatch::self(), SIGNAL(dirty(const QString& )), this, SLOT(slotModifiedOnDisk(const QString& )));
  /* if(!pViewList)
   {
     pViewList = new QList<KWordQuizView>();
   }

   pViewList->setAutoDelete(true);*/
}

KWordQuizDoc::~KWordQuizDoc()
{}

void KWordQuizDoc::addView(KWordQuizView *view)
{
  //pViewList->append(view);
  m_view = view;
}

void KWordQuizDoc::removeView(KWordQuizView *view)
{
  // m_view = Null;
  // pViewList->remove(view);
}
void KWordQuizDoc::setURL(const KURL &url)
{
  doc_url=url;
}

const KURL& KWordQuizDoc::URL() const
{
  return doc_url;
}

void KWordQuizDoc::slotUpdateAllViews(KWordQuizView *sender)
{
  //m_view->repaint();
  /*KWordQuizView *w;
  if(pViewList)
  {
    for(w=pViewList->first(); w!=0; w=pViewList->next())
    {
      if(w!=sender)
        w->repaint();
    }
  }*/

}

void KWordQuizDoc::setModified(bool _m)
{
  modified=_m;
  if (modified)
  {
    //m_view-> tabMain->setTabIconSet(m_view-> tabEdit, QIconSet(QPixmap(locate("data", "kwordquiz/pics/tabeditsave.png"))));
  }
  else
  {
    //m_view->tabMain->setTabIconSet(m_view-> tabEdit, QIconSet(QPixmap(locate("data", "kwordquiz/pics/tabedit.png"))));
  }
}

bool KWordQuizDoc::saveModified()
{
  bool completed=true;

  if(modified)
  {
    KWordQuizApp *win=(KWordQuizApp *) parent();
    int want_save = KMessageBox::warningYesNoCancel(win,
                    i18n("The current file has been modified.\n"
                         "Do you want to save it?"),
                    i18n("Warning"));
    switch(want_save)
    {
    case KMessageBox::Yes:
      if (doc_url.fileName() == i18n("Untitled"))
      {
        completed = win->saveAsFileName();
      }
      else
      {
        completed = saveDocument(URL());
      };

      break;

    case KMessageBox::No:
      setModified(false);
      completed=true;
      break;

    case KMessageBox::Cancel:
      completed=false;
      break;

    default:
      completed=false;
      break;
    }
  }

  return completed;
}

void KWordQuizDoc::closeDocument()
{
  deleteContents();
}

bool KWordQuizDoc::newDocument()
{
  modified=false;
  doc_url.setFileName(i18n("Untitled"));
  return true;
}

bool KWordQuizDoc::openDocument(const KURL& url, const char *format /*=0*/)
{
  QString tmpfile;
  if (KIO::NetAccess::download( url, tmpfile, 0 ))
  {
    QFile file(tmpfile);
    if (!file.open(IO_ReadOnly))
    {
      KMessageBox::error(0, i18n("<qt>Cannot open file<br><b>%1</b></qt>")
                         .arg(url.path()));
      //if (filename == fname)
      // filename = "";
      return false;
    }

    doc_url = url;
    QTable* g = m_view;
    g->setUpdatesEnabled(false);
    int i = 0;
    if (url.path().right(6) == ".kvtml")
    {
      KEduVocData * kvtmldoc = new KEduVocData;
      KEduVocDataItemList dataList = kvtmldoc->parse(url.path()); // KEduVocData::parse(url.path());
      if ((uint) kvtmldoc->numRows() > dataList.count())
        g->setNumRows(kvtmldoc->numRows());
      else
        g->setNumRows(dataList.count());
      g->horizontalHeader()->setLabel(0, kvtmldoc->language(0));
      g->horizontalHeader()->setLabel(1, kvtmldoc->language(1));
      g->setColumnWidth(0, kvtmldoc->colWidth(0));
      g->setColumnWidth(1, kvtmldoc->colWidth(1));
      QString s;
      for(KEduVocDataItemList::Iterator dataIt = dataList.begin(); dataIt != dataList.end(); dataIt++)
      {
        s = (*dataIt).originalText();
        if (!s.isEmpty())
          g->setText(i, 0, s); //calling setText only when there actually is text helps with sorting
        s = (*dataIt).translatedText();
        if (!s.isEmpty())
          g->setText(i, 1, s);
        i++;
      }
    }
    else
    {
      QTextStream* ts = new QTextStream(&file);
      m_view->fromStream(ts, QTextStream::Latin1);
    }
    file.close();
    KIO::NetAccess::removeTempFile( tmpfile );

    //Apply word wrap to cell with text
    QTableItem* itm;
    for (int r = 0; r <= g->numRows() -1; ++r)
    {
      itm = g->item(r, 0);
      if (itm != 0)
        itm->setWordWrap(true);
      itm = g->item(r, 1);
      if (itm != 0)
        itm->setWordWrap(true);
    }

    g->setUpdatesEnabled(true);
    g->repaintContents();
  }
  modified=false;
  return true;
}

bool KWordQuizDoc::saveDocument(const KURL& url, const char *format /*=0*/)
{

  QFile file(url.path());
  if (!file.open(IO_WriteOnly))
  {
    KMessageBox::error(0, i18n("<qt>Cannot write to file<br><b>%1</b></qt>")
                       .arg(url.path()));
    //if (filename == fname)
    // filename = "";
    return false;
  }

  QTable* g = m_view;
  QString s = "";
  int w = 0;

  if (url.path().right(6) == ".kvtml")
  {
    file.close();
    KVTMLWriter writer(&file);
    writer.addHeader(QString("kwordquiz %1").arg(KWQ_VERSION), 2, g->numRows(), url.fileName());
    writer.addFirstItem(g->horizontalHeader()->label(0), g->columnWidth(0), g->text(w, 0), g->horizontalHeader()->label(1), g->columnWidth(1), g->text(w, 1));
    w++;
    int r = g->numRows() - 1;
    while (w < r)
    {
      writer.addItem(g->text(w, 0), g->text(w, 1));
      w++;
    }
  }
  else
  {
    QTextStream* ts = new QTextStream(&file);
    m_view->toStream(ts, QTextStream::Latin1);
    /*
    QTextStream ts(&file);
    //ts.setEncoding(QTextStream::UnicodeUTF8);
    // \r\n is for  compatibility with the Windows version
    ts << "WordQuiz\r\n";
    ts << "5.9.0\r\n\r\n";

    QFont f = g -> font();

    ts << "[Font Info]\r\n";
    ts << "FontName1=\"" + f.family() + "\"\r\n";
    ts << "FontSize1=" + s.setNum(f.pointSize()) + "\r\n";
    if (f.bold())
    {
      ts << "FontBold1=1\r\n";
    }
    else
    {
      ts << "FontBold1=0\r\n";
    }
    if (f.italic())
    {
      ts << "FontItalic1=1\r\n";
    }
    else
    {
      ts << "FontItalic1=0\r\n";
    }
    ts << "FontColor1=0\r\n";
    ts << "CharSet1=0\r\n";
    ts << "Layout1=0\r\n";
    ts << "FontName2=\"" + f.family() + "\"\r\n";
    ts << "FontSize2=" + s.setNum(f.pointSize()) + "\r\n";
    if (f.bold())
    {
      ts << "FontBold2=1\r\n";
    }
    else
    {
      ts << "FontBold2=0\r\n";
    }
    if (f.italic())
    {
      ts << "FontItalic2=1\r\n";
    }
    else
    {
      ts << "FontItalic2=0\r\n";
    }
    ts << "FontColor2=0\r\n";
    ts << "CharSet2=0\r\n";
    ts << "Layout2=0\r\n\r\n";

    ts << "[Character Info]\r\n";
    ts << "Characters1=abcdefghi\r\n";
    ts << "Characters2=jklmnopqr\r\n\r\n";

    ts << "[Grid Info]\r\n";
    ts << "ColWidth0=" + s.setNum(g->verticalHeader()->sectionSize(0)) + "\r\n";
    ts << "ColWidth1=" + s.setNum(g->columnWidth(0)) + "\r\n";
    ts << "ColWidth2=" + s.setNum(g->columnWidth(1)) + "\r\n";
    ts << "RowCount=" + s.setNum(g->numRows()) + "\r\n";

    // Selection
    if (g->numSelections() > 0)
    {
      QTableSelection qts = g->selection(0);
      ts << "SelLeft=" + s.setNum(qts.leftCol() + 1) + "\r\n";
      ts << "SelTop=" + s.setNum(qts.topRow() + 1) + "\r\n";
      ts << "SelRight=" + s.setNum(qts.rightCol() +1) + "\r\n";
      ts << "SelBottom=" + s.setNum(qts.bottomRow() +1) + "\r\n\r\n";
    }
    else
    {
      ts << "SelLeft=" + s.setNum(g->currentColumn() + 1) + "\r\n";
      ts << "SelTop=" + s.setNum(g->currentRow() + 1) + "\r\n";
      ts << "SelRight=" + s.setNum(g->currentColumn() +1) + "\r\n";
      ts << "SelBottom=" + s.setNum(g->currentRow() +1) + "\r\n\r\n";
    }

    ts << "[Vocabulary]\r\n";

    ts << g->horizontalHeader()->label(0)  + "   [0000000300]\r\n"; // + s.setNum(g->horizontalHeader()->sectionSize(2) * 15) + "]\r\n";
    ts << g->horizontalHeader()->label(1)  + "\r\n";
    int i = 0;
    int r = g->numRows() - 1;
    while (i < r)
    {
      ts << g->text(i,0)  + QString( "   [%1]\r\n" ).arg( g->rowHeight(i) * 15, 10, 10 );
      ts << g->text(i,1) + "\r\n";
      i++;
    }*/
  }
  doc_url = url;
  setModified(false);
  return true;
}

void KWordQuizDoc::deleteContents()
{
  /////////////////////////////////////////////////
  // TODO: Add implementation to delete the document contents
  /////////////////////////////////////////////////

}

void KWordQuizDoc::slotModifiedOnDisk( const QString & path)
{
  /*@todo this code doesn't work very well. Have to look in more detail on how Kate does it.
  if (doc_url.path() == path)
  {
    QString str = i18n("The file %1 was changed (modified) on disc by another program!\n\n").arg(doc_url.fileName());    
    int i = KMessageBox::warningYesNoCancel(0, str + i18n("Do you want to reload the modified file? Data loss may occur."));
    if ( i == KMessageBox::Yes)
      openDocument(doc_url);
  }
  */
}
