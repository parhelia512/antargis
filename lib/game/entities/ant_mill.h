#ifndef ANT_MILL_H
#define ANT_MILL_H

#include "ant_house.h"

class AntMill:public AntHouse {
public:
    AntMill(AntMap* pMap);
    void init();
    
    void setupMesh();

    virtual AGString xmlName() const {
      return "antMill";
    }

    std::vector<AntStockNeed > neededStock();
    void process();
private:
};
#endif

