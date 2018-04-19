//
//  blocks.cpp
//  synPop
//
//  Created by Sting Xu on 5/11/17.
//  Copyright © 2017 Sting Xu. All rights reserved.
//

#include "block.h"
extern int deaths;
extern int births;

void cblok::rmv_agent(agent *p){        //remove dead agent
    if(p->epids == 'e') pre_indiv.erase(p->aid);
    else if(p->epids == 'i') inf_indiv.erase(p->aid);
    else if(p->epids == 'r') rmv_indiv.erase(p->aid);
    
    hhold *h_hold = p->h_d;
    mblok *mbk = h_hold->rdg->mbk;
    
    h_hold->rmv_member(p);
    
    if(p->s_h != NULL){
        p->s_h->student.erase(p->aid);
        p->s_h = NULL;
    }
    else if(p->w_p != NULL){
        p->w_p->workers.erase(p->aid);
        p->w_p = NULL;
    }

    if(p->gendr == 'm') mbk->mblok_males.erase(p->aid);
    else mbk->mblok_fmals.erase(p->aid);
    
    if(p->margs == 'm'){
        if(p->gendr == 'f'){
            if(int(p->age/365) < 50) fmal_cbrs[p->births].erase(p->aid);
            else fmal_marry.erase(p->aid);
        }
        else{
            if(int(p->spw->age/365) < 50) fmal_cbrs[p->spw->births].erase(p->spw->aid);
            else fmal_marry.erase(p->spw->aid);
        }
        
        p->spw->margs = 'w';
        p->spw->spw = NULL;
        p->spw = NULL;
    }
    
    if(p->chdr.size() > 0){
        for(map<int, agent*>::iterator j = p->chdr.begin(); j != p->chdr.end(); ++j){
            if(p->gendr == 'm') j->second->dad = NULL;
            else j->second->mom = NULL;
        }
        p->chdr.clear();
    }
    
    if(p->dad != NULL){
        p->dad->chdr.erase(p->aid);
        p->dad = NULL;
    }
    
    if(p->mom != NULL){
        p->mom->chdr.erase(p->aid);
        p->mom = NULL;
    }
    
    delete p;
}

void cblok::renew_pop(int year, int day){
    cchild = 0;
    vector<agent*> v_1, v_2;    //v_1, death; v_2, new adult move out
    for(map<int, mblok*>::iterator j = mbloks.begin(); j != mbloks.end(); ++j){
        mblok *mbk = j->second;
        
        for(map<int, agent*>::iterator k = mbk->mblok_males.begin(); k != mbk->mblok_males.end(); ++k){
            agent *cur = k->second;
            
            if(int(cur->age/365) < 15) ++cchild;
            
            int index = int(int(cur->age/365)/5)+1;
            if(cur->age < 365) index = 0;
            if(cur->age >= 80*365) index = 17;
            
            double prob = 1 - exp(-mmortlty[index]*pow(mortality_improve, year));
            
            if(drand48() < prob) v_1.push_back(cur);
            else{
                ++cur->age;
                if(cur->age == 365*15 && drand48() < adult_relocation) v_2.push_back(cur);
            }
        }
        
        for(map<int, agent*>::iterator k = mbk->mblok_fmals.begin(); k != mbk->mblok_fmals.end(); ++k){
            agent *cur = k->second;
            
            if(int(cur->age/365) < 15) ++cchild;
            
            int index = int(int(cur->age/365)/5)+1;
            if(cur->age < 365) index = 0;
            if(cur->age >= 80*365) index = 17;
            
            double prob = 1 - exp(-fmortlty[index]*pow(mortality_improve, year));
            
            if(drand48() < prob) v_1.push_back(cur);
            else{
                ++cur->age;
                if(cur->age == 365*15 && drand48() < adult_relocation) v_2.push_back(cur);
                if(cur->bth_wind > 0) --cur->bth_wind;
                
                if(cur->age == 365*50 && cur->margs == 'm'){
                    fmal_cbrs[cur->births].erase(cur->aid);
                    fmal_marry.insert(pair<int, agent*>(cur->aid, cur));
                }
            }
        }
    }
    
    deaths += v_1.size();
    
    //remove dead agents
    while(v_1.size() > 0){
        agent *cur = v_1.back();
        hhold *h_d = cur->h_d;
        mblok *mbk = h_d->rdg->mbk;
        
        rmv_agent(cur);
        
        h_d->update_hhold();
        if(h_d->size == 0) mbk->rmv_hhold(h_d);
        v_1.pop_back();
    }
    
    //handle new adults moving out
    while(v_2.size() > 0){
        agent *cur = v_2.back();
        hhold *h_d = cur->h_d;
        mblok *mbk = h_d->rdg->mbk;
        
        hhold *new_hold = new hhold(next_hid++);
        int rnd = rand() % cblok_vcnt_rbldgs.size();
        map<int, rbldg*>::iterator it = cblok_vcnt_rbldgs.begin();
        while(rnd-- > 0) ++it;
        rbldg *rbg = it->second;
        
        new_hold->asg_rbldg(rbg);
        rbg->mbk->add_hhold(new_hold);
        
        re_location(cur, new_hold);
        new_hold->asg_holder(cur);
        new_hold->update_hhold();
        new_hold->exp = false;
        
        h_d->update_hhold();
        if(h_d->size == 0) mbk->rmv_hhold(h_d);
        
        v_2.pop_back();
    }
    
    v_1.shrink_to_fit();
    v_2.shrink_to_fit();
}

