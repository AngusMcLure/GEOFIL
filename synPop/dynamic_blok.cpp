//
//  blockMBlok.cpp
//  synPop
//
//  Created by Sting Xu on 7/11/17.
//  Copyright © 2017 Sting Xu. All rights reserved.
//

#include "block.h"

extern double max_prv;

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

void cblok::hndl_jobs(int year){
    if(year % 5 == 0) radt_model('r');
    
    if(year == 0){
        struct _comp_w_node{
            bool operator()(const mblok::w_node *p, const mblok::w_node *q){return p->p > q->p; }
        } _larger;
        
        for(map<int, mblok*>::iterator j = mbloks.begin(); j != mbloks.end(); ++j){
            mblok *mbk = j->second;
            
            double total = 0;
            for(map<int, workp*>::iterator k = mbk->mblok_workps.begin(); k != mbk->mblok_workps.end(); ++k){
                workp *wp = k->second;
                total += wp->area;
                mbk->mblok_working.push_back(new mblok::w_node(wp->wid, wp->area));
            }
            
            for(int i = 0; i < mbk->mblok_working.size(); ++i){
                mbk->mblok_working[i]->p /= total;
            }
            
            stable_sort(mbk->mblok_working.begin(), mbk->mblok_working.end(), _larger);
        }
    }
    
    cpop = 0;
    labor_force = 0;
    cchild = 0;
    for(map<int, mblok*>::iterator j = mbloks.begin(); j != mbloks.end(); ++j){
        mblok *mbk = j->second;
        mbk->labors = 0;
        
        cpop += mbk->mblok_males.size() + mbk->mblok_fmals.size();
        
        for(map<int, agent*>::iterator k = mbk->mblok_males.begin(); k != mbk->mblok_males.end(); ++k){
            agent *cur = k->second;
            
            int age = int(cur->age/365);
            if(age < 70 && age >= 15) mbk->labors += LFPR_by_age[0][age-15];          //age 15-69
            
            if(age < 15) ++cchild;
            
            double p_1 = 0, p_2 = 0;
            if(age > 15 && age <= 70) p_1 = LFPR_by_age[0][age-16];
            if(age >= 15 && age < 70) p_2 = LFPR_by_age[0][age-15];
            
            if(age >= 15 && age <= 70){
                if(year > 0){
                    if(p_2 > p_1 && cur->w_p == NULL && drand48()<(p_2-p_1)/(1-p_1)){
                        rnd_jobs(cur);
                        
                        if(cur->s_h != NULL){           //remove from students
                            cur->s_h->student.erase(cur->aid);
                            cur->s_h = NULL;
                        }
                    }
                }
                else{
                    if(cur->w_p == NULL && drand48() < p_2){
                        rnd_jobs(cur);
                        
                        if(cur->s_h != NULL){           //remove from students
                            cur->s_h->student.erase(cur->aid);
                            cur->s_h = NULL;
                        }
                    }
                }
                
                if(p_2 < p_1 && cur->w_p != NULL){
                    if(drand48() < (p_1-p_2)/(1-p_1)){
                        cur->w_p->workers.erase(cur->aid);
                        cur->w_p = NULL;
                    }
                }
            }
        }
        
        for(map<int, agent*>::iterator k = mbk->mblok_fmals.begin(); k != mbk->mblok_fmals.end(); ++k){
            agent *cur = k->second;
            
            int age = int(cur->age/365);
            if(age < 70 && age >= 15) mbk->labors += LFPR_by_age[1][age-15];          //age 15-69
            
            if(age < 15) ++cchild;
            
            double p_1 = 0, p_2 = 0;
            if(age > 15 && age <= 70) p_1 = LFPR_by_age[1][age-16];
            if(age >= 15 && age < 70) p_2 = LFPR_by_age[1][age-15];
            
            if(age >= 15 && age <= 70){
                if(year > 0){
                    if(p_2 > p_1 && cur->w_p == NULL && drand48()<(p_2-p_1)/(1-p_1)){
                        rnd_jobs(cur);
                        
                        if(cur->s_h != NULL){           //remove from students
                            cur->s_h->student.erase(cur->aid);
                            cur->s_h = NULL;
                        }
                    }
                }
                else{
                    if(cur->w_p == NULL && drand48() < p_2){
                        rnd_jobs(cur);
                        
                        if(cur->s_h != NULL){           //remove from students
                            cur->s_h->student.erase(cur->aid);
                            cur->s_h = NULL;
                        }
                    }
                }
                
                if(p_2 < p_1 && cur->w_p != NULL){
                    if(drand48() < (p_1-p_2)/(1-p_1)){
                        cur->w_p->workers.erase(cur->aid);
                        cur->w_p = NULL;
                    }
                }
            }
        }
        
        labor_force += mbk->labors;
    }
}

