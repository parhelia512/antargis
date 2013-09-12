#ifndef __ANT_HLJOB_FETCHING_H
#define __ANT_HLJOB_FETCHING_H

#include "ant_hljob.h"

#include <set>
#include <map>

class AntHLJobFetching:public AntHLJob {
public:
    AntHLJobFetching(AntBoss* pBoss);

    virtual void checkPerson(AntPerson*);
    virtual bool finished();
    virtual AGVector2 basePos();
protected:
    struct StockNeed {
        std::string resource;
        float amount,currentNeed;
	StockNeed(const std::string &r,float amount,float cNeed);
	
	bool operator<(const StockNeed &o) const;
    };

    virtual std::vector<StockNeed> neededStock();

private:
    bool atHome(AntMan *man);
    std::set<AntMan*> menAtHome;
    bool checkFood(AntMan *man);
    void fetchForStock(std::vector<StockNeed> needed, AntMan* man);
    
    enum MODE {FETCH,REST};
    MODE mode;
};

#endif