void cblok::rnd_jobs(agent *p){
    mblok *mbk = p->h_d->rdg->mbk;
    
    while(true){
        int ii = 0;
        map<int, double>::iterator j = mbk->mblok_commuting.begin();
        double rnd = drand48();
        while(rnd >= j->second && ii < mbk->mblok_commuting.size()-1) {
            rnd -= j->second;
            ++j; ++ii;
        }
        
        int mid = j->first;
        mblok *dst = mbloks[mid];
        
        if(dst->mblok_working.size() == 0) continue;
        
        ii = 0;
        rnd = drand48();
        while(rnd >= dst->mblok_working[ii]->p && ii < dst->mblok_working.size()-1){
            rnd -= dst->mblok_working[ii]->p;
            ++ii;
        }
        
        int wid = dst->mblok_working[ii]->wid;
        workp *wp = dst->mblok_workps[wid];
        
        wp->workers.insert(pair<int, agent*>(p->aid, p));
        p->w_p = wp;
        break;
    }
}

void cblok::hndl_birth(int year, int day){
    int total_birth = 0;
    for(map<int, mblok*>::iterator j = mbloks.begin(); j != mbloks.end(); ++j){
        mblok *mbk = j->second;
        for(map<int, agent*>::iterator k = mbk->mblok_fmals.begin(); k != mbk->mblok_fmals.end(); ++k){
            agent *cur = k->second;
            if(cur->age >= 15*365 && cur->age < 49*365){
                int index = int((int(cur->age/365)-15)/5);
                double prob = 1 - exp(-fertlty[year][index]);
                
                if(drand48() < prob) ++total_birth;
            }
        }
    }
    
    births += total_birth;
    
    while(total_birth > 0){
        double rnd = drand48();
        int index = 0;
        while(rnd > live_birth_order_pro[index]) rnd -= live_birth_order_pro[index++];
        
        if(fmal_cbrs[index].size() == 0) continue;
        
        vector<agent*> v[7];
        for(map<int, agent*>::iterator j = fmal_cbrs[index].begin(); j != fmal_cbrs[index].end(); ++j){
            agent *q = j->second;
            if(q->bth_wind > 0) continue;
            
            int q_age = int(q->age/365);
            
            if(q_age < 20) v[0].push_back(q);
            else if(q_age < 25) v[1].push_back(q);
            else if(q_age < 30) v[2].push_back(q);
            else if(q_age < 35) v[3].push_back(q);
            else if(q_age < 40) v[4].push_back(q);
            else if(q_age < 45) v[5].push_back(q);
            else if(q_age < 50) v[6].push_back(q);
        }
        
        double v_p[7], t_1 = 0;
        for(int i = 0; i < 7; ++i){
            if(v[i].size() > 0) v_p[i] = live_birth_age_pro[index][i];
            else v_p[i] = 0;
            
            t_1 += v_p[i];
        }
        
        if(t_1 == 0) continue;
        
        for(int i = 0; i < 7; ++i) v_p[i] /= t_1;
        
        double d_rnd = drand48();
        int ii = 0;
        for(ii = 0; ii < 7; ++ii){
            d_rnd -= v_p[ii];
            if(d_rnd <= 0 && v_p[ii] != 0) break;
        }
        
        agent *cur = v[ii][rand()%v[ii].size()];
        mblok *mbk = cur->h_d->rdg->mbk;
        
        char gender = drand48()<male_born ? 'm' : 'f';
        agent *bb = new agent(next_aid++, 0, gender, 's', cur->h_d);
        mbk->add_member(bb);
        
        cur->add_child(bb);
        cur->bth_wind = birth_window;
        bb->mom = cur;
        
        fmal_cbrs[cur->births].erase(cur->aid); ++cur->births;
        fmal_cbrs[cur->births].insert(pair<int, agent*>(cur->aid, cur));
        
        cur->spw->add_child(bb);
        bb->dad = cur->spw;
        
        cur->h_d->add_member(bb);
        cur->h_d->update_hhold();
        
        for(int i = 0; i < 7; ++i){
            v[i].clear();
            v[i].shrink_to_fit();
        }
        -- total_birth;
    }
}