void cblok::hndl_schol(int year){
    for(map<int, mblok*>::iterator j = mbloks.begin(); j != mbloks.end(); ++j){
        mblok *mbk = j->second;
        
        for(map<int, agent*>::iterator k = mbk->mblok_males.begin(); k != mbk->mblok_males.end(); ++k){
            agent *cur = k->second;
            int age = int(cur->age/365);
            
            if(age >= 20 && cur->s_h != NULL){
                cur->s_h->student.erase(cur->aid);
                cur->s_h = NULL;
            }
            else if(age >= 18 && age <= 19 && cur->w_p == NULL){
                if(age == 18 || cur->s_h == NULL) select_schol(cur, 'C');   //enroll college || no schol
            }
            else if(age >= 14 && age <= 17 && cur->w_p == NULL){
                if(age == 14 || cur->s_h == NULL) select_schol(cur, 'H');   //enroll high || no shol
            }
            else if(age >= 6 && age <= 13){
                if(cur->s_h == NULL) select_schol(cur, 'E');                //enroll elementary || no schol
            }
        }
        
        for(map<int, agent*>::iterator k = mbk->mblok_fmals.begin(); k != mbk->mblok_fmals.end(); ++k){
            agent *cur = k->second;
            int age = int(cur->age/365);
            
            if(age >= 20 && cur->s_h != NULL){
                cur->s_h->student.erase(cur->aid);
                cur->s_h = NULL;
            }
            else if(age >= 18 && age <= 19 && cur->w_p == NULL){
                if(age == 18 || cur->s_h == NULL) select_schol(cur, 'C');   //enroll college || no schol
            }
            else if(age >= 14 && age <= 17 && cur->w_p == NULL){
                if(age == 14 || cur->s_h == NULL) select_schol(cur, 'H');   //enroll high || no shol
            }
            else if(age >= 6 && age <= 13){
                if(cur->s_h == NULL) select_schol(cur, 'E');                //enroll elementary || no schol
            }
        }
    }
}

void cblok::select_schol(agent *p, char level){
    hhold *hd = p->h_d;
    rbldg *rb = hd->rdg;
    schol *sh = NULL;
    
    map<int, schol*>::iterator j;
    if(level == 'E'){
        j = cblok_e_schols.begin();
        sh = j->second;
        
        double d_1 = pow(rb->lat - sh->lat, 2) + pow(rb->log - sh->log, 2);
        while(++j != cblok_e_schols.end()){
            double d_2 = pow(rb->lat - j->second->lat, 2) + pow(rb->log - j->second->log, 2);
            
            if(d_2 < d_1){
                d_1 = d_2;
                sh = j->second;
            }
        }
    }
    else if(level == 'H'){
        j = cblok_h_schols.begin();
        sh = j->second;
        
        double d_1 = pow(rb->lat - j->second->lat, 2) + pow(rb->log - j->second->log, 2);
        while(++j != cblok_h_schols.end()){
            double d_2 = pow(rb->lat - j->second->lat, 2) + pow(rb->log - j->second->log, 2);
            
            if(d_2 < d_1){
                d_1 = d_2;
                sh = j->second;
            }
        }
    }
    else if(level == 'C'){
        j = cblok_c_schols.begin();
        sh = j->second;
        
        double d_1 = pow(rb->lat - j->second->lat, 2) + pow(rb->log - j->second->log, 2);
        while(++j != cblok_c_schols.end()){
            double d_2 = pow(rb->lat - j->second->lat, 2) + pow(rb->log - j->second->log, 2);
            
            if(d_2 < d_1){
                d_1 = d_2;
                sh = j->second;
            }
        }
    }
    
    if(p->s_h != NULL) p->s_h->student.erase(p->aid);
    
    p->s_h = sh;
    sh->student.insert(pair<int, agent*>(p->aid, p));
}

