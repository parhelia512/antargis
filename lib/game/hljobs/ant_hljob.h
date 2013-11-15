#ifndef __HL_JOB_H
#define __HL_JOB_H

#include <ag_geometry.h>

#include <vector>

class AntBoss;
class AntMan;
class AntHero;
class AntEntity;
class AntPerson;
class AntMap;

//#define ANTHLJOB_SAVE_ONLY_ID

class AntHLJob
{
public:
  AntHLJob ( AntBoss *pBoss );

  virtual void checkPerson ( AntPerson *p );
  virtual bool finished() =0;

  AntBoss *getBoss();
  AntEntity *getBossEntity();
  std::vector<AntPerson*> getMenWithoutBoss();
  std::vector<AntPerson*> getMenWithBoss();
  void assignAll();

  virtual AGString xmlName() const=0;
  virtual void saveXML(Node &node) const;
  virtual void loadXML(const Node &node);

protected:
  virtual AntMap *getMap();
  void sit ( AntPerson* man );
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
