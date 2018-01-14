//
//  agent.cpp
//  synPop
//
//  Created by Sting Xu on 2/11/17.
//  Copyright © 2017 Sting Xu. All rights reserved.
//
#include "agent.h"

agent::agent(int aid, int age, char gendr, char margs, hhold *hd, workp *wp, schol *sh){
    this->aid = aid;
    this->age = age;
    this->gendr = gendr;
    this->margs = margs;
   
    this->hd = hd;
    this->wp = wp;
    this->sh = sh;
    
    works = '-';
    bthwind = 0;
    epids = 'S';
}

agent::~agent(){
    spw = NULL;
    mom = NULL;
    dad = NULL;
    mom_law = NULL;
    dad_law = NULL;
    hd = NULL;
    wp = NULL;
    sh = NULL;
    
    chldr.clear();
}

void agent::add_child(agent *p){
    chldr.insert(pair<int, agent*>(p->aid, p));
}

void agent::clr_sociallinks(){
    if(spw != NULL){
        spw->margs = 'S';
        margs = 'S';
        
        spw->spw = NULL;
        spw = NULL;
    }
    
    mom = NULL;
    dad = NULL;
    mom_law = NULL;
    dad_law = NULL;
    
    for(map<int, agent*>::iterator k = chldr.begin(); k != chldr.end(); ++k){
        agent *b = k->second;
        
        if(gendr == 'm'){
            if(b->dad->aid == aid) b->dad = NULL;
            if(b->dad_law->aid == aid) b->dad_law = NULL;
        }
        else{
            if(b->mom->aid == aid) b->mom = NULL;
            if(b->mom_law->aid == aid) b->mom_law = NULL;
        }
    }
    chldr.clear();
}
