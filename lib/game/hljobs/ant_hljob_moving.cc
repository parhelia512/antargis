#include "ant_hljob_moving.h"
#include "ant_hero.h"
#include "ant_man.h"
#include <map.h>
#include <path/ant_path_finder_complete.h>
#include <ant_formation_block.h>

AntHLJobMoving::AntHLJobMoving(AntBoss* pBoss): AntHLJob(pBoss)
{

}


AntHLJobMoving::AntHLJobMoving(AntBoss* pBoss, const AGVector2& pTargetPosition,float pDist,bool doFormat): AntHLJob(pBoss)
{
    initMoving(pTargetPosition,pDist,doFormat);
}

void AntHLJobMoving::initMoving(const AGVector2& pTargetPosition, float pDist, bool doFormat)
{
    targetPosition=pTargetPosition;
    dist=pDist;
    shouldFormat=doFormat;
    formatDist=0;

    if (getMap()) {
        waypoints=getMap()->getCompletePathFinder()->computePath(getBossEntity()->getPos2D(),pTargetPosition,getHero());
    } else {
    }

    waypoints.push_front(getHero()->getPos2D());
    waypoints.push_back(pTargetPosition);
    if (getMap()) {
        waypoints=getMap()->getCompletePathFinder()->refinePath(waypoints,getHero());
    }
    waypoints.pop_front();

    moveFinished=false;

    if (getMen().size()>0) {
        startFormatting();
    } else {
        startWalking();
    }
}


void AntHLJobMoving::init()
{
}


void AntHLJobMoving::checkPerson(AntPerson* p)
{
    CTRACE;
    if (moveFinished)
        return;
    cdebug("state:"<<state);
    switch (state) {
    case FORMAT: {
        cdebug("man mode:"<<p->getMode()<<" FORMAT:"<<AntMan::FORMAT);
        if (p->getMode()==AntMan::FORMAT) {
            p->setMode(AntMan::READY);
            p->setMeshState("stand");
            p->setDirection(formatDir().getAngle());
        }
        cdebug("count formatting:"<<countMen(AntMan::FORMAT));
        if (countMen(AntMan::FORMAT)==0) {
            cdebug("Go start walking");
            startWalking();
        }
    }break;
    case MOVING: {
        cdebug("WAYPOINTS:"<<waypoints.size());
        if (waypoints.size()>0)
            startFormatting();
        else {
            cdebug("FINISHED:");//<<@hero.getPos2D,@pos,@overalltarget

            moveFinished=true;
        }

    }break;
    }
}

AntHero* AntHLJobMoving::getHero()
{
    return dynamic_cast<AntHero*>(getBossEntity());
}


std::vector< AntPerson* > AntHLJobMoving::getMen()
{
    return getBoss()->getMenWithoutBoss();
}

void AntHLJobMoving::startFormatting()
{
    CTRACE;
    cdebug("startFormatting:::::::::::");
    cdebug("formatDir:"<<formatDir());
    getHero()->setFormation(new AntFormationBlock(getHero(),formatDir()));
    std::vector<AntPerson*> men=getMenWithBoss();
    for (std::vector<AntPerson*>::iterator man=men.begin();man!=men.end();man++) {
        AGVector2 pos=getHero()->getFormation(*man,getHero()->getPos2D());
        (*man)->setMeshState("walk");
        (*man)->newMoveJob(0,pos,formatDist);
        (*man)->setMode(AntPerson::FORMAT);
    }
    state=FORMAT;
    cdebug("startFormatting.");
}

AGVector2 AntHLJobMoving::formatDir()
{
    return (*waypoints.begin()-getBossEntity()->getPos2D()).normalized();
}

void AntHLJobMoving::startWalking()
{
    cdebug("startWalking:::::::::::");
    getHero()->setFormation(new AntFormationBlock(getHero(),formatDir()));
    targetPosition=*waypoints.begin();
    cdebug("waypoints #:"<<waypoints.size());
    cdebug("targetpos:"<<targetPosition);
    waypoints.pop_front();
    std::vector<AntPerson*> men=getMenWithBoss();
    for (std::vector<AntPerson*>::iterator man=men.begin();man!=men.end();man++) {
        AGVector2 f=getHero()->getFormation(*man,targetPosition);
        cdebug("new move job:"<<(*man)->getPos2D()<<" to "<<f);
        (*man)->newMoveJob(0,f,formatDist);
        (*man)->setMode(AntPerson::MOVING);
        (*man)->setMeshState("walk");

    }
    state=MOVING;
    cdebug("startWalking.");

}


void AntHLJobMoving::check(AntHero* p)
{
    checkPerson(p);
}

size_t AntHLJobMoving::countMen(AntMan::JobMode mode)
{
    std::vector< AntPerson* > men=getMenWithoutBoss();
    size_t count=0;
    for (std::vector< AntPerson* >::iterator i= men.begin();i!=men.end();i++) {
        AntMan *man=dynamic_cast<AntMan*>(*i);
        if (man)
            if (man->getMode()==mode)
                count++;
    }
    return count;
}

void AntHLJobMoving::check(AntMan* p)
{
    checkPerson(p);
}

bool AntHLJobMoving::finished()
{
    return moveFinished;
}