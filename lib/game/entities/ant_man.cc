#include "ant_man.h"
#include "map.h"
#include "ant_house.h"
#include "ant_models.h"
#include "anim_mesh.h"

AntMan::AntMan(AntMap* pMap): AntEntity(pMap)
{
    boss=0;
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

AGString genName() {
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

    return nstart[rand()%nstart.size()]+
           nend[rand()%nend.size()];
}


void AntMan::init()
{
    AntEntity::init();
    setHunger(0.006);
    setProvide("man",true);

    setName(genName());

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

    setFighting(false);

    if (!boss) {

        if (bossName.length()==0) {
            AntEntity *nextHouse=getMap()->getNext(this,"house");
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
            boss=dynamic_cast<AntBoss*>(e);
            if (boss)
                boss->signUp(this);
        }

    }

    if (boss) {
        boss->assignJob(this);
    }

}

void AntMan::setMeshState(const AGString& pname)
{
    AGString name=pname;
    if (name=="row")
        name="walk";

    name=checkOnWater(name);
    AGString meshState=name;
    float dir=getDirection();
    if (name=="fight") {
        if (mode==MOVING) {
            setMesh(AntModels::createModel(getScene(),"man","walk"));
            AnimMesh *m=dynamic_cast<AnimMesh*>(getFirstMesh());
            if (m)
                m->setAnimation("walk");
        } else {
            //assert{["dagger","shield","sword","bow"].member?(getWeapon)}
            setMesh(AntModels::createModel(getScene(),"man",AGString("fight_")+getWeapon()));
        }
    } else if (name=="dead") {
        setMesh(AntModels::createModel(getScene(),"man","grave"));
    } else if (name=="row") {

        setMesh(AntModels::createModel(getScene(),"man","sit"));
        AnimMesh *m=dynamic_cast<AnimMesh*>(getFirstMesh());
        if (m)
            m->setAnimation("sit");
        addMesh(AntModels::createModel(getScene(),"boat",""),AGVector3(0,0,0));
    } else if (name=="name" || name=="axe" || name=="pick" || name=="wood" || name=="stone" || name=="flour"
               || name=="corn" || name=="walk" || name=="sitdown" || name=="sit") {


        setMesh(AntModels::createModel(getScene(),"man",name));
        if (name=="stand" ||name=="walk" ||name=="sitdown"||name=="sit") {
            AnimMesh *m=dynamic_cast<AnimMesh*>(getFirstMesh());
            if (m)
                m->setAnimation(name);
        }

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
