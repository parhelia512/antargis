#include "ant_hljob_moving.h"
#include "ant_hero.h"
#include "ant_man.h"
#include <map.h>
#include <path/ant_path_finder_complete.h>
#include <ant_formation_block.h>

AntHLJobMoving::AntHLJobMoving ( AntBoss* pBoss ) : AntHLJob ( pBoss )
{
  dontMove=false;
}


AntHLJobMoving::AntHLJobMoving ( AntBoss* pBoss, const AGVector2& pTargetPosition,float pDist,bool doFormat, bool doNothingAtAll ) : AntHLJob ( pBoss )
{
  targetPosition=pTargetPosition;
  dist=pDist;
  shouldFormat=doFormat;
  formatDist=0;
  dontMove=doNothingAtAll;

}

void AntHLJobMoving::init() {
  AntHLJob::init();
  if(!dontMove)
    initMoving();
}

void AntHLJobMoving::initMoving ()
{
  formatDist=0;

  if ( getMap() )
  {
    waypoints=getMap()->getCompletePathFinder()->computePath ( getBossEntity()->getPos2D(),targetPosition,getHero() );
  }
  else
  {
  }

  waypoints.push_front ( getHero()->getPos2D() );
  waypoints.push_back ( targetPosition );
  if ( getMap() )
  {
    waypoints=getMap()->getCompletePathFinder()->refinePath ( waypoints,getHero() );
  }
  waypoints.pop_front();

  moveFinished=false;

  if ( getMen().size() >0 )
  {
    startFormatting();
  }
  else
  {
    startWalking();
  }
}


bool AntHLJobMoving::checkPerson ( AntPerson* p )
{
  if ( moveFinished ) {
    // already finished - early out
    return true;
  }
  switch ( state )
  {
    case FORMAT:
      {
        if ( p->getMode() ==AntMan::FORMAT )
        {
          p->setMode ( AntMan::READY );
          p->setMeshState ( "stand" );
          p->setDirection ( formatDir().getAngle() );
        }
        if ( countMen ( AntMan::FORMAT ) ==0 )
        {
          startWalking();
        }
      }
      break;
    case MOVING:
      {
        if ( waypoints.size() >0 )
          startFormatting();
        else
        {
          moveFinished=true;
          eventMoveFinished();
          return true;
        }

      }
      break;
  }
  return false;
}
void AntHLJobMoving::eventMoveFinished()
{

}

AntHero* AntHLJobMoving::getHero()
{
  return dynamic_cast<AntHero*> ( getBossEntity() );
}


std::vector< AntPerson* > AntHLJobMoving::getMen()
{
  return getBoss()->getMenWithoutBoss();
}

void AntHLJobMoving::startFormatting()
{
  getHero()->setFormation ( new AntFormationBlock ( getHero(),formatDir() ) );
  std::vector<AntPerson*> men=getMenWithBoss();
  for ( std::vector<AntPerson*>::iterator man=men.begin(); man!=men.end(); man++ )
  {
    AGVector2 pos=getHero()->getFormation ( *man,getHero()->getPos2D() );

    ( *man )->setMeshState ( "walk" );
    ( *man )->newMoveJob ( 0,pos,formatDist );
    ( *man )->setMode ( AntPerson::FORMAT );
  }
  state=FORMAT;
}

AGVector2 AntHLJobMoving::formatDir()
{
  return ( *waypoints.begin()-getBossEntity()->getPos2D() ).normalized();
}

void AntHLJobMoving::startWalking()
{
  getHero()->setFormation ( new AntFormationBlock ( getHero(),formatDir() ) );
  targetPosition=*waypoints.begin();
  waypoints.pop_front();
  std::vector<AntPerson*> men=getMenWithBoss();
  for ( std::vector<AntPerson*>::iterator man=men.begin(); man!=men.end(); man++ )
  {
    AGVector2 f=getHero()->getFormation ( *man,targetPosition );
    float curDist=formatDist;
    if(waypoints.size()==0)
      curDist=dist;
    ( *man )->newMoveJob ( 0,f,curDist );
    ( *man )->setMode ( AntPerson::MOVING );
    ( *man )->setMeshState ( "walk" );

  }
  state=MOVING;

}



size_t AntHLJobMoving::countMen ( AntMan::JobMode mode )
{
  std::vector< AntPerson* > men=getMenWithoutBoss();
  size_t count=0;
  for ( std::vector< AntPerson* >::iterator i= men.begin(); i!=men.end(); i++ )
  {
    AntMan *man=dynamic_cast<AntMan*> ( *i );
    if ( man )
      if ( man->getMode() ==mode )
        count++;
  }
  return count;
}

bool AntHLJobMoving::finished()
{
  return moveFinished;
}


AGString AntHLJobMoving::xmlName() const {
  return "hljobMoving";
}
void AntHLJobMoving::saveXML(Node &node) const {
  AntHLJob::saveXML(node);
  node.set("targetPos",targetPosition.toString());
  node.set("nearingDist",dist);
  node.set("shouldFormat",shouldFormat);
}
void AntHLJobMoving::loadXML(const Node &node) {
  AntHLJob::loadXML(node);
  targetPosition=AGVector2(node.get("targetPos"));
  dist=node.get("nearingDist").toFloat();
  shouldFormat=node.get("shouldFormat").toBool();
  initMoving ();
}


bool AntHLJobMoving::fireBurning() const {
  return false;
}

void AntHLJobMoving::dontMoveAnymore() {
  if(!moveFinished) {
    moveFinished=true;
    eventMoveFinished();
  }
}
    
