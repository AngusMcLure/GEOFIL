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
    map<int, double> ngbbldg;   //neighbour buildings
    
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
        ngbbldg.clear();
    }
};

class bbldg{
    int bid;
    double lat, log;
    double area;
    map<int, double> ngbbldg;   //neighbour buildings
    string name;
    
    mblok *mbk;
    cblok *cbk;
    
    bbldg(int bid, double lat, double log, double area, string name, mblok *mbk, cblok *cbk){
        this->bid = bid;
        this->lat = lat;
        this->log = log;
        this->area = area;
        this->name = name;
        this->mbk = mbk;
        this->cbk = cbk;
        
        ngbbldg.clear();
    }
    
    ~work_bldg(){
        mbk = NULL;
        cbk = NULL;
        ngbbldg.clear();
    }
};

class gbldg{
    
};

class sbldg{
    
};

#endif /* bldgs_hpp */
