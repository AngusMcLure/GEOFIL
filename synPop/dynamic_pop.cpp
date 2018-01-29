//
//  blocks.cpp
//  synPop
//
//  Created by Sting Xu on 5/11/17.
//  Copyright © 2017 Sting Xu. All rights reserved.
//

#include "block.h"

void cblok::rmv_agent(agent *p){        //remove dead agent
    hhold *h_hold = p->h_d;
    mblok *mbk = h_hold->rdg->mbk;
    
    h_hold->rmv_member(p);
    h_hold->update_hhold();
    
    if(h_hold->size == 0) mbk->rmv_hhold(h_hold);
    
    if(p->works == 's'){
        p->s_h->student.erase(p->aid);
        p->s_h = NULL;
    }
    else if(p->works == 'w'){
        if(p->w_p != NULL){
            p->w_p->employee.erase(p->aid);
            p->w_p = NULL;
        }
        
        if(p->s_h != NULL){
            p->s_h->teacher.erase(p->aid);
            p->s_h = NULL;
        }
    }

    if(p->gendr == 'm') mbk->mblok_males.erase(p->aid);
    else mbk->mblok_fmals.erase(p->aid);
    
    if(p->gendr == 'f' && p->margs == 'm') fmal_marrd.erase(p->aid);
    
    if(p->chldr.size() > 0){
        for(map<int, agent*>::iterator j = p->chldr.begin(); j != p->chldr.end(); ++j){
            if(p->gendr == 'm') j->second->dad = NULL;
            else j->second->mom = NULL;
        }
        p->chldr.clear();
    }
    
    if(p->dad != NULL){
        p->dad->chldr.erase(p->aid);
        p->dad = NULL;
    }
    
    if(p->mom != NULL){
        p->mom->chldr.erase(p->aid);
        p->mom = NULL;
    }
    
    if(p->spw != NULL){
        p->spw->spw = NULL;
        p->spw->margs = 'w';
    }
    
    p->epids = 'd';
    delete p;
}

void cblok::hndl_birth(int t){
    int year = int(t/365);
    
    int total_birth = 0;
    for(map<int, mblok*>::iterator j = mbloks.begin(); j != mbloks.end(); ++j){
        mblok *mbk = j->second;
        for(map<int, agent*>::iterator k = mbk->mblok_fmals.begin(); k != mbk->mblok_fmals.end(); ++k){
            agent *cur = k->second;
            if(cur->age >= 15*365 && cur->age < 45*365){
                int index = int((int(cur->age/365)-15)/5);
                double prob = 1 - exp(-fertlty[year][index]);
                
                if(drand48() < prob) ++total_birth;
            }
        }
    }
    
    vector<agent*> vec;
    while(vec.size() < total_birth){
        for(map<int, agent*>::iterator j = fmal_marrd.begin(); j != fmal_marrd.end(); ++j){
            agent *cur = j->second;
            if(cur->age < 45*365 && cur->chldr.size() < 10 && cur->bth_wind == 0){
                int index = int((int(cur->age/365)-15)/5);
                double prob = 1 - exp(-fertlty[year][index]);
                
                if(drand48() < prob){
                    vec.push_back(cur);
                    cur->bth_wind = birth_window;
                    
                    if(vec.size() == total_birth) break;
                }
            }
        }
    }
    
    while(vec.size() > 0){
        agent *cur = vec.back();
        mblok *mbk = cur->h_d->rdg->mbk;
        
        char gender = drand48()<male_born ? 'M' : 'F';
        agent *bb = new agent(next_aid++, 0, gender, 's', cur->h_d);
        mbk->add_member(bb);
        
        cur->add_child(bb);
        bb->mom = cur;
        
        cur->spw->add_child(bb);
        bb->dad = cur->spw;
        
        cur->h_d->add_member(bb);
        cur->h_d->update_hhold();
        
        vec.pop_back();
    }
    vec.shrink_to_fit();
}

