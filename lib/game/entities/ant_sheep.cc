#include <ant_sheep.h>
#include <map.h>
#include <ant_models.h>
#include <ag_mixer.h>
#include <anim_mesh.h>

AntSheep::AntSheep(AntMap* pMap): AntAnimal(pMap)
{
}

AntSheep::~AntSheep() throw()
{

}

void AntSheep::init()
{
  AntAnimal::init();
  setMesh("sheep","");
  setSpeed(0.4);
}

void AntSheep::moveAround() {
  if (rand()%10<5) {
    newMoveJob(0,getTargetPos(),0);
    setMeshState("go");
  } else {
    newRestJob(3);
    setMeshState("eat");
    playSound("sheep");
  }
}

AGVector2 AntSheep::getTargetPos()
{
  AGVector2 p=getPos2D();
  int tries=10;
  // # assure that sheep doesn't walk into water
  while (tries>0) {
    float dx=rand()%100;
    float dy=rand()%100;
    AGVector2 d=AGVector2(dx,dy).normalized()*2;
    AGVector2 t=p+d;
    t=getMap()->truncPos(t);
    tries-=1;
    if (getMap()->getHeight(t.getX(),t.getY())>0) {
      return t;
    }
  }
  return p;
}

AntSheep* AntSheep::createOne()
{
  return new AntSheep(getMap());
}

void AntSheep::setMeshState(const AGString &name)
{
  setMesh("sheep",name);
}

void AntSheep::die()
{
  AntAnimal::die();
  setMesh("sheep","rip");
  newRestJob(1);
  setProvide("sheep",false);
}

