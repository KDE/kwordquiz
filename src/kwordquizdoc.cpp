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

// include files for KDE
#include <klocale.h>
#include <kmessagebox.h>
#include <kio/netaccess.h>
#include "keduvocdata.h"

// application specific includes
#include "kwordquizdoc.h"
#include "kwordquiz.h"
#include "kvtmlwriter.h"
#include "version.h"
#include "paukerreader.h"

//QList<KWordQuizView> *KWordQuizDoc::pViewList = 0L;
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
    
    if (url.path().right(7) == ".xml.gz")
    {
      doc_url.setFileName(i18n("Untitled"));//To invoke Save As..., since we don't have save support for this format
      PaukerData * paukerDoc = new PaukerData;
      PaukerDataItemList dataList = paukerDoc->parse(url.path());
      g->setNumRows(dataList.count());
      QString s;
      for(PaukerDataItemList::Iterator dataIt = dataList.begin(); dataIt != dataList.end(); dataIt++)
      {
        s = (*dataIt).frontSide();
        if (!s.isEmpty())
          g->setText(i, 0, s); //calling setText only when there actually is text helps with sorting
        s = (*dataIt).backSide();
        if (!s.isEmpty())
          g->setText(i, 1, s);
        i++;
      }
    }
        
    if (url.path().right(6) == ".kvtml")
    {
      KEduVocData * kvtmldoc = new KEduVocData;
      KEduVocDataItemList dataList = kvtmldoc->parse(url.path());
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
    
    if (url.path().right(4) == ".wql")
    {
      QTextStream* ts = new QTextStream(&file);
      m_view->fromStream(ts, QTextStream::Latin1);
    }
    
    if (url.path().right(4) == ".csv")
    {
      QTextStream ts(&file);
      ts.setEncoding(QTextStream::UnicodeUTF8);      
      
      QString f = ts.read();
      QStringList fl = QStringList::split('\n', f, true);
      g->setNumRows(fl.count() - 1);

      QStringList sl = QStringList::split(",", fl[0], true);
      if (!sl[0].isEmpty())
        g->horizontalHeader()->setLabel(0, sl[0]);
      if (!sl[1].isEmpty())
        g->horizontalHeader()->setLabel(1, sl[1]);
            
      for(int i = 1; i < fl.count(); i++)
      {
        QStringList sl = QStringList::split(",", fl[i], true);
        if (!sl[0].isEmpty())
          g->setText(i - 1, 0, sl[0]);
        if (!sl[1].isEmpty())
          g->setText(i - 1, 1, sl[1]);
      }      
      
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
  
  if (url.path().right(4) == ".wql")
  {
    QTextStream* ts = new QTextStream(&file);
    m_view->toStream(ts, QTextStream::Latin1);
  }
  
  if (url.path().right(4) == ".csv")
  {
    QTextStream ts(&file);
    ts.setEncoding(QTextStream::UnicodeUTF8);
    ts << g->horizontalHeader()->label(0)  + "," + g->horizontalHeader()->label(1)  + "\n"; 

    int i = 0;
    int r = g->numRows();
    while (i < r)
    {
      ts << g->text(i, 0)  + "," + g->text(i, 1) + "\n";
      i++;
    }    
  }      
  
  doc_url = url;
  setModified(false);
  return true;
}

void KWordQuizDoc::deleteContents()
{
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

#include "kwordquizdoc.moc"
