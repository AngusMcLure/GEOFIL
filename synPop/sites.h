//
//  sites.hpp
//  synPop
//
//  Created by Sting Xu on 5/11/17.
//  Copyright © 2017 Sting Xu. All rights reserved.
//

#ifndef sites_hpp
#define sites_hpp

#include "agent.h"
using namespace std;

//census blocks
class mblok;   //meshblock, smallest census unit
class cblok;   //cities or other large census area

//pop by age groups
class agrp;
class agrps;

class hhold{
public:
    int hid;                            //hhold id
    char typ;                          /* Married-couple family              ----- 0
                                           Female householder, no husband     ----- 1
                                           Male householder, no wife          ----- 2
                                           Householder living alone           ----- 3
                                           Householder with non-relatives     ----- 4
                                        */
    int siz;
    
    double lat, log;
    double area;
    rbldg *rdg;
    
    agent *hldr;                        //hhold holder
    map<int, agent*> mmbrs;             //hhold members
    
    hhold(int hid, int siz, char typ);
    ~hhold();
    
    void asg_bldg(rbldg *rdg);
    bool asg_hldr(agent *p);
    void add_mmbr(agent *p, char role);
    void rmv_mmbr(agent *p);
    bool updt_hhold();
};

class workp{
public:
    int wid;
    int siz;
    map<int, agent*> emplys;             //employees members
    double lat, log;
    double area;
    
    map<int, wbldg*> wdg;                         //building list
    
    workp(int wid, int siz);
    ~workp();
    
    void asg_bldg(wbldg *wdg);
    void add_mmbr(agent *p);
    void rmv_mmbr(agent *p);
    void update();
    void removd();
};

class schol{
public:
    int sid;
    int siz;
    double lat, log;
    double area;
    
    map<int, sbldg*> sdg;
    
    map<int, agent*> tchrs;
    map<int, agent*> stdts;
    
    void asg_bldg(sbldg *sdg);
    void add_tchr(agent *p);
    void add_stdt(agent *p);
    void rmv_tchr(agent *p);
    void rmv_stdt(agent *p);
    void update();
    void removd();
};

class rbldg{
public:
    int bid;
    double lat, log;
    double area;
    hhold *hd;
    mblok *mbk;
    cblok *cbk;
    
    rbldg(int bid, double lat, double log, double area, mblok *mbk, cblok *cbk);
    ~rbldg();
    //void asgn_hhold(hhold *p);
};

class wbldg{
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

#endif /* sites_hpp */
