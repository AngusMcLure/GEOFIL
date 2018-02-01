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
    this->size = size;
    hldr = holder;
    
    lat = 0;
    log = 0;
    area = 0;
    rdg = NULL;
    
    mmbrs.clear();
    
    if(holder != NULL) add_member(holder);
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
    rdg->mbk->mblok_vcnt_rbldgs.erase(rdg->bid);
    rdg->mbk->mblok_ocpy_rbldgs.insert(pair<int, rbldg*>(rdg->bid, rdg));
    rdg->cbk->rmv_vcnt_rbldg(rdg);
}

bool hhold::asg_holder(agent *p){
    if(p != NULL) hldr = p;
    else{
        //choose the most aged & married one to be new householder
        map<int, agent*>::iterator it = mmbrs.begin();
        for(map<int, agent*>::iterator k = mmbrs.begin(); k != mmbrs.end(); ++k){
            if(it->second->age < k->second->age || (it->second->margs != 'm' && k->second->margs == 'm'))
                it = k;
        }
        
        int age = it->second->age;
        if(age < 18*365) return false;          //all other members are children
        
        hldr = it->second;
    }
    
    if(hldr->spw != NULL && hldr->gendr == 'f')
        hldr = hldr->spw;
    
    return true;
}

void hhold::add_member(agent *p){
    p->h_d = this;
    mmbrs.insert(pair<int, agent*>(p->aid, p));
}

void hhold::rmv_member(agent *p){
    p->h_d = NULL;
    if(hldr != NULL && hldr->aid == p->aid){
        if(p->spw != NULL && is_member(p->spw)) hldr = p->spw;
        else hldr = NULL;
    }
    mmbrs.erase(p->aid);
}

bool hhold::is_member(agent *p){
    if(p->h_d != NULL && p->h_d->hid == hid) return true;
    return false;
}

void hhold::adopted(){
    agent *q = NULL;
    if(rdg->cbk->fmal_cbrs[0].size() > 0) q = rdg->cbk->fmal_cbrs[0].begin()->second;
    
    mblok *cur = rdg->mbk;
    mblok *mbk = q->h_d->rdg->mbk;
    
    while(mmbrs.size() > 0){
        agent *bb = mmbrs.begin()->second;
        
        q->add_child(bb);       bb->mom = q;
        q->spw->add_child(bb);  bb->dad = q->spw;
        
        rmv_member(bb);
        q->h_d->add_member(bb);
        
        cur->rmv_member(bb);
        mbk->add_member(bb);
    }
    q->h_d->update_hhold();
}

void hhold::update_hhold(){
    //choose the most aged one to be new householder
    if(hldr == NULL && mmbrs.size() > 0){
        map<int, agent*>::iterator j = mmbrs.begin();
        for(map<int, agent*>::iterator k = mmbrs.begin(); k != mmbrs.end(); ++k){
            if(j->second->age < k->second->age) j = k;
        }
        
        agent *cur = j->second;
        int age = cur->age;
        
        if(age < 15*365) adopted();           //all other members are children
        else{
            hldr = cur;
            
            if(hldr->spw != NULL && hldr->gendr == 'f')
                hldr = hldr->spw;
        }
    }
    
    size = int(mmbrs.size());
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

void cblok::re_location(agent *p, hhold *h_hold){
    hhold *p_h = p->h_d;
    mblok *p_bk = p_h->rdg->mbk;
    mblok *mbk = h_hold->rdg->mbk;
    
    p_h->rmv_member(p);
    h_hold->add_member(p);
    
    p_bk->rmv_member(p);
    mbk->add_member(p);
    
    for(map<int, agent*>::iterator j = p->chdr.begin(); j != p->chdr.end(); ++j){
        agent *cur = j->second;
        if(p_h->is_member(cur)){
            int age = int(cur->age/365);
            
            bool c_1 = age < 15 && p_h->is_member(cur); //children
            bool c_2 = cur->chdr.size() == 0 && cur->margs != 'm'; //not married with no children
            if(c_1 || c_2){
                p_h->rmv_member(cur);
                h_hold->add_member(cur);
                
                p_bk->rmv_member(cur);
                mbk->add_member(cur);
            }
        }
    }
}
