/***************************************************************************
                          prefcardappearance.cpp
                             -------------------

    copyright            : (C) 2004-2007 by Peter Hedlund
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

#include "prefcardappearance.h"


#include <kfontrequester.h>
#include <kcolorbutton.h>
#include <klocale.h>

PrefCardAppearance::PrefCardAppearance(QWidget *parent) : QWidget(parent)
{
  setupUi(this);

  connect(flipButton, SIGNAL(clicked()), this, SLOT(slotFlipButtonClicked()));
  connect(kcfg_FrontFont, SIGNAL(fontSelected(const QFont&)), this, SLOT(slotFontChanged(const QFont&)));
  connect(kcfg_FrontTextColor, SIGNAL(changed(const QColor&)), this, SLOT(slotTextColorChanged(const QColor&)));
  connect(kcfg_FrontCardColor, SIGNAL(changed(const QColor&)), this, SLOT(slotCardColorChanged(const QColor&)));
  connect(kcfg_FrontFrameColor, SIGNAL(changed(const QColor&)), this, SLOT(slotFrameColorChanged(const QColor&)));
  connect(kcfg_BackFont, SIGNAL(fontSelected(const QFont&)), this, SLOT(slotFontChanged(const QFont&)));
  connect(kcfg_BackTextColor, SIGNAL(changed(const QColor&)), this, SLOT(slotTextColorChanged(const QColor&)));
  connect(kcfg_BackCardColor, SIGNAL(changed(const QColor&)), this, SLOT(slotCardColorChanged(const QColor&)));
  connect(kcfg_BackFrameColor, SIGNAL(changed(const QColor&)), this, SLOT(slotFrameColorChanged(const QColor&)));
  connect(widgetStack, SIGNAL(currentChanged(int)), this, SLOT(slotCurrentChanged(int)));

  widgetStack->setCurrentWidget(frontStackPage);
  // initialize the buddies, etc.
  slotCurrentChanged(widgetStack->indexOf(frontStackPage));
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
  QPalette pal = textLabel->palette();
  pal.setColor(textLabel->foregroundRole(), color);
  textLabel->setPalette(pal);
}

void PrefCardAppearance::slotCardColorChanged( const QColor & color)
{
  QPalette pal = cardFrame->palette();
  pal.setColor(cardFrame->backgroundRole(), color);
  cardFrame->setPalette(pal);

  pal = titleLabel->palette();
  pal.setColor(titleLabel->backgroundRole(), color);
  titleLabel->setPalette(pal);

  pal = textLabel->palette();
  pal.setColor(textLabel->backgroundRole(), color);
  textLabel->setPalette(pal);

  pal = line->palette();
  pal.setColor(line->backgroundRole(), color);
  line->setPalette(pal);
}

void PrefCardAppearance::slotFrameColorChanged( const QColor & color)
{
  QPalette pal = cardFrame->palette();
  pal.setColor(cardFrame->foregroundRole(), color);
  cardFrame->setPalette(pal);

  pal = line->palette();
  pal.setColor(line->foregroundRole(), color);
  line->setPalette(pal);
}

void PrefCardAppearance::slotCurrentChanged(int index)
{
  if (index == widgetStack->indexOf(backStackPage))
  {
    titleLabel->setText(i18nc("Back of the flashcard", "Back"));
    QPalette pal = titleLabel->palette();
    pal.setColor(titleLabel->backgroundRole(), kcfg_BackCardColor->color());
    titleLabel->setPalette(pal);

    textLabel->setText(i18n("Answer"));
    textLabel->setFont(kcfg_BackFont->font());

    pal = textLabel->palette();
    pal.setColor(textLabel->foregroundRole(), kcfg_BackTextColor->color());
    pal.setColor(textLabel->backgroundRole(), kcfg_BackCardColor->color());
    textLabel->setPalette(pal);

    pal = cardFrame->palette();
    pal.setColor(cardFrame->foregroundRole(), kcfg_BackFrameColor->color());
    pal.setColor(cardFrame->backgroundRole(), kcfg_BackCardColor->color());
    cardFrame->setPalette(pal);

    pal = line->palette();
    pal.setColor(line->foregroundRole(), kcfg_BackFrameColor->color());
    pal.setColor(line->backgroundRole(), kcfg_BackCardColor->color());
    line->setPalette(pal);

    fontLabel->setBuddy(kcfg_BackFont);
    textColorLabel->setBuddy(kcfg_BackTextColor);
    frameColorLabel->setBuddy(kcfg_BackFrameColor);
    cardColorLabel->setBuddy(kcfg_BackCardColor);
  }
  else
  {
    titleLabel->setText(i18n("Front"));
    QPalette pal = titleLabel->palette();
    pal.setColor(titleLabel->backgroundRole(), kcfg_FrontCardColor->color());
    titleLabel->setPalette(pal);

    textLabel->setText(i18n("Question"));
    textLabel->setFont(kcfg_FrontFont->font());

    pal = textLabel->palette();
    pal.setColor(textLabel->foregroundRole(), kcfg_FrontTextColor->color());
    pal.setColor(textLabel->backgroundRole(), kcfg_FrontCardColor->color());
    textLabel->setPalette(pal);

    pal = cardFrame->palette();
    pal.setColor(cardFrame->foregroundRole(), kcfg_FrontFrameColor->color());
    pal.setColor(cardFrame->backgroundRole(), kcfg_FrontCardColor->color());
    cardFrame->setPalette(pal);

    pal = line->palette();
    pal.setColor(line->foregroundRole(), kcfg_FrontFrameColor->color());
    pal.setColor(line->backgroundRole(), kcfg_FrontCardColor->color());
    line->setPalette(pal);

    fontLabel->setBuddy(kcfg_FrontFont);
    textColorLabel->setBuddy(kcfg_FrontTextColor);
    frameColorLabel->setBuddy(kcfg_FrontFrameColor);
    cardColorLabel->setBuddy(kcfg_FrontCardColor);
  }
}

#include "prefcardappearance.moc"
