//
//  agent.cpp
//  synPop
//
//  Created by Sting Xu on 2/11/17.
//  Copyright © 2017 Sting Xu. All rights reserved.
//
#include "agent.h"

agent::agent(int aid, int age, char gendr, char margs, hhold *h_d, workp *w_p, schol *s_h){
    this->aid = aid;
    this->age = age;
    this->gendr = gendr;
    this->margs = margs;
   
    this->h_d = h_d;
    this->w_p = w_p;
    this->s_h = s_h;
    
    spw = NULL;
    mom = NULL;
    dad = NULL;
    
    births = 0;
    bth_wind = 0;
    
    worms = 0;
    epids = 's';
    clock_pre = -1;
    clock_inf = -1;
    active_len = 0;
}

agent::~agent(){
    spw = NULL;
    mom = NULL;
    dad = NULL;
    h_d = NULL;
    w_p = NULL;
    s_h = NULL;
    
    chdr.clear();
}

void agent::add_child(agent *p){
    chdr.insert(pair<int, agent*>(p->aid, p));
}

void agent::calc_risk(double prv){
    double c = 1;
    int age = int(this->age/365);
    if(age <= 4) c = c0_4;
    else if(age <= 15) c = c5_15;

    double p = r_b*c * prv * r_w;      //bites * positive * probability reciving mated worm
    //cout << fixed << setprecision(5) << p << endl;
    if(drand48() < p) ++worms;
}

void agent::renew_epidemics(){
    if(epids == 's'){
        if(worms > 0){
            if(drand48() < s_w){
                epids = 'e';
                clock_pre = 210;
            }
            worms = 0;
        }
    }
    else if(epids == 'e'){
        if(clock_pre > 0) --clock_pre;
        
        if(clock_pre == 0){
            epids = 'i';
            clock_inf = 14;
            active_len = 6*365 + drand48()*2*365;   
        }
    }
    else if(epids == 'i'){
        if(clock_inf > 0) --clock_inf;
        
        --active_len;
        if(active_len == 0) epids = 'r';
    }
    else if(epids == 'r' && drand48() > 0.99726402359/*exp(-r_l)*/)
        epids = 's';
}
