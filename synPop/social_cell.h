//
//  social_cell.h
//  synPop
//
//  Created by Sting Xu on 22/1/18.
//  Copyright © 2018 Sting Xu. All rights reserved.
//

#ifndef social_cell_h
#define social_cell_h

#include "bldg.h"
using namespace std;

//family units
struct unit{
    agent *father;
    agent *mother;
    vector<agent*> child;
    vector<int> avail_ages;
    
    unit(agent *p = NULL, agent *q = NULL){ father = p;  mother = q;  child.clear();  avail_ages.clear(); }
    ~unit(){
        father = NULL;
        mother = NULL;
        child.clear();
        child.shrink_to_fit();
        avail_ages.clear();
        avail_ages.shrink_to_fit();
    }
    int u_size(){
        int n = int(child.size());
        if (father != NULL) ++n;
        if (mother != NULL) ++n;
        return n;
    }
};

class hhold{
public:
    int hid;                            //hhold id
    int siz;
    
    double lat, log;
    double area;
    rbldg *rdg;
    
    agent *hldr;                        //hhold holder
    map<int, agent*> mmbrs;             //hhold members
    
    hhold(int hid, int siz = 0, agent *holder = NULL);
    ~hhold();
    
    void asg_bldg(rbldg *rdg);
    void add_mmbr(agent *p);
    bool asg_hldr(agent *p);
    void rmv_mmbr(agent *p);
    void updt_hhold();
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

#endif /* social_cell_h */
