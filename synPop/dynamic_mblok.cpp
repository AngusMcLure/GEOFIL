//
//  blockMBlok.cpp
//  synPop
//
//  Created by Sting Xu on 7/11/17.
//  Copyright © 2017 Sting Xu. All rights reserved.
//

#include "block.h"

void mblok::add_hhold(hhold *p){
    mblok_hholds.insert(pair<int, hhold*>(p->hid, p));
}

void mblok::add_agent(agent *p){
    mblok_pop.insert(pair<int, agent*>(p->aid, p));
}

void mblok::rmv_hhold(hhold *p){
    mblok_hholds.erase(p->hid);
    p->rdg->hd = NULL;
    cbk->cblok_vcnt_rbldgs.insert(pair<int, rbldg*>(p->rdg->bid, p->rdg));
    p->rdg = NULL;
    
    delete p;
}

void mblok::rmv_agent(agent *p){
    mblok_pop.erase(p->aid);
}

void mblok::add_rbldg(rbldg *p){
    mblok_rbldgs.insert(pair<int, rbldg*>(p->bid, p));
}

void mblok::rnd_margs(agent *p){
    int index = int((int(p->age/365)-15)/5);
    index = index>10? 10 : index;
    
    double s_p, m_p, w_p, d_p;
    if(p->gendr == 'm'){
        s_p = cbk->male_single[index];
        m_p = cbk->male_married[index];
        w_p = cbk->male_widowed[index];
        d_p = cbk->male_divorce[index];
    }
    else{
        s_p = cbk->fmal_single[index];
        m_p = cbk->fmal_married[index];
        w_p = cbk->fmal_widowed[index];
        d_p = cbk->fmal_divorce[index];
    }
    
    double r = drandom();
    if(r <= s_p) p->margs = 's';
    else if(r <= s_p + m_p) p->margs = 'm';
    else if(r <= s_p + m_p + w_p) p->margs = 'w';
    else p->margs = 'd';
}

void mblok::adpt_chldrs(hhold *p){
    agent *pp = NULL;
    for(map<int, agent*>::iterator j = cbk->fmal_marrd.begin(); j != cbk->fmal_marrd.end(); ++j){
        if(j->second->chldr.size() == 0){
            pp = j->second;
            break;
        }
    }
    
    if(pp == NULL){
        cout << "err no suitable family for adopted kids in mblok::adpt_chldrs" << endl;
        exit(1);
    }
    
    if(pp->spw == NULL){
        cout << "err married female's spw == NULL in mblok::adpt_chldrs" << endl;
        exit(1);
    }
    
    while(p->mmbrs.size() > 0){
        agent *cur = p->mmbrs.begin()->second;
        
        pp->add_child(cur);
        pp->spw->add_child(cur);
        
        if(pp->gendr == 'm'){
            cur->dad = pp;
            cur->mom = pp->spw;
        }
        else{
            cur->mom = pp;
            cur->dad = pp->spw;
        }
        
        p->rmv_mmbr(cur);
        pp->h_d->add_mmbr(cur);
    }
    
    pp->h_d->updt_hhold();
}
