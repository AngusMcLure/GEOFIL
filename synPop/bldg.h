//
//  bldgs.h
//  synPop
//
//  Created by Sting Xu on 22/1/18.
//  Copyright © 2018 Sting Xu. All rights reserved.
//

#ifndef bldgs_h
#define bldgs_h

#include "agent.h"
using namespace std;

class rbldg;        //residential
class wbldg;        //work buildings
class sbldg;        //school bldgs

class mblok;
class cblok;

class rbldg{
public:
    int bid;
    double log, lat;
    double area;
    hhold *h_d;
    mblok *mbk;
    cblok *cbk;
    
    rbldg(int bid, double log, double lat, double area, mblok *mbk, cblok *cbk);
    ~rbldg();
};

class wbldg{
public:
    int bid;
    double lat, log;
    double area;
    workp *wkp;
    mblok *mbk;
    cblok *cbk;
    
    wbldg(int bid, double lat, double log, double area, workp *wkp, mblok *mbk, cblok *cbk){
        this->bid = bid;
        this->lat = lat;
        this->log = log;
        this->area = area;
        this->wkp = wkp;
        this->mbk = mbk;
        this->cbk = cbk;
    }
    
    ~wbldg(){
        wkp = NULL;
        mbk = NULL;
        cbk = NULL;
    }
};

class sbldg{
    int bid;
    double lat, log;
    double area;
    schol *sch;
    mblok *mbk;
    cblok *cbk;
    
    sbldg(int bid, double lat, double log, double area, schol *sch, mblok *mbk, cblok *cbk){
        this->bid = bid;
        this->lat = lat;
        this->log = log;
        this->area = area;
        this->sch = sch;
        this->mbk = mbk;
        this->cbk = cbk;
    }
    
    ~sbldg(){
        sch = NULL;
        mbk = NULL;
        cbk = NULL;
    }
};

#endif /* bldgs_h */
