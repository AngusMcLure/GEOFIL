//
//  bldgs.hpp
//  synPop
//
//  Created by Sting Xu on 5/11/17.
//  Copyright © 2017 Sting Xu. All rights reserved.
//

#ifndef bldgs_hpp
#define bldgs_hpp

#include "headers.h"
using namespace std;

class rbldg{
public:
    int bid;
    double lat, log;
    double area;
    
    hhold *hd;
    mblok *mbk;
    cblok *cbk;
    
    rbldg(int bid, double lat, double log, double area, mblok *mbk, cblok *cbk){
        this->bid = bid;
        this->lat = lat;
        this->log = log;
        this->area = area;
        this->mbk = mbk;
        this->cbk = cbk;
        this->hd = NULL;
    }
    
    ~rbldg(){
        mbk = NULL;
        cbk = NULL;
        hd = NULL;
    }
};

class wbldg{
    int bid;
    double lat, log;
    double area;
    
    workp *wkp;
    mblok *mbk;
    cblok *cbk;
    
    wbldg(int bid, double lat, double log, double area, wokp *wkp, mblok *mbk, cblok *cbk){
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

#endif /* bldgs_hpp */