void cblok::calc_risk(int year, int day){
    for(map<int, agent*>::iterator j = inf_indiv.begin(); j != inf_indiv.end(); ++j){
        agent *cur = j->second;
        hhold *hd = cur->h_d;
        rbldg *rb = cur->h_d->rdg;
        schol *sh = cur->s_h;
        workp *wp = cur->w_p;
        
        int sum = 0;
        for(map<int, agent*>::iterator k = hd->mmbrs.begin(); k != hd->mmbrs.end(); ++k){
            if(k->second->s_h == NULL && k->second->w_p == NULL) ++sum;
        }
        
        inf_rbldg_night.insert(pair<int, rbldg*>(rb->bid, rb));
        rb->night_p += 1.0/(double)hd->mmbrs.size();
        if(sh != NULL){
            sh->day_p += 1.0/(double)sh->student.size();
            inf_schol.insert(pair<int, schol*>(sh->sid, sh));
        }
        if(wp != NULL){
            wp->day_p += 1.0/(double)wp->workers.size();
            inf_workp.insert(pair<int, workp*>(wp->wid, wp));
        }
        if(sh == NULL && wp == NULL){
            rb->day_p += 1.0/(double)sum;
            inf_rbldg_day.insert(pair<int, rbldg*>(rb->bid, rb));
        }
    }
    
    //risk at day-time
    risk_loc_day(year, day);
    
    //calculate risk for individuals at risk rbldg
    for(map<int, rbldg*>::iterator j = risk_rbldg.begin(); j != risk_rbldg.end(); ++j){
        rbldg *r_1 = j->second;
        hhold *hd = r_1->h_d;
        
        vector<agent*> r_indiv;
        r_indiv.shrink_to_fit();
        for(map<int, agent*>::iterator k = hd->mmbrs.begin(); k != hd->mmbrs.end(); ++k){
            agent *p = k->second;
            if(p->epids == 's' && p->s_h == NULL && p->w_p == NULL)
                r_indiv.push_back(p);
        }
        if(r_indiv.size() == 0) continue;
        
        //itself
        r_1->t_f = 0;
        r_1->t_f += 1.0;
        
        //r_neigbors
        for(int i = 0; i < r_1->r_neigh.size(); ++i){
            rbldg *r_2 = r_1->r_neigh[i];
            if(r_2->h_d == NULL) continue;
            r_1->t_f += 1 - r_1->r_neigh_d[i]/r_r;
        }
        
        //s_neigbors
        for(int i = 0; i < r_1->s_neigh.size(); ++i){
            schol *sh = r_1->s_neigh[i];
            if(sh->student.size() == 0) continue;
            r_1->t_f += 1 - r_1->s_neigh_d[i]/r_r;
        }
        
        //w_neigbors
        for(int i = 0; i < r_1->w_neigh.size(); ++i){
            workp *wp = r_1->w_neigh[i];
            if(wp->workers.size() == 0) continue;
            r_1->t_f += 1 - r_1->w_neigh_d[i]/r_r;
        }
        
        //calculate prevalence
        double prv = 0;
        double f = 1.0 / r_1->t_f;
        prv += f * r_1->day_p;
        for(int i = 0; i < r_1->r_neigh.size(); ++i){
            rbldg *r_2 = r_1->r_neigh[i];
            if(r_2->h_d == NULL) continue;
            
            f = (1 - r_1->r_neigh_d[i]/r_r) / r_1->t_f;
            prv += f * r_2->day_p;
        }
        
        for(int i = 0; i < r_1->s_neigh.size(); ++i){
            schol *sh = r_1->s_neigh[i];
            if(sh->student.size() == 0) continue;
            f = (1 - r_1->s_neigh_d[i]/r_r) / r_1->t_f;
            prv += f * sh->day_p;
        }
        
        for(int i = 0; i < r_1->w_neigh.size(); ++i){
            workp *wp = r_1->w_neigh[i];
            if(wp->workers.size() == 0) continue;
            f = (1 - r_1->w_neigh_d[i]/r_r) / r_1->t_f;
            prv += f * wp->day_p;
        }
        
        prv *= (r_i * s_l3);
        if(max_prv < prv) max_prv = prv;
        
        for(int i = 0; i < r_indiv.size(); ++i){
            agent *p = r_indiv[i];
            
            double c = 1;
            int age = int(p->age/365);
            if(age <= 15)
                c = exposure_by_age[age];
            
            p->calc_risk(prv, 'd', c);
            p->renew_epidemics();
            
            if(p->epids == 'e') pre_indiv.insert(pair<int, agent*>(p->aid, p));
        }
        
        r_indiv.clear();
        r_indiv.shrink_to_fit();
    }
    
    for(map<int, schol*>::iterator j = risk_schol.begin(); j != risk_schol.end(); ++j){
        schol *sh = j->second;
        
        //itself
        sh->t_f = 0;
        sh->t_f += 1.0;
        
        //r_neigbors
        for(int i = 0; i < sh->r_neigh.size(); ++i){
            rbldg *r_2 = sh->r_neigh[i];
            if(r_2->h_d == NULL) continue;
            sh->t_f += 1 - sh->r_neigh_d[i]/r_r;
        }
        
        //s_neigbors
        for(int i = 0; i < sh->s_neigh.size(); ++i){
            schol *s_2 = sh->s_neigh[i];
            if(s_2->student.size() == 0) continue;
            sh->t_f += 1 - sh->s_neigh_d[i]/r_r;
        }
        
        //w_neigbors
        for(int i = 0; i < sh->w_neigh.size(); ++i){
            workp *wp = sh->w_neigh[i];
            if(wp->workers.size() == 0) continue;
            sh->t_f += 1 - sh->w_neigh_d[i]/r_r;
        }
        
        //calculate prevalence
        double prv = 0;
        double f = 1.0 / sh->t_f;
        prv += f * sh->day_p;
        for(int i = 0; i < sh->r_neigh.size(); ++i){
            rbldg *r_2 = sh->r_neigh[i];
            if(r_2->h_d == NULL) continue;
            
            f = (1 - sh->r_neigh_d[i]/r_r) / sh->t_f;
            prv += f * r_2->day_p;
        }
        
        for(int i = 0; i < sh->s_neigh.size(); ++i){
            schol *s_2 = sh->s_neigh[i];
            if(s_2->student.size() == 0) continue;
            f = (1 - sh->s_neigh_d[i]/r_r) / sh->t_f;
            prv += f * s_2->day_p;
        }
        
        for(int i = 0; i < sh->w_neigh.size(); ++i){
            workp *wp = sh->w_neigh[i];
            if(wp->workers.size() == 0) continue;
            f = (1 - sh->w_neigh_d[i]/r_r) / sh->t_f;
            prv += f * wp->day_p;
        }
        
        prv *= (r_i * s_l3);
        if(max_prv < prv) max_prv = prv;
        
        for(map<int, agent*>::iterator k = sh->student.begin(); k != sh->student.end(); ++k){
            agent *p = k->second;
            if(p->epids != 's') continue;
            
            double c = 1;
            int age = int(p->age/365);
            if(age <= 15)
                c = exposure_by_age[age];
            
            p->calc_risk(prv, 'd', c);
            p->renew_epidemics();
            
            if(p->epids == 'e') pre_indiv.insert(pair<int, agent*>(p->aid, p));
        }
    }
    
    for(map<int, workp*>::iterator j = risk_workp.begin(); j != risk_workp.end(); ++j){
        workp *wp = j->second;
        
        //itself
        wp->t_f = 0;
        wp->t_f += 1.0;
        
        //r_neigbors
        for(int i = 0; i < wp->r_neigh.size(); ++i){
            rbldg *r_2 = wp->r_neigh[i];
            if(r_2->h_d == NULL) continue;
            wp->t_f += 1 - wp->r_neigh_d[i]/r_r;
        }
        
        //s_neigbors
        for(int i = 0; i < wp->s_neigh.size(); ++i){
            schol *s_2 = wp->s_neigh[i];
            if(s_2->student.size() == 0) continue;
            wp->t_f += 1 - wp->s_neigh_d[i]/r_r;
        }
        
        //w_neigbors
        for(int i = 0; i < wp->w_neigh.size(); ++i){
            workp *w_2 = wp->w_neigh[i];
            if(w_2->workers.size() == 0) continue;
            wp->t_f += 1 - wp->w_neigh_d[i]/r_r;
        }
        
        //calculate prevalence
        double prv = 0;
        double f = 1.0 / wp->t_f;
        prv += f * wp->day_p;
        for(int i = 0; i < wp->r_neigh.size(); ++i){
            rbldg *r_2 = wp->r_neigh[i];
            if(r_2->h_d == NULL) continue;
            
            f = (1 - wp->r_neigh_d[i]/r_r) / wp->t_f;
            prv += f * r_2->day_p;
        }
        
        for(int i = 0; i < wp->s_neigh.size(); ++i){
            schol *s_2 = wp->s_neigh[i];
            if(s_2->student.size() == 0) continue;
            f = (1 - wp->s_neigh_d[i]/r_r) / wp->t_f;
            prv += f * s_2->day_p;
        }
        
        for(int i = 0; i < wp->w_neigh.size(); ++i){
            workp *w_2 = wp->w_neigh[i];
            if(w_2->workers.size() == 0) continue;
            f = (1 - wp->w_neigh_d[i]/r_r) / wp->t_f;
            prv += f * w_2->day_p;
        }
        
        prv *= (r_i * s_l3);
        if(max_prv < prv) max_prv = prv;
        
        for(map<int, agent*>::iterator k = wp->workers.begin(); k != wp->workers.end(); ++k){
            agent *p = k->second;
            if(p->epids != 's') continue;
            
            double c = 1;
            int age = int(p->age/365);
            if(age <= 15)
                c = exposure_by_age[age];
            
            p->calc_risk(prv, 'd', c);
            p->renew_epidemics();
            
            if(p->epids == 'e') pre_indiv.insert(pair<int, agent*>(p->aid, p));
        }
    }
    
    //risk at night-time
    risk_loc_night(year, day);
    
    //calculate risk for individuals at risk rbldg
    for(map<int, rbldg*>::iterator j = risk_rbldg.begin(); j != risk_rbldg.end(); ++j){
        rbldg *r_1 = j->second;
        hhold *hd = r_1->h_d;
        
        //itself
        r_1->t_f = 0;
        r_1->t_f += 1.0;
        
        //r_neigbors
        for(int i = 0; i < r_1->r_neigh.size(); ++i){
            rbldg *r_2 = r_1->r_neigh[i];
            if(r_2->h_d == NULL) continue;
            r_1->t_f += 1 - r_1->r_neigh_d[i]/r_r;
        }
        
        //calculate prevalence
        double prv = 0;
        double f = 1.0 / r_1->t_f;
        prv += f * r_1->night_p;
        for(int i = 0; i < r_1->r_neigh.size(); ++i){
            rbldg *r_2 = r_1->r_neigh[i];
            if(r_2->h_d == NULL) continue;
            
            f = (1 - r_1->r_neigh_d[i]/r_r) / r_1->t_f;
            prv += f * r_2->night_p;
        }
        
        prv *= (r_i * s_l3);
        if(max_prv < prv) max_prv = prv;
        
        for(map<int, agent*>::iterator k = hd->mmbrs.begin(); k != hd->mmbrs.end(); ++k){
            agent *p = k->second;
            if(p->epids != 's') continue;
            
            double c = 1;
            int age = int(p->age/365);
            if(age <= 15)
                c = exposure_by_age[age];
            
            p->calc_risk(prv, 'n', c);
            p->renew_epidemics();
            
            if(p->epids == 'e') pre_indiv.insert(pair<int, agent*>(p->aid, p));
        }
    }
    
    //reset
    for(map<int, rbldg*>::iterator j = inf_rbldg_night.begin(); j != inf_rbldg_night.end(); ++j){
        rbldg *rb = j->second;
        rb->day_p = 0;
        rb->night_p = 0;
    }
    
    for(map<int, rbldg*>::iterator j = inf_rbldg_day.begin(); j != inf_rbldg_day.end(); ++j){
        rbldg *rb = j->second;
        rb->day_p = 0;
        rb->night_p = 0;
    }
    
    for(map<int, schol*>::iterator j = inf_schol.begin(); j != inf_schol.end(); ++j){
        schol *sh = j->second;
        sh->day_p = 0;
    }
    
    for(map<int, workp*>::iterator j = inf_workp.begin(); j != inf_workp.end(); ++j){
        workp *wp = j->second;
        wp->day_p = 0;
    }
    inf_rbldg_day.clear();
    inf_rbldg_night.clear();
    inf_schol.clear();
    inf_workp.clear();
}