void cblok::hndl_marrg(int year){
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
        
        p->spw = q;         q->spw = p;
        p->margs = 'm';     q->margs = 'm';
        
        if(int(q->age/365) < 50) fmal_cbrs[q->births].insert(pair<int, agent*>(q->aid, q));
        else fmal_marry.insert(pair<int, agent*>(q->aid, q));
        
        //relocation due to marriage
        if(p_h->size == 1 || q_h->size == 1){
            if(p_h->size == 1) re_location(q, p_h);     //(i) p & q = 1, or (ii) p = 1 & q != 1
            else re_location(p, q_h);
        }
        else{
            if(drand48() < marg_relocation){
                hhold *new_hold = new hhold(next_hid++);
                
                int rnd = rand() % cblok_vcnt_rbldgs.size();
                map<int, rbldg*>::iterator it = cblok_vcnt_rbldgs.begin();
                while(rnd-- > 0) ++it;
                rbldg *rbg = it->second;
                
                new_hold->asg_rbldg(rbg);
                rbg->mbk->add_hhold(new_hold);
                
                re_location(p, new_hold);
                re_location(q, new_hold);
                
                new_hold->asg_holder(p);
                new_hold->update_hhold();
                new_hold->exp = false;
            }
            else{
                if(p_h->size <= q_h->size) re_location(q, p_h);
                else re_location(p, q_h);
            }
        }
        
        p_h->update_hhold();
        q_h->update_hhold();
        
        if(p_h->size == 0) p_bk->rmv_hhold(p_h);
        if(q_h->size == 0) q_bk->rmv_hhold(q_h);
        
        v_1.pop_back();
        v_2.pop_back();
    }
    v_1.shrink_to_fit();
    v_2.shrink_to_fit();
}

void cblok::hndl_divrc(int year){
    vector<agent*> v_1;
    
    for(int i = 0; i < 11; ++i){
        for(map<int, agent*>::iterator j = fmal_cbrs[i].begin(); j != fmal_cbrs[i].end(); ++j){
            if(drand48() < annual_divorce) v_1.push_back(j->second);
        }
    }
    
    for(map<int, agent*>::iterator j = fmal_marry.begin(); j != fmal_marry.end(); ++j){
        if(drand48() < annual_divorce) v_1.push_back(j->second);
    }
    
    //cout << "year = " << year << " divorce = " << v_1.size() << endl;
    
    while(v_1.size() > 0){
        agent *q = v_1.back();
        agent *p = q->spw;
        
        if(int(q->age/365) < 50) fmal_cbrs[q->births].erase(q->aid);
        else fmal_marry.erase(q->aid);
        
        p->margs = 'd';     q->margs = 'd';
        p->spw = NULL;      q->spw = NULL;
        
        hhold *h_1 = p->h_d;
        hhold *h_2 = new hhold(next_hid++);
        
        int rnd = rand() % cblok_vcnt_rbldgs.size();
        map<int, rbldg*>::iterator it = cblok_vcnt_rbldgs.begin();
        while(rnd-- > 0) ++it;
        rbldg *rbg = it->second;
        
        h_2->asg_rbldg(rbg);
        rbg->mbk->add_hhold(h_2);
        
        re_location(q, h_2);    //female leave the household with children
        h_2->asg_holder(q);
        h_2->update_hhold();
        h_2->exp = false;
        
        h_1->update_hhold();
        
        v_1.pop_back();
    }
    v_1.shrink_to_fit();
}

void cblok::hndl_migrt(int year){
    int migrants = int(cpop*pop_loss[year]/1000);
    
    cpop -= migrants;
    
    int total_hhold = 0;
    for(map<int, mblok*>::iterator j = mbloks.begin(); j != mbloks.end(); ++j){
        mblok *mbk = j->second;
        total_hhold += mbk->mblok_hholds.size();
    }
    
    map<int, hhold*> vec;
    while(migrants > 0){
        int rnd = rand() % total_hhold;
        map<int, mblok*>::iterator it_1 = mbloks.begin();
        while(rnd >= it_1->second->mblok_hholds.size()){
            rnd -= it_1->second->mblok_hholds.size();
            ++it_1;
        }
        
        map<int, hhold*>::iterator it_2 = it_1->second->mblok_hholds.begin();
        while(rnd-- > 0) ++it_2;
        
        if(it_2->second->size != 0){
            vec.insert(pair<int, hhold*>(it_2->first, it_2->second));
            migrants -= it_2->second->size;
            it_2->second->size = 0;
        }
    }
    
    while(vec.size() > 0){
        hhold *h_d = vec.begin()->second;
        mblok *mbk = h_d->rdg->mbk;
        
        if(vec.size() > 1){
            while(h_d->mmbrs.size() > 0){
                agent *cur = h_d->mmbrs.begin()->second;
                rmv_agent(cur);
            }
        }
        else{
            int num = int(vec.begin()->second->mmbrs.size() + migrants);
            while(num-- > 0){
                map<int, agent*>::iterator it_3 = h_d->mmbrs.begin();
                int rnd = rand() % h_d->mmbrs.size();
                while(rnd-- > 0) ++it_3;
                
                agent *cur = it_3->second;
                rmv_agent(cur);
            }
        }
    
        h_d->update_hhold();
        if(h_d->size == 0)  mbk->rmv_hhold(h_d);
        
        vec.erase(vec.begin());
    }
}

