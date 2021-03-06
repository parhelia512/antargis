#include "ant_boss.h"
#include "ant_hljob.h"
#include "entity.h"
#include "ant_person.h"
#include "ant_man.h"
#include "ant_formation.h"
#include "ant_player.h"
#include "map.h"
#include "ant_hljobs_create.h"
#include "ant_hljob_fighting.h"

#include <algorithm>

AntBoss::AntBoss ( ) :hlJob ( 0 ),menToAddCount ( 0 )
{
}

AntBoss::~AntBoss() throw()
{
  getPlayer()->remove ( this );
}

void AntBoss::setupMeshBoss()
{
  setupMesh();
  setupRing();
}

void AntBoss::init()
{
  setupMeshBoss();

}

void AntBoss::removeMan ( AntMan* man )
{
  menWithoutBoss.remove ( man );
  eventLostMan(man);
}

void AntBoss::signUp ( AntMan* man )
{
  if(!man)
    throw std::runtime_error("signing up man == NULL ???");
  menWithoutBoss.push_back ( man );
}


void AntBoss::assignJob ( AntPerson* person )
{
  if ( !hlJob )
    eventNoHlJob();

  if ( hlJob )
  {
    hlJob->checkPerson ( person );
  }
  else
  {
    person->newRestJob ( 100 );
    person->setMeshState ( "sit" );
  }

}
std::vector< AntPerson* > AntBoss::getMenWithBoss()
{
  std::vector< AntPerson* > l;
  auto bossPerson=dynamic_cast<AntPerson*> ( getEntity() );

  // don#t add if it's a house
  if(bossPerson)
    l.push_back (bossPerson);
  std::copy ( menWithoutBoss.begin(),menWithoutBoss.end(),std::back_inserter ( l ) );
  return l;
}
std::vector< AntPerson* > AntBoss::getMenWithoutBoss()
{
  std::vector< AntPerson* > l;
  std::copy ( menWithoutBoss.begin(),menWithoutBoss.end(),std::back_inserter ( l ) );
  return l;
}

AntFormation* AntBoss::getFormation()
{
  return formation;
}

AGVector2 AntBoss::getFormation ( AntPerson* e, const AGVector2& v )
{
  if ( formation )
    return formation->getPosition ( e,v );
  else
    return getEntity()->getPos2D();
}

void AntBoss::setHlJob ( AntHLJob* job )
{
  if(hlJob)
    hlJob->eventJobDiscarded();

  delete hlJob;
  hlJob=job;
  if(!hlJob || hlJob->startTogether())
    delJobs();
  if(hlJob)
    hlJob->init();
}
AntHLJob *AntBoss::getHlJob() {
  return hlJob;
}

void AntBoss::setFormation ( AntFormation* pformation )
{
  formation=pformation;
}

AntPlayer* AntBoss::getPlayer()
{
  return player;
}
void AntBoss::setPlayer ( AntPlayer* pPlayer )
{
  player=pPlayer;
}

void AntBoss::checkHlJobEnd()
{
  if ( hlJob )
  {
    if ( hlJob->finished() )
    {
      eventNoHlJob();
    }
  }
}

void AntBoss::saveXMLBoss ( Node &node ) const {
  if(hlJob) {
    Node &child=node.addChild(hlJob->xmlName());
    hlJob->saveXML(child);
  }
}
void AntBoss::loadXMLBoss ( const Node& node )
{
  if ( node.get ( "men" ).length() >0 )
  {
    menToAddCount=node.get ( "men" ).toInt();

    for ( size_t i=0; i<menToAddCount; i++ )
    {
      AntMan *man=new AntMan ( getEntity()->getMap() );
      man->init();
      man->setPos ( getEntity()->getPos2D() );
      man->setBoss ( this );
      getEntity()->getMap()->insertEntity ( man );
    }

    menToAddCount=0;
  }
  for(auto child:node.getChildren()) {
    if(child->getName().substr(0,5)=="hljob") {
      AntHLJob *job=Antargis::createFromXML(this,*child);
      if(job) {
        cdebug("loaded hljob:"<<job);
        setHlJob(job);
      }
    }
  }

}

std::vector< AntPerson* > AntBoss::getRestingMenWithHero()
{
  std::vector< AntPerson* > restList;
  if ( getEntity()->isResting() )
    restList.push_back ( dynamic_cast<AntPerson*> ( getEntity() ) );
  for ( std::list<AntMan*>::iterator manIterator=menWithoutBoss.begin(); manIterator!=menWithoutBoss.end(); manIterator++ )
  {
    if ( ( *manIterator )->isResting() )
      restList.push_back ( *manIterator );

  }
  return restList;
}


std::vector<AntPerson*> AntBoss::getMenWithoutBoss(AntPerson::JobMode mode) {
  std::vector<AntPerson*> r;
  std::vector<AntPerson*> all=getMenWithoutBoss();
  std::copy_if(all.begin(),all.end(),std::back_inserter(r),
      [mode](AntPerson* person) {
      return person->getMode()==mode;
      });
  return r;
}

size_t AntBoss::menCount() const {
  return menWithoutBoss.size();
}

void AntBoss::eventLostMan(AntPerson *person) {
  if(hlJob) {
    auto fightJob=dynamic_cast<AntHlJobFighting*>(hlJob);
    if(fightJob) {
      fightJob->removeFightingperson(person);
    }
  }
} 

void AntBoss::delJobs() {
  for(auto entity:getMenWithBoss()) {
    entity->delJob();
  }
}

std::vector<AntStockNeed > AntBoss::neededStock() {
  std::vector< AntStockNeed > a;
  a.push_back(AntStockNeed("wood",15,0));
  a.push_back(AntStockNeed("stone",15,0));
  a.push_back(AntStockNeed("food",15,0));
  return a;
}

size_t AntBoss::atHome()
{
  return getMenWithoutBoss(AntPerson::READY).size();
}
