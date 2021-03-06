#ifndef __HL_JOB_H
#define __HL_JOB_H

#include <ag_geometry.h>
#include <ant_person.h>

#include <vector>

class AntBoss;
class AntMan;
class AntHero;
class AntEntity;
class AntMap;

//#define ANTHLJOB_SAVE_ONLY_ID

class AntHLJob
{
public:
  AntHLJob ( AntBoss *pBoss );
  virtual ~AntHLJob();

  virtual bool checkPerson ( AntPerson *p );
  virtual bool finished() =0;
  virtual bool startTogether() const;
  virtual void init();

  AntBoss *getBoss();
  AntEntity *getBossEntity();
  std::vector<AntPerson*> getMenWithoutBoss();
  std::vector<AntPerson*> getMenWithBoss();
  void assignAll();

  virtual AGString xmlName() const=0;
  virtual void saveXML(Node &node) const;
  virtual void loadXML(const Node &node);
  virtual bool fireBurning() const;
  virtual void eventJobDiscarded();

  virtual bool allAre(AntPerson::JobMode mode);

protected:
  virtual AntMap *getMap();
  bool sit ( AntPerson* man, const AGVector2 &pBasePos=AGVector2(-1,-1) );
  bool moveTo(AntPerson *man, const AGVector2 &pos, bool withFormation=false);
  virtual AGVector2 basePos();

private:
  AntMap *mMap;
#ifdef ANTHLJOB_SAVE_ONLY_ID
  int heroId;
#else
  AntBoss *mBoss;
#endif
};

#endif
