//
//  sites.cpp
//  synPop
//
//  Created by Sting Xu on 5/11/17.
//  Copyright © 2017 Sting Xu. All rights reserved.
//

#include "agent.h"
#include "block.h"

hhold::hhold(int hid, int siz, agent *holder){
    this->hid = hid;
    this->siz = siz;
    hldr = holder;
}

hhold::~hhold(){
    this->rdg = NULL;
    this->hldr = NULL;
    this->mmbrs.clear();
}

void hhold::asg_bldg(rbldg *rdg){
    this->rdg = rdg;

    lat = rdg->lat;
    log = rdg->log;
    area = rdg->area;
    
    rdg->hd = this;
    rdg->cbk->rmv_vcnt_rbldg(rdg);
}

bool hhold::asg_hldr(agent *p){
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

void hhold::add_mmbr(agent *p){
    p->h_d = this;
    mmbrs.insert(pair<int, agent*>(p->aid, p));
}

void hhold::rmv_mmbr(agent *p){
    p->h_d = NULL;
    if(hldr->aid == p->aid) hldr = NULL;
    mmbrs.erase(p->aid);
}

void hhold::updt_hhold(){
    siz = (int)mmbrs.size();
}

rbldg::rbldg(int bid, double lat, double log, double area, mblok *mbk, cblok *cbk){
    this->bid = bid;
    this->lat = lat;
    this->log = log;
    this->area = area;
    this->hd = NULL;
    this->mbk = mbk;
    this->cbk = cbk;
}

rbldg::~rbldg(){
    hd = NULL;
    mbk = NULL;
    cbk = NULL;
}

void cblok::add_vcnt_rbldg(rbldg *p){
    cblok_vcnt_rbldgs.insert(pair<int, rbldg*>(p->bid, p));
}

void cblok::rmv_vcnt_rbldg(rbldg *p){
    cblok_vcnt_rbldgs.erase(p->bid);
}