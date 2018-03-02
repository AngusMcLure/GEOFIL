//
//  social_cell.h
//  synPop
//
//  Created by Sting Xu on 22/1/18.
//  Copyright © 2018 Sting Xu. All rights reserved.
//

#ifndef social_cell_h
#define social_cell_h

#include "units.h"
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
    int size;
    
    bool exp;                           //hhold expanding
    
    double lat, log;
    double area;
    rbldg *rdg;
    
    agent *hldr;                        //hhold holder
    map<int, agent*> mmbrs;             //hhold members
    
    hhold(int hid, int size = 0, agent *holder = NULL);
    ~hhold();
    
    void adopted();
    void update_hhold();
    void asg_rbldg(rbldg *rdg);
    void add_member(agent *p);
    bool asg_holder(agent *p);
    void rmv_member(agent *p);
    bool is_member(agent *p);
    
};

#endif /* social_cell_h */