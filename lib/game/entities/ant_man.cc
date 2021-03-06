#include "ant_man.h"
#include "map.h"
#include "ant_house.h"
#include "ant_models.h"
#include "anim_mesh.h"
#include "ant_boss.h"
#include "ant_hero.h"
#include "ag_rand.h"

AntMan::AntMan(AntMap* pMap): AntPerson(pMap)
{
  boss=0;
  dead=false;
}

AntMan::~AntMan() throw()
{

}

AntMap* AntMan::getMap()
{
  return AntEntity::getMap();
}
AGVector2 AntMan::getPos2D() const
{
  return AntEntity::getPos2D();
}
Resource& AntMan::getResources()
{
  return resource;
}

AGString genName(AntMap *map) {
  std::vector<AGString> nstart;

  nstart.push_back("Ban");
  nstart.push_back("Gor");
  nstart.push_back("Bal");
  nstart.push_back("Da");
  nstart.push_back("Arg");
  nstart.push_back("Ol");
  nstart.push_back("Gar");
  nstart.push_back("Hek");
  nstart.push_back("Gav");
  std::vector<AGString> nend;

  nend.push_back("dor");
  nend.push_back("in");
  nend.push_back("and");
  nend.push_back("or");
  nend.push_back("tor");
  nend.push_back("ain");
  nend.push_back("yn");

  AGString name;
  int trials=0;
  do {
    name= nstart[rand()%nstart.size()]+
      nend[rand()%nend.size()];
    trials++;
    if(trials>100)
      throw std::runtime_error("Could not generate name for man, because all possible names are used already!");
  } while(map->getByName(name));
  return name;
}


void AntMan::init()
{
  AntEntity::init();
  setHunger(0.006);
  setProvide("man",true);

  setName(genName(getMap()));

  age=rand()%20+15;
  checkResources();
  setMeshState("walk");
}



void AntMan::eventNoJob()
{
  AntEntity::eventNoJob();

  setVisible(true);

  if (getEnergy()==0 || dead) {
    simDeath();
    return;
  }

  if(getMorale()==0 && boss && dynamic_cast<AntHero*>(boss)) {
    bossName="";
    setBoss(0);   
  }

  setFighting(false);

  if (!boss) {
    std::cout<<"AntMann::eventNoJob but no job name:"<<getName()<<" boss:"<<bossName<<std::endl;
    if (bossName.length()==0) {
      AntEntity *nextHouse=getMap()->getNext(this,"house");
      std::cout<<"nextHouse "<<nextHouse<<std::endl;
      if (nextHouse) {
        AntHouse *house=dynamic_cast<AntHouse*>(nextHouse);
        if (house) {
          boss=house;
          newRestJob((rand()%10)/5.0);
          house->signUp(this);
        }
      }
    } else {
      AntEntity *e=getMap()->getByName(bossName);
      if(!e) {
        throw std::runtime_error("No entity found with name "+bossName);
      }
      boss=dynamic_cast<AntBoss*>(e);
      if (boss)
        boss->signUp(this);
      else {
        std::cout<<"Could not find Boss by Name:"<<bossName<<" e:"<<e<<" boss:"<<boss<<" type:"<<typeid(*e).name()<<std::endl;
      }
    }

  }

  if (boss) {
    boss->assignJob(this);
  }

}

const AGString &AntMan::getMeshState() const
{
  return meshState;
}


void AntMan::setMeshState(const AGString& pname)
{
  AGString name=pname;
  if (name=="row")
    name="walk";

  name=checkOnWater(name);
  meshState=name;
  float dir=getDirection();
  if (name=="fight") {
    if (getMode()==MOVING) {
      setMesh("man","walk");
    } else {
      //assert{["dagger","shield","sword","bow"].member?(getWeapon)}
      setMesh("man",AGString("fight_")+getWeapon());
    }
  } else if (name=="dead") {
    setMesh("man","grave");
  } else if (name=="row") {

    setMesh("man","boat");
  } else if (name=="stand" || name=="axe" || name=="pick" || name=="wood" || name=="stone" || name=="flour"
      || name=="corn" || name=="walk" || name=="sitdown" || name=="sit") {

    setMesh("man",name);

  }
  setDirection(dir);

}

void AntMan::simDeath()
{
  AntPerson::simDeath();
  if (boss)
    boss->removeMan(this);
}


AGString AntMan::getWeapon()
{
  if (resource.get("bow")>0)
    return "bow";

  if (resource.get("sword")==0)
    return "dagger";
  if (resource.get("shield")==0)
    return "sword";
  return "shield";
}

void AntMan::setDefense(float f)
{
  AntEntity::setDefense(f);
}

void AntMan::setMoraleStrength(float f)
{
  AntEntity::setMoraleStrength(f);
}

void AntMan::setStrength(float f)
{
  AntEntity::setStrength(f);
}



void AntMan::playSound(const AGString& soundName)
{
  AntEntity::playSound(soundName);
}

void AntMan::removeMeFromMap()
{
  getMap()->removeEntity(this);
}

void AntMan::setFighting(bool flag)
{
  fighting=flag;
}
void AntMan::saveXML(Node &n) const {
  AntPerson::saveXML(n);
  AGString bname=bossName;
  if(boss)
    bname=boss->getEntity()->getName();
  if(bname.length()>0)
    n.set("bossName",bname);
  n.set("meshState",meshState); 
  n.set("age",age);
  n.set("dead",dead);
  n.set("fighting",fighting);
  n.set("fetchResource",fetchResource);
}

void AntMan::loadXML(const Node& n)
{
  AntEntity::loadXML(n);
  bossName=n.get("bossName");
  meshState=n.get("meshState");
  age=n.get("age").toFloat();
  dead=n.get("dead").toBool();
  fighting=n.get("fighting").toBool();
  fetchResource=n.get("fetchResource");
  setMeshState(meshState);
}

void AntMan::setBoss(AntBoss* pBoss)
{
  if (boss) {
    boss->removeMan(this);
  }
  boss=pBoss;
  if(boss)
    boss->signUp(this);
}

AGString AntMan::getFetchResource() const
{
  return fetchResource;
}
void AntMan::setFetchResource(const AGString& r)
{
  fetchResource=r;
}


void AntMan::digResource(const AGString& res)
{
  newRestJob(2+agRand(2.0f),true);
  if (res=="wood")  {
    setMeshState("axe");
  } else if (res=="fruit") {
    setMeshState("stand");
  } else {
    setMeshState("pick");
  }

}
void AntMan::collectResource(const AGString& res)
{
  if (res=="wood") {
    setMeshState("wood");
  } else if (res=="stone" || res=="ore" || res=="coal" || res=="food" || res=="corn" || res=="crop" || res=="flour") {
    // FIXME new animation for other resources
    setMeshState("stone");
  }
}


float AntMan::canCarry() const
{
  return 3;
}

void AntMan::eventDefeated() {
  CTRACE;
  leaveBoss();
}
void AntMan::eventMoraleLow() {
  CTRACE;
  leaveBoss();
}
    
void AntMan::leaveBoss() {
  CTRACE;
  setBoss(0);
  delJob();
}
    
AntBoss* AntMan::getBoss() {
  return boss;
}
