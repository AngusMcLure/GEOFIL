//
//  sites.hpp
//  synPop
//
//  Created by Sting Xu on 5/11/17.
//  Copyright © 2017 Sting Xu. All rights reserved.
//

#ifndef sites_hpp
#define sites_hpp

#include "headers.h"
using namespace std;

class hhold{
public:
    int hid;                            //hhold id
    agent *hldr;                        //hhold holder
    char type;                          /* Married-couple family              ----- 0
                                           Female householder, no husband     ----- 1
                                           Male householder, no wife          ----- 2
                                           Householder living alone           ----- 3
                                           Householder with non-relatives     ----- 4
                                        */
    int size;
    map<int, agent*> mmbrs;             //hhold members
    double lat, log;
    double area;
    
    rbldg *rdg;
    mblok *mbk;
    cblok *cbk;
    
    hhold(int hid, int size, char type, rbldg *rdg, mblok *mbk, cblok *cbk);
    ~hhold();
    
    void asg_bldg(rbldg *rdg);
    void add_mmbr(agent *p, char role);
    void rmv_mmbr(agent *p);
    void update();
    void removd();
};

class workp{
public:
    int wid;
    int size;
    map<int, agent*> emplys;             //employees members
    double lat, log;
    double area;
    
    map<int, wbldg*> wdg;                         //building list
    mblok *mbk;
    cblok *cbk;
    
    workp(int wid, int size, mblok *mbk, cblok *cbk);
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
    int size;
    map<int, agent*> tchrs;
    map<int, agent*> stdts;
    
    double lat, log;
    double area;
    
    sbldg *sdg;
    mblok *mbk;
    cblok *cbk;
    
    void asg_bldg(sbldg *sdg);
    void add_tchr(agent *p);
    void add_stdt(agent *p);
    void rmv_tchr(agent *p);
    void rmv_stdt(agent *p);
    void update();
    void removd();
};

#endif /* sites_hpp */
