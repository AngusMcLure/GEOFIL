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

void agent::calc_general_risk(double d){
    double c = 1;
    int age = int(this->age/365);
    if(age <= 4) c = c0_4;
    else if(age <= 15) c = c5_15;

    double p = r_b*c * p_c*(1 - d/r_r) * r_w;      //bites * positive * probability reciving mated worm
    if(drand48() < p) ++worms;
}

void agent::renew_epidemics(){
    int deaths = 0;
    for(int i = 0; i < worms; ++i){
        if(drand48() < exp(-r_d)) ++deaths;
    }
    worms -= deaths;
    
    double p = 1 - pow(1-exp(-r_p), worms);
    if(drand48() < p){
        worms = 0;
        epids = 'i';
    }
}
