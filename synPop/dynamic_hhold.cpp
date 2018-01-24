//
//  sites.cpp
//  synPop
//
//  Created by Sting Xu on 5/11/17.
//  Copyright © 2017 Sting Xu. All rights reserved.
//

#include "agent.h"
#include "block.h"

hhold::hhold(int hid, int size, agent *holder){
    this->hid = hid;
    this->size = siz;
    hldr = holder;
}

hhold::~hhold(){
    this->rdg = NULL;
    this->hldr = NULL;
    this->mmbrs.clear();
}

void hhold::asg_rbldg(rbldg *rdg){
    this->rdg = rdg;

    lat = rdg->lat;
    log = rdg->log;
    area = rdg->area;
    
    rdg->h_d = this;
    rdg->cbk->rmv_vcnt_rbldg(rdg);
}

bool hhold::asg_holder(agent *p){
    if(p != NULL) hldr = p;
    else{
        //choose the most aged & married one to be new householder
        map<int, agent*>::iterator it = mmbrs.begin();
        for(map<int, agent*>::iterator k = mmbrs.begin(); k != mmbrs.end(); ++k){
            if(it->second->age < k->second->age || (it->second->margs != 'M' && k->second->margs == 'M'))
                it = k;
        }
        
        int age = it->second->age;
        if(age < 18*365) return false;          //all other members are children
        
        hldr = it->second;
    }
    
    if(hldr->spw != NULL && hldr->gendr == 'F')
        hldr = hldr->spw;
    
    return true;
}

void hhold::add_member(agent *p){
    p->h_d = this;
    mmbrs.insert(pair<int, agent*>(p->aid, p));
    
    size = int(mmbrs.size());
}

void hhold::rmv_member(agent *p){
    p->h_d = NULL;
    if(hldr->aid == p->aid){
        if(p->spw != NULL && is_member(p->spw)) hldr = p->spw;
        else hldr = NULL;
    }
    mmbrs.erase(p->aid);
    
    size = int(mmbrs.size());
}

bool hhold::is_member(agent *p){
    if(mmbrs.find(p->aid) != mmbrs.end()) return true;
    return false;
}

void hhold::asg_holder(){
    //choose the most aged one to be new householder
    map<int, agent*>::iterator j = mmbrs.begin();
    for(map<int, agent*>::iterator k = mmbrs.begin(); k != mmbrs.end(); ++k){
        if(j->second->age < k->second->age) j = k;
    }
    
    agent *cur = j->second;
    int age = cur->age;
    
    if(age < 15*365){           //all other members are children
        agent *q = NULL;
        for(map<int, agent*>::iterator k = rdg->cbk->fmal_marrd.begin(); k != rdg->cbk->fmal_marrd.end(); ++k){
            if(k->second->chldr.size() == 0){
                q = k->second;
                break;
            }
        }
        
        while(mmbrs.size() > 0){
            agent *bb = mmbrs.begin()->second;
            
            q->add_child(bb);       bb->mom = q;
            q->spw->add_child(bb);  bb->dad = q->spw;
            
            rmv_member(bb);
            q->h_d->add_member(bb);
        }
        
        
    }
    else{
        hldr = cur;
        
        if(hldr->spw != NULL && hldr->gendr == 'f')
            hldr = hldr->spw;
    }
}

rbldg::rbldg(int bid, double lat, double log, double area, mblok *mbk, cblok *cbk){
    this->bid = bid;
    this->lat = lat;
    this->log = log;
    this->area = area;
    this->h_d = NULL;
    this->mbk = mbk;
    this->cbk = cbk;
}

rbldg::~rbldg(){
    h_d = NULL;
    mbk = NULL;
    cbk = NULL;
}

void cblok::add_vcnt_rbldg(rbldg *p){
    cblok_vcnt_rbldgs.insert(pair<int, rbldg*>(p->bid, p));
}

void cblok::rmv_vcnt_rbldg(rbldg *p){
    cblok_vcnt_rbldgs.erase(p->bid);
}