void cblok::hndl_death(int t){
    /*if(gendr == 'F') cbk->fmal_marrd.erase(aid);
    else cbk->fmal_marrd.erase(spw->aid);*/
    int year = int(t/365);
    
    vector<agent*> vec;
    for(map<int, mblok*>::iterator j = mbloks.begin(); j != mbloks.end(); ++j){
        mblok *mbk = j->second;
        for(map<int, agent*>::iterator k = mbk->mblok_males.begin(); k != mbk->mblok_males.end(); ++k){
            agent *cur = k->second;
            
            int index = int(int(cur->age/365)/5)+1;
            if(cur->age < 365) index = 0;
            if(cur->age >= 80*365) index = 17;
            
            double prob = 1 - exp(-mmortlty[index]*pow(mortality_improve, year));
            
            if(drand48() < prob) vec.push_back(cur);
        }
        
        for(map<int, agent*>::iterator k = mbk->mblok_fmals.begin(); k != mbk->mblok_fmals.end(); ++k){
            agent *cur = k->second;
            
            int index = int(int(cur->age/365)/5)+1;
            if(cur->age < 365) index = 0;
            if(cur->age >= 80*365) index = 17;
            
            double prob = 1 - exp(-fmortlty[index]*pow(mortality_improve, year));
            
            if(drand48() < prob) vec.push_back(cur);
        }
    }
    
    while(vec.size() > 0){
        agent *cur = vec.back();
        rmv_agent(cur);
        vec.pop_back();
    }
    vec.shrink_to_fit();
}

void cblok::hndl_marrg(int t){
    vector<agent*> v_1, v_2;
    for(map<int, mblok*>::iterator j = mbloks.begin(); j != mbloks.end(); ++j){
        mblok *mbk = j->second;
        for(map<int, agent*>::iterator k = mbk->mblok_males.begin(); k != mbk->mblok_males.end(); ++k){
            agent *cur = k->second;
            if(cur->age < 365*15) continue;
            
            int index = int(cur->age/365-15);
            index = index>54 ? 54 : index;
            if(cur->margs != 'm' && drand48() < male_mrg_prob[index])
                v_1.push_back(cur);
        }
        
        for(map<int, agent*>::iterator k = mbk->mblok_fmals.begin(); k != mbk->mblok_fmals.end(); ++k){
            agent *cur = k->second;
            if(cur->age < 365*15) continue;
            
            int index = int(cur->age/365-15);
            index = index>54 ? 54 : index;
            if(cur->margs != 'm' && drand48() < fmal_mrg_prob[index])
                v_2.push_back(cur);
        }
    }
    
    struct _comp_age{
        bool operator() (const agent *p, const agent *q){ return (p->age < q->age);}
    } _younger;
    
    stable_sort(v_1.begin(), v_1.end(), _younger);
    stable_sort(v_2.begin(), v_2.end(), _younger);
    
    while(v_1.size() > v_2.size()) v_1.erase(v_1.begin());
    while(v_1.size() < v_2.size()) v_2.erase(v_2.begin());
    
    while(v_1.size() > 0){
        agent *p = v_1.back();
        agent *q = v_2.back();
        
        hhold *p_h = p->h_d;
        hhold *q_h = q->h_d;
        
        rbldg *p_b = p_h->rdg;
        rbldg *q_b = q_h->rdg;
        
        mblok *p_bk = p_b->mbk;
        mblok *q_bk = q_b->mbk;
        
        //relocation due to marriage
        if(p_h->size == 1 || q_h->size == 1){
            if(p_h->size == 1){     //(i) p & q = 1, or (ii) p = 1 & q != 1
                re_location(q, p_h);
                if(q_h->size == 0) q_bk->rmv_hhold(q_h);
            }
            else{
                re_location(p, q_h);
                if(p_h->size == 0) p_bk->rmv_hhold(p_h);
            }
        }
        else{
            
        }
        
        v_1.pop_back();
        v_2.pop_back();
    }
    v_1.shrink_to_fit();
    v_2.shrink_to_fit();
}

void cblok::hndl_divrc(int t){
    /*if(gendr == 'F') cbk->fmal_marrd.erase(aid);
     else cbk->fmal_marrd.erase(spw->aid);*/
}

void cblok::hndl_ploss(int t){
    /*if(gendr == 'F') cbk->fmal_marrd.erase(aid);
     else cbk->fmal_marrd.erase(spw->aid);*/
}

void cblok::hndl_mvout(int t){
    
}

void cblok::hndl_mvgin(int t){
    
}

void cblok::hndl_hldfrc(int t){
    
}