void cblok::risk_loc_day(int year, int day){
    risk_rbldg.clear();
    risk_schol.clear();
    risk_workp.clear();
    
    for(map<int, rbldg*>::iterator j = inf_rbldg_day.begin(); j != inf_rbldg_day.end(); ++j){
        rbldg *r_1 = j->second;
        risk_rbldg.insert(pair<int, rbldg*>(r_1->bid, r_1));
        
        for(int i = 0; i < r_1->r_neigh.size(); ++i){       //add household
            rbldg *r_2 = r_1->r_neigh[i];
            if(r_2->h_d == NULL) continue;
            risk_rbldg.insert(pair<int, rbldg*>(r_2->bid, r_2));
        }
        
        for(int i = 0; i < r_1->s_neigh.size(); ++i){       //add school
            schol *sh = r_1->s_neigh[i];
            if(sh->student.size() == 0) continue;
            risk_schol.insert(pair<int, schol*>(sh->sid, sh));
        }
        
        for(int i = 0; i < r_1->w_neigh.size(); ++i){       //add workplace
            workp *wp = r_1->w_neigh[i];
            if(wp->workers.size() == 0) continue;
            risk_workp.insert(pair<int, workp*>(wp->wid, wp));
        }
    }
    
    for(map<int, schol*>::iterator j = inf_schol.begin(); j != inf_schol.end(); ++j){
        schol *s_1 = j->second;
        risk_schol.insert(pair<int, schol*>(s_1->sid, s_1));
        
        for(int i = 0; i < s_1->r_neigh.size(); ++i){       //add household
            rbldg *r_2 = s_1->r_neigh[i];
            if(r_2->h_d == NULL) continue;
            risk_rbldg.insert(pair<int, rbldg*>(r_2->bid, r_2));
        }
        
        for(int i = 0; i < s_1->s_neigh.size(); ++i){       //add school
            schol *s_2 = s_1->s_neigh[i];
            if(s_2->student.size() == 0) continue;
            risk_schol.insert(pair<int, schol*>(s_2->sid, s_2));
        }
        
        for(int i = 0; i < s_1->w_neigh.size(); ++i){       //add workplace
            workp *wp = s_1->w_neigh[i];
            if(wp->workers.size() == 0) continue;
            risk_workp.insert(pair<int, workp*>(wp->wid, wp));
        }
    }
    
    for(map<int, workp*>::iterator j = inf_workp.begin(); j != inf_workp.end(); ++j){
        workp *w_1 = j->second;
        risk_workp.insert(pair<int, workp*>(w_1->wid, w_1));
        
        for(int i = 0; i < w_1->r_neigh.size(); ++i){       //add household
            rbldg *r_2 = w_1->r_neigh[i];
            if(r_2->h_d == NULL) continue;
            risk_rbldg.insert(pair<int, rbldg*>(r_2->bid, r_2));
        }
        
        for(int i = 0; i < w_1->s_neigh.size(); ++i){       //add school
            schol *sh = w_1->s_neigh[i];
            if(sh->student.size() == 0) continue;
            risk_schol.insert(pair<int, schol*>(sh->sid, sh));
        }
        
        for(int i = 0; i < w_1->w_neigh.size(); ++i){       //add workplace
            workp *wp = w_1->w_neigh[i];
            if(wp->workers.size() == 0) continue;
            risk_workp.insert(pair<int, workp*>(wp->wid, wp));
        }
    }
}

