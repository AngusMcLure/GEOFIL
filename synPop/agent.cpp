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
    clock_inf = -1;
}

agent::~agent(){
    spw = NULL;
    mom = NULL;
    dad = NULL;
    h_d = NULL;
    w_p = NULL;
    s_h = NULL;
    
    chdr.clear();
    for(int i = 0; i < wvec.size(); ++i){
        delete wvec[i];
    }
    wvec.clear();
}

void agent::add_child(agent *p){
    chdr.insert(pair<int, agent*>(p->aid, p));
}

void agent::calc_risk(double prv, char time, double c){
    double p;
    if(time == 'd') p = rb_working * c * prv * r_w;      //bites * positive * probability reciving mated worm
    else p = rb_offwork * c * prv * r_w;
    if(drand48() < p) ++worms;
}

void agent::renew_epidemics(){
    int clock_pre = 182 + int(drand48()*183);
    double s_w = pow(0.13, clock_pre/(double)75);

    if(drand48() < 1-pow(1-s_w, worms)){
        int active = 4*365 + int(drand48()*2*365);
        wvec.push_back(new worm('p', clock_pre, active));
    }
    
    char ch = 's';
    if(wvec.size() > 0){
        ch = 'e';
        for(int i = 0; i < wvec.size(); ++i){
            if(wvec[i]->status == 'm'){
                ch = 'i'; break;
            }
        }
    }
    
    if(epids == 's') epids = ch;
    else if(epids == 'e'){
        epids = ch;
        if(epids == 'i') clock_inf = 14;
    }
    else if(epids == 'i'){
        epids = ch;
        if(clock_inf > 0) --clock_inf;
    }
    
    worms = 0;
}

void agent::update(){
    if(wvec.size() > 0){
        for(int i = 0; i < wvec.size();){
            wvec[i]->update();
            
            if(wvec[i]->status == 'd'){
                delete wvec[i];
                wvec.erase(wvec.begin() + i);
            }
            else ++i;
        }
    }
    
    if(wvec.size() == 0) epids = 's';
    else{
        if(wvec.size() > 0) epids = 'e';
        for(int i = 0; i < wvec.size(); ++i){
            if(wvec[i]->status == 'm'){
                epids = 'i'; break;
            }
        }
    }
}
