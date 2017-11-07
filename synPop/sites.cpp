//
//  sites.cpp
//  synPop
//
//  Created by Sting Xu on 5/11/17.
//  Copyright © 2017 Sting Xu. All rights reserved.
//

#include "sites.h"

hhold::hhold(int hid, int siz, char typ){
    this->hid = hid;
    this->siz = siz;
    this->typ = typ;
    
    hldr = NULL;
}

hhold::~hhold(){
    this->rdg = NULL;
    this->hldr = NULL;
    this->mmbrs.clear();
}

void hhold::add_bldg(rbldg *rdg){
    this->rdg = rdg;

    lat = rdg->lat;
    log = rdg->log;
    area = rdg->area;
    
    rdg->hd = this;
}

bool hhold::add_hldr(agent *p){
    if(p != NULL) hldr = p;
    else{
        //choose the most aged & married one to be new householder
        map<int, agent*>::iterator it = mmbrs.begin();
        for(map<int, agent*>::iterator k = mmbrs.begin(); k != mmbrs.end(); ++k){
            if(k->second->role == '1'){
                it = k;
                break;
            }
            if(it->second->age < k->second->age || (it->second->margs != 'M' && k->second->margs == 'M'))
                it = k;
        }
        
        int age = it->second->age;
        if(age < 18*365) return false;          //all other members are children
        
        hldr = it->second;
    }
    return true;
}

void hhold::add_mmbr(agent *p, char role){
    p->hd = this;
    p->role = role;
    
    if(role == '0') hldr = p;
    mmbrs.insert(pair<int, agent*>(p->aid, p));
}

void hhold::rmv_mmbr(agent *p){
    p->hd = NULL;
    if(p->role == '0') hldr = NULL;
    mmbrs.erase(p->aid);
}

bool hhold::updt_hhold(){
    siz = (int)mmbrs.size();
    if(siz != 0){
        if(hldr == NULL && !add_hldr(NULL)) return false;
        
        if(hldr->spw != NULL && hldr->gendr == 'F')
            hldr = hldr->spw;
        
        hldr->role = '0';
        if(hldr->spw != NULL) hldr->spw->role = '1';
        
        for(map<int, agent*>::iterator k = hldr->chldr.begin(); k != hldr->chldr.end(); ++k){
            if(k->second->hd->hid == hid){
                k->second->role = '2';
            }
        }
        
        if(typ != '4'){
            if(mmbrs.size() == 1) typ = '3';
            else{
                if(hldr->margs != 'M'){
                    if(hldr->gendr == 'F') typ = '1';
                    else typ = '2';
                }
                else typ = '0';
            }
        }
    }
    return true;
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