void cblok::risk_loc_night(int year, int day){
    risk_rbldg.clear();
    risk_schol.clear();
    risk_workp.clear();
    
    for(map<int, rbldg*>::iterator j = inf_rbldg_night.begin(); j != inf_rbldg_night.end(); ++j){
        rbldg *r_1 = j->second;
        risk_rbldg.insert(pair<int, rbldg*>(r_1->bid, r_1));
        
        for(int i = 0; i < r_1->r_neigh.size(); ++i){       //add household
            rbldg *r_2 = r_1->r_neigh[i];
            if(r_2->h_d == NULL) continue;
            risk_rbldg.insert(pair<int, rbldg*>(r_2->bid, r_2));
        }
    }
}

void cblok::renew_epidemics(int year, int day){
    for(map<int, agent*>::iterator j = pre_indiv.begin(); j != pre_indiv.end();){
        agent *p = j->second;
        p->renew_epidemics();
        if(p->epids == 'i'){
            pre_indiv.erase(j++);
            inf_indiv.insert(pair<int, agent*>(p->aid, p));
            
            p->h_d->rdg->mbk->sum_mf++;
        }
        else ++j;
    }
    
    for(map<int, agent*>::iterator j = inf_indiv.begin(); j != inf_indiv.end();){
        agent *p = j->second;
        p->renew_epidemics();
        /*if(p->epids == 'r'){
            inf_indiv.erase(j++);
            rmv_indiv.insert(pair<int, agent*>(p->aid, p));
        }*/
        if(p->epids == 's')
            inf_indiv.erase(j++);
        else ++j;
    }
    
    /*for(map<int, agent*>::iterator j = rmv_indiv.begin(); j != rmv_indiv.end();){
        agent *p = j->second;
        p->renew_epidemics();
        if(p->epids == 's'){
            rmv_indiv.erase(j++);
        }
        else ++j;
    }*/
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

