//
// C++ Implementation:
//
// Description: KWordQuiz flashcard appearance preferences
//
//
// Author: Peter Hedlund <peter.hedlund@kdemail.net>, (C) 2004-2005
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include <QStackedWidget>
#include <QLabel>

#include <kfontrequester.h>
#include <kcolorbutton.h>
#include <klocale.h>

#include "prefcardappearance.h"

PrefCardAppearance::PrefCardAppearance(QWidget *parent) : QWidget(parent)
{
  setupUi(this);

  connect(flipButton, SIGNAL(clicked()), this, SLOT(slotFlipButtonClicked()));
  connect(kcfg_FrontFont, SIGNAL(fontSelected(const QFont&)), this, SLOT(slotFontChanged(const QFont&)));
  connect(kcfg_FrontTextColor, SIGNAL(changed(const QColor&)), this, SLOT(slotTextColorChanged(const QColor&)));
  connect(kcfg_FrontCardColor, SIGNAL(changed(const QColor&)), this, SLOT(slotCardColorChanged(const QColor&)));
  connect(kcfg_FrontFrameColor, SIGNAL(changed(const QColor&)), this, SLOT(slotFrameColorChanged(const QColor&)));
  connect(widgetStack, SIGNAL(currentChanged(int)), this, SLOT(slotCurrentChanged(int)));

  widgetStack->setCurrentWidget(frontStackPage);
}

void PrefCardAppearance::slotFlipButtonClicked( )
{
  if (widgetStack->currentWidget() == frontStackPage)
  {
    widgetStack->setCurrentWidget(backStackPage);
  }
  else
  {
    widgetStack->setCurrentWidget(frontStackPage);
  }
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

void PrefCardAppearance::slotCurrentChanged(int index)
{
  if (index == widgetStack->indexOf(backStackPage))
  {
    titleLabel->setText(i18nc("Back of the flashcard", "Back"));
    titleLabel->setPaletteBackgroundColor(kcfg_BackCardColor->color());
    textLabel->setText(i18n("Answer"));
    textLabel->setFont(kcfg_BackFont->font());
    textLabel->setPaletteForegroundColor(kcfg_BackTextColor->color());
    textLabel->setPaletteBackgroundColor(kcfg_BackCardColor->color());
    cardFrame->setPaletteBackgroundColor(kcfg_BackCardColor->color());
    cardFrame->setPaletteForegroundColor(kcfg_BackFrameColor->color());
    line->setPaletteForegroundColor(kcfg_BackFrameColor->color());
    line->setPaletteBackgroundColor(kcfg_BackCardColor->color());
    fontLabel->setBuddy(kcfg_BackFont);
    textColorLabel->setBuddy(kcfg_BackTextColor);
    frameColorLabel->setBuddy(kcfg_BackFrameColor);
    cardColorLabel->setBuddy(kcfg_BackCardColor);
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
    fontLabel->setBuddy(kcfg_FrontFont);
    textColorLabel->setBuddy(kcfg_FrontTextColor);
    frameColorLabel->setBuddy(kcfg_FrontFrameColor);
    cardColorLabel->setBuddy(kcfg_FrontCardColor);
  }
}


#include "prefcardappearance.moc"
