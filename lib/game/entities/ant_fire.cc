#include "ant_fire.h"
#include "map.h"
#include "ant_particle.h"
#include "ant_models.h"

AntFire::AntFire(AntMap *pMap):AntEntity(pMap) {
  CTRACE;
}

void AntFire::init() {
  CTRACE;
  AntEntity::init();
  AGVector3 basePoint(0,0,0);
  setMesh("fire","on");
  enabled=true;
}

AntFire::~AntFire() throw(){
}

void AntFire::eventNoJob() {
  if(enabled) {
    newRestJob(30);
  }  else {
    getMap()->removeEntity(this);
  }
}

void AntFire::disable() {
  CTRACE;
  enabled=false;
  setMesh("fire","off");
}
