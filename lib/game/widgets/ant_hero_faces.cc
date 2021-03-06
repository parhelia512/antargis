#include "ant_hero_faces.h"
#include "ag_image.h"
#include "ant_player.h"
#include "ant_hero.h"
#include "ant_energy_bar.h"
#include "ag_radio.h"


AntHeroFaces::AntHeroFaces(AGWidget *pParent, const AGRect2 &pRect):
  AGWidget(pParent,pRect) {
    aggressionSelector=0;
  }

AntHeroFaces::~AntHeroFaces() throw() {
}

void AntHeroFaces::eventInitEvents() {
  AGWidget::eventInitEvents();
  for(int index=0;index<6;index++) {
    AGString widgetName="hero";
    widgetName+=AGString(index);
    AGWidget *widget=getChild(widgetName);
    widget->sigClick.connect(slot(this,&AntHeroFaces::buttonClicked));
    if(index==0)
      dynamic_cast<AGRadio*>(widget)->setChecked(true);
    /*[](AGEvent*e) {
          e->heroClicked(heroes[index]);
          }));*/
  }
}
    
bool AntHeroFaces::buttonClicked(AGEvent *e) {
  AGWidget *caller=dynamic_cast<AGWidget*>(e->getCaller());
  if(caller) {
    AGString name=caller->getName();
    int index=name.substr(4,name.length()-4).toInt();
    if((int)heroes.size()>index) {
      sigHeroClicked(heroes[index]);
    }
  }
  return true;
}

void AntHeroFaces::update(AntPlayer *pPlayer) {
  int index=0;
  for(AntHero* hero: pPlayer->getHeroes()) {

    AGString widgetName="HeroImage";
    widgetName+=AGString(index);
    AGWidget *widget=getChild(widgetName);

    AGImage *image=dynamic_cast<AGImage*>(widget);
    if(image) {
      image->setTexture(hero->getImage());
    }
    widget=getChild(AGString("HeroBar")+AGString(index));
    AntEnergyBar *bar=dynamic_cast<AntEnergyBar*>(widget);
    if(bar) {
      bar->setHero(hero);
    }
    heroes.push_back(hero);
    index++;
  }
  for(index=0;index<6;index++) {
    AGString widgetName="hero";
    widgetName+=AGString(index);
    AGWidget *widget=getChild(widgetName);
    widget->setVisible(index<pPlayer->getHeroes().size());
  }
}


