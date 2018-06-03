//
//  agent.cpp
//  synPop
//
//  Created by Sting Xu on 2/11/17.
//  Copyright © 2017 Sting Xu. All rights reserved.
//
#include "agent.h"

//extern int rb_working, rb_offwork;

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

void agent::calc_risk(double prv, char time, double c){
    double p;
    if(time == 'd') p = rb_working*c * prv * r_w;      //bites * positive * probability reciving mated worm
    else p = rb_offwork*c * prv * r_w;
    if(drand48() < p) ++worms;
}

void agent::renew_epidemics(){
    if(epids == 's'){
        clock_pre = 182 + drand48()*183;
        double s_w = pow(0.13, clock_pre/(double)75);
        
        if(drand48() < 1-pow(1-s_w, worms)){
            epids = 'e';
        }
        else clock_pre = 0;
        worms = 0;
    }
    else if(epids == 'e'){
        if(clock_pre > 0) --clock_pre;
        
        if(clock_pre == 0){
            epids = 'i';
            clock_inf = 14;
            active_len = 4*365 + drand48()*2*365;
        }
    }
    else if(epids == 'i'){
        if(clock_inf > 0) --clock_inf;
        
        --active_len;
        if(active_len == 0){
            /*epids = 'r';
            clock_rmv = 2*365 + drand48()*365;*/
            epids = 's';
        }
    }
    /*else if(epids == 'r'){
        if(clock_rmv > 0) --clock_rmv;
        if(clock_rmv == 0) epids = 's';
    }*/
}
