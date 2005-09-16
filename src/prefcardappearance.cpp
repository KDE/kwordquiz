//
// C++ Implementation:
//
// Description: KWordQuiz flashcard appearance preferences
//
//
// Author: Peter Hedlund <peter.hedlund@kdemail.net>, (C) 2004
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include <qwidgetstack.h>
#include <qlabel.h>

#include <kfontrequester.h>
#include <kcolorbutton.h>
#include <klocale.h>

#include "prefcardappearance.h"

PrefCardAppearance::PrefCardAppearance(QWidget *parent, const char *name)
 : PrefCardAppearanceBase(parent, name)
{
  widgetStack->raiseWidget(frontStackPage);
}


PrefCardAppearance::~PrefCardAppearance()
{
}

void PrefCardAppearance::slotFlipButtonClicked( )
{
  if (widgetStack->visibleWidget() == frontStackPage)
    widgetStack->raiseWidget(backStackPage);
  else
    widgetStack->raiseWidget(frontStackPage);
}

void PrefCardAppearance::slotFontChanged(const QFont & font)
{
  textLabel->setFont(font);
}

void PrefCardAppearance::slotTextColorChanged( const QColor & color)
{
  textLabel->setPaletteForegroundColor(color);
}

void PrefCardAppearance::slotCardColorChanged( const QColor & color)
{
  cardFrame->setPaletteBackgroundColor(color);
  titleLabel->setPaletteBackgroundColor(color);
  textLabel->setPaletteBackgroundColor(color);
  line->setPaletteBackgroundColor(color);
}

void PrefCardAppearance::slotFrameColorChanged( const QColor & color)
{
  cardFrame->setPaletteForegroundColor(color);
  line->setPaletteForegroundColor(color);
}

void PrefCardAppearance::slotAboutToShowWidget(QWidget * widget)
{
  if (widget == backStackPage)
  {
    titleLabel->setText(i18n("Back of the flashcard", "Back"));
    titleLabel->setPaletteBackgroundColor(kcfg_BackCardColor->color());
    textLabel->setText(i18n("Answer"));
    textLabel->setFont(kcfg_BackFont->font());
    textLabel->setPaletteForegroundColor(kcfg_BackTextColor->color());
    textLabel->setPaletteBackgroundColor(kcfg_BackCardColor->color());
    cardFrame->setPaletteBackgroundColor(kcfg_BackCardColor->color());
    cardFrame->setPaletteForegroundColor(kcfg_BackFrameColor->color());
    line->setPaletteForegroundColor(kcfg_BackFrameColor->color());
    line->setPaletteBackgroundColor(kcfg_BackCardColor->color());
  }
  else
  {
    titleLabel->setText(i18n("Front"));
    titleLabel->setPaletteBackgroundColor(kcfg_FrontCardColor->color());
    textLabel->setText(i18n("Question"));
    textLabel->setFont(kcfg_FrontFont->font());
    textLabel->setPaletteForegroundColor(kcfg_FrontTextColor->color());
    textLabel->setPaletteBackgroundColor(kcfg_FrontCardColor->color());
    cardFrame->setPaletteBackgroundColor(kcfg_FrontCardColor->color());
    cardFrame->setPaletteForegroundColor(kcfg_FrontFrameColor->color());
    line->setPaletteForegroundColor(kcfg_FrontFrameColor->color());
    line->setPaletteBackgroundColor(kcfg_FrontCardColor->color());
  }
}


#include "prefcardappearance.moc"
