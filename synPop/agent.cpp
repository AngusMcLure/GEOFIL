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
    
    role = '-';
    works = '-';
    bwind = 0;
    epids = 'S';
}

agent::~agent(){
    spw = NULL;
    hd = NULL;
    wp = NULL;
    sh = NULL;
    
    chldr.clear();
    parnt.clear();
}

void agent::add_child(agent *p){
    chldr.insert(pair<int, agent*>(p->aid, p));
}

void agent::add_parent(agent *p){
    parnt.insert(pair<int, agent*>(p->aid, p));
}

void agent::clr_sociallinks(){
    if(spw != NULL){
        spw->margs = 'S';
        margs = 'S';
        
        spw->spw = NULL;
        spw = NULL;
    }
    
    for(map<int, agent*>::iterator k = parnt.begin(); k != parnt.end(); ++k){
        k->second->chldr.erase(aid);
    }
    parnt.clear();
    
    for(map<int, agent*>::iterator k = chldr.begin(); k != chldr.end(); ++k){
        k->second->parnt.erase(aid);
    }
    chldr.clear();
}
