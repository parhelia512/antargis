#include "ant_hero.h"
#include "ant_models.h"
#include "map.h"
#include "ant_hljob_rest.h"
#include "ant_fire.h"

AntHero::AntHero ( AntMap* pMap ) : AntPerson ( pMap ) {
  fire=0;
}

AntHero::~AntHero() throw()
{
}

void AntHero::init()
{
  AntPerson::init();
  AntBoss::init();
  checkResources();
}


void AntHero::setupMesh()
{
  setMeshState ( "normal" );
}


void AntHero::setMeshState ( const AGString& pname )
{
  AGString name=pname;
  if ( name=="row" )
  {
    name="walk";
  }
  //origMeshState=name;
  name=checkOnWater ( name );
  meshState=name;
  float dir=getDirection();

  Scene *scene=getScene();
  if ( name=="row" )
  {
    setMesh ( AntModels::createModel ( scene,"hero","" ) );
    addMesh ( AntModels::createModel ( scene,"hero","boat" ),AGVector3 ( 0,0,0 ) );
  }
  else if ( name=="dead" )
  {
    setMesh ( AntModels::createModel ( scene,"hero","grave_hero" ) );
  }
  else
  {
    setMesh ( AntModels::createModel ( scene,"hero","" ) );
  }
  setDirection ( dir );
  setupRing();
  cdebug("setMeshState Hero"<<pname);
}

const AGString &AntHero::getMeshState() const {
  return meshState;
}

void AntHero::setupRing() {
}

void AntHero::loadXML ( const Node& node )
{
  AntEntity::loadXML ( node );
  primary= ( AGString ( "true" ) ==node.get ( "primary" ) );
  std::cout<<"PRIMARY:"<< ( primary?"true":"false" ) <<std::endl;
  AntBoss::loadXMLBoss ( node );
}
void AntHero::saveXML ( Node& node ) const
{
  AntEntity::saveXML ( node );
  if ( primary )
  {
    node.set ( "primary","true" );
  }
}
bool AntHero::isPrimary() const
{
  return primary;
}

AntEntity* AntHero::getEntity()
{
  return this;
}

void AntHero::eventNoHlJob()
{
  setHlJob ( new AntHLJobRest ( this,20 ) );
}

void AntHero::eventNoJob()
{
  //AntPerson::eventNoJob();

  checkHlJobEnd();
  assignJob ( this );
}


AntMap* AntHero::getMap()
{
  return AntPerson::getMap();
}


int AntHero::getID()
{
  return AntPerson::getID();
}


void AntHero::setFire(bool flag) {

  if(fire) {
    //getMap()->removeEntity(fire);
    fire->disable();
    fire=0;
  }

  if(flag) {
    AGVector2 firePos=getPos2D();
    firePos+=AGVector2(0.5,-0.5);
    fire=new AntFire(getMap());
    fire->setPos(firePos);
    fire->init();
  }
}


void AntHero::setHlJob ( AntHLJob *job ) {
  AntBoss::setHlJob(job);
  setFire(false);
}
