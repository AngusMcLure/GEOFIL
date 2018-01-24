//
//  blocks.cpp
//  synPop
//
//  Created by Sting Xu on 5/11/17.
//  Copyright © 2017 Sting Xu. All rights reserved.
//

#include "block.h"

void cblok::rmv_agent(agent *p){        //remove dead agent
    p->h_d->rmv_member(p);
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
        cur->h_d->updt_hhold();
        
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
        
        //cur->h_d->rmv_member(cur);
        
        
        vec.pop_back();
    }
    vec.shrink_to_fit();
}

void cblok::hndl_marrg(int t){
    
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

