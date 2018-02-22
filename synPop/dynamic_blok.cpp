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

void mblok::add_member(agent *p){
    if(p->gendr == 'm') mblok_males.insert(pair<int, agent*>(p->aid, p));
    else mblok_fmals.insert(pair<int, agent*>(p->aid, p));
}

void mblok::rmv_hhold(hhold *p){
    if(p->mmbrs.size() > 0 || p->hldr != NULL){
        cout << "err: hhold not empty to be removed" << endl;
        exit(1);
    }
    
    cbk->add_vcnt_rbldg(p->rdg);
    mblok_ocpy_rbldgs.erase(p->rdg->bid);
    mblok_vcnt_rbldgs.insert(pair<int, rbldg*>(p->rdg->bid, p->rdg));
    
    mblok_hholds.erase(p->hid);
    p->rdg->h_d = NULL;
    p->rdg = NULL;
    
    delete p;
}

void mblok::rmv_member(agent *p){
    if(p->gendr == 'm') mblok_males.erase(p->aid);
    else mblok_fmals.erase(p->aid);
}

void mblok::add_workp(workp *p){
    mblok_workps.insert(pair<int, workp*>(p->wid, p));
    cbk->cblok_workps.insert(pair<int, workp*>(p->wid, p));
}

void mblok::add_rbldg(rbldg *p, hhold* h_hold){
    if(h_hold == NULL){
        mblok_vcnt_rbldgs.insert(pair<int, rbldg*>(p->bid, p));
        cbk->add_vcnt_rbldg(p);
    }
    else mblok_ocpy_rbldgs.insert(pair<int, rbldg*>(p->bid, p));
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
    
    double r = drand48();
    if(r <= s_p) p->margs = 's';
    else if(r <= s_p + m_p) p->margs = 'm';
    else if(r <= s_p + m_p + w_p) p->margs = 'w';
    else p->margs = 'd';
}

void mblok::adpt_chldrs(hhold *p){
    agent *pp = NULL;
    if(cbk->fmal_cbrs[0].size() > 0) pp = cbk->fmal_cbrs[0].begin()->second;
    
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
        
        p->rmv_member(cur);
        pp->h_d->add_member(cur);
    }
    
    pp->h_d->update_hhold();
}

void cblok::add_vcnt_rbldg(rbldg *p){
    cblok_vcnt_rbldgs.insert(pair<int, rbldg*>(p->bid, p));
}

void cblok::rmv_vcnt_rbldg(rbldg *p){
    cblok_vcnt_rbldgs.erase(p->bid);
}

void cblok::sim_pop(int year){
    hndl_jobs(year);
    
    //get_works(year);
    
    for(int day = 0; day < 365; ++day){
        renew_pop(year, day);
        hndl_birth(year, day);
    }
    
    hndl_marrg(year);
    hndl_divrc(year);
    hndl_migrt(year);
    hndl_hold_rupt(year);
}

void cblok::validate_pop(int year, int day){
    for(map<int, mblok*>::iterator j = mbloks.begin(); j != mbloks.end(); ++j){
        mblok *mbk = j->second;
        for(map<int, agent*>::iterator k = mbk->mblok_fmals.begin(); k != mbk->mblok_fmals.end(); ++k){
            agent *cur = k->second;
            if(cur->margs == 'm'){
                if(int(cur->age/365) < 50 && fmal_cbrs[cur->births].find(cur->aid) == fmal_cbrs[cur->births].end()){
                    cout << "year = " << year << " day = " << day << endl;
                    cout << "err: " << cur->aid << " " << int(cur->age/365) << " " << cur->margs << " not in fmal_marrd" << endl;
                    exit(1);
                }
                else if(int(cur->age/365) >= 50 && fmal_marry.find(cur->aid) == fmal_marry.end()){
                    cout << "year = " << year << " day = " << day << endl;
                    cout << "err: " << cur->aid << " " << int(cur->age/365) << " " << cur->margs << " not in fmal_marrd" << endl;
                    exit(1);
                }
            }
            
            if(cur->margs == 'm' && cur->spw == NULL){
                cout << "year = " << year << " day = " << day << endl;
                cout << "err: " << cur->aid << " " << int(cur->age/365) << " " << cur->margs << " " << mbloksIndexB[mbk->mid] << " spw = NULL" << endl;
                exit(1);
            }
        }
        
        for(map<int, hhold*>::iterator k = mbk->mblok_hholds.begin(); k != mbk->mblok_hholds.end(); ++k){
            hhold *cur = k->second;
            if(cur->rdg == NULL){
                cout << "year = " << year << " day = " << day << endl;
                cout << "err: " << cur->hid << " " << cur->size << " " << mbloksIndexB[mbk->mid] << " rbldg = NULL" << endl;
                exit(1);
            }
        }
    }
}

