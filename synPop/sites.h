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
class agrps;

class hhold{
public:
    int hid;                            //hhold id
    char typ;                           // C-couple, F-female, M-male, A-alone, N-nonrelatives
    int siz;
    
    double lat, log;
    double area;
    rbldg *rdg;
    
    agent *hldr;                        //hhold holder
    map<int, agent*> mmbrs;             //hhold members
    
    hhold(int hid, int siz, char typ);
    ~hhold();
    
    void add_bldg(rbldg *rdg);
    bool asg_hldr(agent *p);
    void add_mmbr(agent *p);
    void add_hldr(agent *p);
    void rmv_mmbr(agent *p);
    void updt_hhold();
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
    
    void add_bldg(wbldg *wdg);
    void add_mmbr(agent *p);
    void rmv_mmbr(agent *p);
    void update();
    void removd();
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

class schol{
public:
    int sid;
    int siz;
    double lat, log;
    double area;
    
    map<int, sbldg*> sdg;
    
    map<int, agent*> tchrs;
    map<int, agent*> stdts;
    
    void add_bldg(sbldg *sdg);
    void add_tchr(agent *p);
    void add_stdt(agent *p);
    void rmv_tchr(agent *p);
    void rmv_stdt(agent *p);
    void update();
    void removd();
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
