#ifndef __ANT_HLJOB_FIGHT_ANIMAL_H
#define __ANT_HLJOB_FIGHT_ANIMAL_H

#include "ant_hljob_moving.h"

class AntAnimal;

class AntHLJobFightAnimal: public AntHLJobMoving {
  public:
    AntHLJobFightAnimal(AntBoss* pBoss, AntAnimal *pTarget);

    bool finished();
    bool checkPerson ( AntPerson* person );

  private:
    void killAnimal();

    enum FightAnimalState { START, STATE_EAT, NEARLY_FINISHED };
    FightAnimalState state;
    bool mFinished;
    AntAnimal *mTarget;
};

#endif
