//
//  blockMBlok.cpp
//  synPop
//
//  Created by Sting Xu on 7/11/17.
//  Copyright © 2017 Sting Xu. All rights reserved.
//

#include "block.h"
#include <stdio.h>
#include <cstdlib>
#include <algorithm>
extern unsigned seed;
extern double max_prv;      // get the max prevalence of infective mosquitoes

// add household p to village
void mblok::add_hhold(hhold *p){
    mblok_hholds.insert(pair<int, hhold*>(p->hid, p));
}

// add agent p to village
void mblok::add_member(agent *p){
    if(p->gendr == 'm') mblok_males.insert(pair<int, agent*>(p->aid, p));
    else mblok_fmals.insert(pair<int, agent*>(p->aid, p));
}

// remove household p from village
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

// remove agent p from village
void mblok::rmv_member(agent *p){
    if(p->gendr == 'm') mblok_males.erase(p->aid);
    else mblok_fmals.erase(p->aid);
}

// add workplace p to village
void mblok::add_workp(workp *p){
    mblok_workps.insert(pair<int, workp*>(p->wid, p));
    cbk->cblok_workps.insert(pair<int, workp*>(p->wid, p));
}

// add a residential building to village
// the building can either be occupied or unoccupied
void mblok::add_rbldg(rbldg *p, hhold* h_hold){
    if(h_hold == NULL){
        mblok_vcnt_rbldgs.insert(pair<int, rbldg*>(p->bid, p));
        cbk->add_vcnt_rbldg(p);
    }
    else mblok_ocpy_rbldgs.insert(pair<int, rbldg*>(p->bid, p));
}

// estimate agent p's martial status according to demographics
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

// deal with adopting children
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

// add an unoccupied residential building to AS
void cblok::add_vcnt_rbldg(rbldg *p){
    cblok_vcnt_rbldgs.insert(pair<int, rbldg*>(p->bid, p));
}

// remove an unoccupied residential building to AS
void cblok::rmv_vcnt_rbldg(rbldg *p){
    cblok_vcnt_rbldgs.erase(p->bid);
}

// every 5 years, recalculate the commuting flux according to radiation model
// any new job seeker will follow the recalibrated commuting flux to find jobs
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

// schooling children enrol according to age
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

// schooling children choose the closest school to their residential building
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

// calcualte the force of infection in AS
// FOI has to be calculated separately for day/night, since mosquitoes are different and human locations are different
// for each building with infective mosquitoes, buildings in a range centred at the building will be at risk
// for example, if a school is of infective mosquitoes, it will affect all buildings centred at the school during day
// and only residential buildings during night (people stay at home during night)
void cblok::calc_risk(int year, int day, mda_strat strategy, default_random_engine* generator_path) {
    //resetting building lists back to zero- maybe set this to only occur every 30 days??
    //only want to run this function if date is one that we want, or else just use old values

    // This section of code creates pointers for each location from the infected buidling map and sets prevlance to zero for night and day

        for (auto const& j : inf_rbldg_night){
            rbldg *rb = j.second;
            rb->day_p = 0;
            rb->night_p = 0;
        }

        //for(map<int, rbldg*>::iterator j = inf_rbldg_night.begin(); j != inf_rbldg_night.end(); ++j){
        //for (map<int, rbldg*>::iterator j = std::begin(inf_rbldg_night); j != std::end(inf_rbldg_night); j++){



    for (auto const& j : inf_rbldg_day){
        rbldg *rb = j.second;
        rb->day_p = 0;
        rb->night_p = 0;
    }

    for (auto const& j : inf_schol){
        schol *sh = j.second;
        sh->day_p = 0;
    }

    for (auto const& j : inf_workp){
        workp *wp = j.second;
        wp->day_p = 0;
    }


    inf_rbldg_day.clear();
    inf_rbldg_night.clear();
    inf_schol.clear();
    inf_workp.clear();


    double ProbOneSex = strategy.ProbOneSex;
    double ProbBothSex = strategy.ProbBothSex;

    // looping through infected people to find force of infection, for all three locations (home, school and work)
    for (auto const& j : inf_indiv){//for each infected  agent add their contributon to the FOI (via mosquito infection prevalence) to the appropriate buildings day and night
        agent *cur = j.second;
        hhold *hd = cur->h_d;
        rbldg *rb = cur->h_d->rdg;
        schol *sh = cur->s_h;
        workp *wp = cur->w_p;

        int age = int(cur->age / 365); // Age in years
        double c = 1;

        if (age <= 15) c = exposure_by_age[age]; //Below 15 exposure read from (exposure_age.csv)
        c = c * (cur->mda_f);  //mda_f is the infectiousness of the host. 1 is default. However can be reduced if all female worms are partially sterilised. mda_f is the fertility of the most fertile female (if there are no fertile males then person is in the uninfectious category, so won't come up here)

        inf_rbldg_night.insert(pair<int, rbldg *>(rb->bid, rb)); // adding building an
        //cout << rb->night_p ;

        rb->night_p += c / (double) hd->mmbrs.size(); //Infectivity of household at night and  household size

        if (sh != NULL) {
            sh->day_p += c / (double) sh->student.size();
            inf_schol.insert(pair<int, schol *>(sh->sid, sh));
        }
        if (wp != NULL) {
            wp->day_p += c / (double) wp->workers.size();
            inf_workp.insert(pair<int, workp *>(wp->wid, wp));
        }
        if (sh == NULL && wp == NULL) {
            // count all the people who live their house who don't work or go to school
            int FamAtHome = 0;
            for (map<int, agent *>::iterator k = hd->mmbrs.begin(); k != hd->mmbrs.end(); ++k) {
                if (k->second->s_h == NULL && k->second->w_p == NULL) ++FamAtHome;
            }
            rb->day_p += c / (double) FamAtHome;
            inf_rbldg_day.insert(pair<int, rbldg *>(rb->bid, rb));
        }
    }

    //risk at day-time

    risk_loc_day(year, day);



    //calculate risk for individuals at risk rbldg
    for (auto const& j : risk_rbldg){
        rbldg *r_1 = j.second;
        hhold *hd = r_1->h_d;

        vector<agent *> r_indiv; //A list of all the retired/unemployed members of the household (who will therefore be home during the day)
        r_indiv.shrink_to_fit();
        for (auto const& k : hd->mmbrs){
            agent *p = k.second;
            if (p->s_h == NULL &&
                p->w_p == NULL) //if they don't have a school or a workplace then they will be at home
                r_indiv.push_back(p);
        }
        if (r_indiv.size() == 0) continue;

        //itself
        r_1->t_f = 0;
        r_1->t_f += 1.0;

        //r_neigbors
        for (int i = 0; i < r_1->r_neigh.size(); ++i) {
            rbldg *r_2 = r_1->r_neigh[i];
            if (r_2->h_d == NULL) continue;
            r_1->t_f += 1 - r_1->r_neigh_d[i] / r_r;
        }

        //s_neigbors
        for (int i = 0; i < r_1->s_neigh.size(); ++i) {
            schol *sh = r_1->s_neigh[i];
            if (sh->student.size() == 0) continue;
            r_1->t_f += 1 - r_1->s_neigh_d[i] / r_r;
        }

        //w_neigbors
        for (int i = 0; i < r_1->w_neigh.size(); ++i) {
            workp *wp = r_1->w_neigh[i];
            if (wp->workers.size() == 0) continue;
            r_1->t_f += 1 - r_1->w_neigh_d[i] / r_r;
        }

        //calculate prevalence
        double prv = 0;
        double f = 1.0 / r_1->t_f;
        prv += f * r_1->day_p;
        for (int i = 0; i < r_1->r_neigh.size(); ++i) {
            rbldg *r_2 = r_1->r_neigh[i];
            if (r_2->h_d == NULL) continue;

            f = (1 - r_1->r_neigh_d[i] / r_r) / r_1->t_f;
            prv += f * r_2->day_p;
        }

        for (int i = 0; i < r_1->s_neigh.size(); ++i) {
            schol *sh = r_1->s_neigh[i];
            if (sh->student.size() == 0) continue;
            f = (1 - r_1->s_neigh_d[i] / r_r) / r_1->t_f;
            prv += f * sh->day_p;
        }

        for (int i = 0; i < r_1->w_neigh.size(); ++i) {
            workp *wp = r_1->w_neigh[i];
            if (wp->workers.size() == 0) continue;
            f = (1 - r_1->w_neigh_d[i] / r_r) / r_1->t_f;
            prv += f * wp->day_p;
        }

        prv *= (r_i * s_l3); //account for probability that exposed mosquitos survive to become infectious
        if (max_prv < prv) max_prv = prv; //keep track of which building has the maximum prevalence

        for (int i = 0; i < r_indiv.size(); ++i) {
            agent *p = r_indiv[i];
            char ps = p->epids;

            double c = 1;
            int age = int(p->age / 365);
            if (age <= 15) c = exposure_by_age[age];

            p->sim_bites(prv, 'd', c, ProbOneSex, ProbBothSex, generator_path);

            //Add new exposure to the list of prepatent agents
            if (ps == 's' && p->epids == 'e') pre_indiv.insert(pair<int, agent *>(p->aid, p));
        }

        r_indiv.clear();
        r_indiv.shrink_to_fit();
    }

    for (auto const& j : risk_schol){
        schol *sh = j.second;

        //itself
        sh->t_f = 0;
        sh->t_f += 1.0;

        //r_neigbors
        for (int i = 0; i < sh->r_neigh.size(); ++i) {
            rbldg *r_2 = sh->r_neigh[i];
            if (r_2->h_d == NULL) continue;
            sh->t_f += 1 - sh->r_neigh_d[i] / r_r;
        }

        //s_neigbors
        for (int i = 0; i < sh->s_neigh.size(); ++i) {
            schol *s_2 = sh->s_neigh[i];
            if (s_2->student.size() == 0) continue;
            sh->t_f += 1 - sh->s_neigh_d[i] / r_r;
        }

        //w_neigbors
        for (int i = 0; i < sh->w_neigh.size(); ++i) {
            workp *wp = sh->w_neigh[i];
            if (wp->workers.size() == 0) continue;
            sh->t_f += 1 - sh->w_neigh_d[i] / r_r;
        }

        //calculate prevalence
        double prv = 0;
        double f = 1.0 / sh->t_f;
        prv += f * sh->day_p;
        for (int i = 0; i < sh->r_neigh.size(); ++i) {
            rbldg *r_2 = sh->r_neigh[i];
            if (r_2->h_d == NULL) continue;

            f = (1 - sh->r_neigh_d[i] / r_r) / sh->t_f;
            prv += f * r_2->day_p;
        }

        for (int i = 0; i < sh->s_neigh.size(); ++i) {
            schol *s_2 = sh->s_neigh[i];
            if (s_2->student.size() == 0) continue;
            f = (1 - sh->s_neigh_d[i] / r_r) / sh->t_f;
            prv += f * s_2->day_p;
        }

        for (int i = 0; i < sh->w_neigh.size(); ++i) {
            workp *wp = sh->w_neigh[i];
            if (wp->workers.size() == 0) continue;
            f = (1 - sh->w_neigh_d[i] / r_r) / sh->t_f;
            prv += f * wp->day_p;
        }

        prv *= (r_i * s_l3);
        if (max_prv < prv) max_prv = prv;

        for (auto const& k : sh->student){
            agent *p = k.second;
            char ps = p->epids;

            double c = 1;
            int age = int(p->age / 365);
            if (age <= 15)
                c = exposure_by_age[age];

            p->sim_bites(prv, 'd', c, ProbOneSex, ProbBothSex, generator_path);

            if (ps == 's' && p->epids == 'e') pre_indiv.insert(pair<int, agent *>(p->aid, p));
        }
    }

    for (auto const& j : risk_workp){
        workp *wp = j.second;

        //itself
        wp->t_f = 0;
        wp->t_f += 1.0;

        //r_neigbors
        for (int i = 0; i < wp->r_neigh.size(); ++i) {
            rbldg *r_2 = wp->r_neigh[i];
            if (r_2->h_d == NULL) continue;
            wp->t_f += 1 - wp->r_neigh_d[i] / r_r;
        }

        //s_neigbors
        for (int i = 0; i < wp->s_neigh.size(); ++i) {
            schol *s_2 = wp->s_neigh[i];
            if (s_2->student.size() == 0) continue;
            wp->t_f += 1 - wp->s_neigh_d[i] / r_r;
        }

        //w_neigbors
        for (int i = 0; i < wp->w_neigh.size(); ++i) {
            workp *w_2 = wp->w_neigh[i];
            if (w_2->workers.size() == 0) continue;
            wp->t_f += 1 - wp->w_neigh_d[i] / r_r;
        }

        //calculate prevalence
        double prv = 0;
        double f = 1.0 / wp->t_f;
        prv += f * wp->day_p;
        for (int i = 0; i < wp->r_neigh.size(); ++i) {
            rbldg *r_2 = wp->r_neigh[i];
            if (r_2->h_d == NULL) continue;

            f = (1 - wp->r_neigh_d[i] / r_r) / wp->t_f;
            prv += f * r_2->day_p;
        }

        for (int i = 0; i < wp->s_neigh.size(); ++i) {
            schol *s_2 = wp->s_neigh[i];
            if (s_2->student.size() == 0) continue;
            f = (1 - wp->s_neigh_d[i] / r_r) / wp->t_f;
            prv += f * s_2->day_p;
        }

        for (int i = 0; i < wp->w_neigh.size(); ++i) {
            workp *w_2 = wp->w_neigh[i];
            if (w_2->workers.size() == 0) continue;
            f = (1 - wp->w_neigh_d[i] / r_r) / wp->t_f;
            prv += f * w_2->day_p;
        }

        prv *= (r_i * s_l3);
        if (max_prv < prv) max_prv = prv;

        for (auto const& k : wp->workers){
            agent *p = k.second;
            char ps = p->epids;

            double c = 1;
            int age = int(p->age / 365);
            if (age <= 15)
                c = exposure_by_age[age];

            p->sim_bites(prv, 'd', c, ProbOneSex, ProbBothSex, generator_path);

            if (ps == 's' && p->epids == 'e') pre_indiv.insert(pair<int, agent *>(p->aid, p));
        }
    }

    //risk at night-time
    risk_loc_night(year, day);

    //calculate risk for individuals at risk rbldg

    for (auto const& j : risk_rbldg){
        rbldg *r_1 = j.second;
        hhold *hd = r_1->h_d;

        //itself
        r_1->t_f = 0;
        r_1->t_f += 1.0;

        //r_neigbors
        for (int i = 0; i < r_1->r_neigh.size(); ++i) {
            rbldg *r_2 = r_1->r_neigh[i];
            if (r_2->h_d == NULL) continue;
            r_1->t_f += 1 - r_1->r_neigh_d[i] / r_r;
        }

        //calculate prevalence
        double prv = 0;
        double f = 1.0 / r_1->t_f;
        prv += f * r_1->night_p;
        for (int i = 0; i < r_1->r_neigh.size(); ++i) {
            rbldg *r_2 = r_1->r_neigh[i];
            if (r_2->h_d == NULL) continue;

            f = (1 - r_1->r_neigh_d[i] / r_r) / r_1->t_f;
            prv += f * r_2->night_p;
        }

        prv *= (r_i * s_l3);
        if (max_prv < prv) max_prv = prv;

        for (auto const& k : hd->mmbrs){
            agent *p = k.second;
            char ps = p->epids;

            double c = 1;
            int age = int(p->age / 365);
            if (age <= 15)
                c = exposure_by_age[age];

            p->sim_bites(prv, 'n', c, ProbOneSex, ProbBothSex, generator_path);

            if (ps == 's' && p->epids == 'e') pre_indiv.insert(pair<int, agent *>(p->aid, p));

        }
    }



}

// calculate the buildings at risk during day
void cblok::risk_loc_day(int year, int day){
    risk_rbldg.clear();
    risk_schol.clear();
    risk_workp.clear();
    //#pragma omp parallel for

    for (auto const& j : inf_rbldg_day){
        rbldg *r_1 = j.second;
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
    //#pragma omp parallel for

    for (auto const& j : inf_schol){
    schol *s_1 = j.second;
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
    //#pragma omp parallel for

    for (auto const& j : inf_workp){
        workp *w_1 = j.second;
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

// calculate the buildings at risk during night
/*void cblok::risk_loc_night(int year, int day){
    risk_rbldg.clear();
    risk_schol.clear();
    risk_workp.clear();
    //#pragma omp parallel for

    #pragma omp parallel
    {
        size_t cnt = 0;
        int ithread = omp_get_thread_num();
        int nthreads = omp_get_num_threads();

        for (auto j = inf_rbldg_night.begin(); j != inf_rbldg_night.end(); ++j, cnt++) {
            if (cnt % nthreads != ithread) continue;
            rbldg *r_1 = j->second;
            risk_rbldg.insert(pair<int, rbldg*>(r_1->bid, r_1));

            for(int i = 0; i < r_1->r_neigh.size(); ++i){       //add household
                rbldg *r_2 = r_1->r_neigh[i];
                if(r_2->h_d == NULL) continue;
                risk_rbldg.insert(pair<int, rbldg*>(r_2->bid, r_2));
            }
        }
    }
}
*/
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
// update epidemic status in AS
void cblok::update_epi_status(int year, int day){
    //update the worms in each person with worms and update their epidemic status accordingly.
    //To stop people who go from prepatent to uninfected or infected or from uninfected to infected from getting updated twice, we keep track of the 'ChangedEpiToday' attribute indicating whether they have been updated today yet
    //Need to include the variable timestep

    for(map<int, agent*>::iterator j = pre_indiv.begin(); j != pre_indiv.end();){

        agent *p = j->second;

        p->update(year, day);

        if(p->epids != 'e'){
            p->ChangedEpiToday = true;
            pre_indiv.erase(j++);
            if(p->epids == 'i'){
                inf_indiv.insert(pair<int, agent*>(p->aid, p));
                // add infected person to village/meshblock running tally
                p->h_d->rdg->mbk->sum_mf++;
            }
            else if(p->epids == 'u'){
                uninf_indiv.insert(pair<int, agent*>(p->aid, p));
            }
        }
        else ++j;
    }
    
    for(map<int, agent*>::iterator j = uninf_indiv.begin(); j != uninf_indiv.end();){
        agent *p = j->second;
        if(!p->ChangedEpiToday) p->update(year, day);
        else p->ChangedEpiToday = false;
        if(p->epids != 'u'){
            uninf_indiv.erase(j++);
            if(p->epids == 'i'){
                p->ChangedEpiToday = true;
                inf_indiv.insert(pair<int, agent*>(p->aid, p));
                // add infected person to village/meshblock running tally
                p->h_d->rdg->mbk->sum_mf++;
            }
            else if(p->epids == 'e'){
                pre_indiv.insert(pair<int, agent*>(p->aid, p));
            }
        }
        else ++j;
    }
    
    for(map<int, agent*>::iterator j = inf_indiv.begin(); j != inf_indiv.end();){
        agent *p = j->second;
        if(!p->ChangedEpiToday) p->update(year, day);
        else p->ChangedEpiToday = false;
        if(p->epids != 'i'){
            //p->ChangedEpiToday = true; //since this is the last group to get updated each day we want everyone to end up with ChangedEpiToday = false to reset for the next day
            inf_indiv.erase(j++);
            if(p->epids == 'e'){
                pre_indiv.insert(pair<int, agent*>(p->aid, p));
            }
            else if(p->epids == 'u'){
                uninf_indiv.insert(pair<int, agent*>(p->aid, p));
            }
        }
        else ++j;
    }
}

// validate the population dynamics according to some simple criteria
// for example, if in a's information, b is a's wife; then in b's information, a has to be b's husband
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

// implement MDA, c is coverage, r1 indicates % of worms killed; r2 indicates % of worms sterlized; l indicates effective length
void cblok::implement_MDA(int year, mda_strat strat){
    cout << "Coverage: " << strat.Coverage << endl;
    //First we calculate the number of people in the target group for MDA (i.e. people over 2 years except pregnant or breastfeeding women)
    int under_min_age1 = 0;
    int child_bearing_age_women = 0;
    int under_nine_months = 0;
    //int breast_feeding = 0;
    int n_males = 0;
    int n_females = 0;
    
    int treat_m = 0; //number of males treated this year
    int treat_f = 0; //number of females treated this year

    
    for(map<int, mblok*>::iterator j = mbloks.begin(); j != mbloks.end(); ++j){ //for every village
        n_males += j->second->mblok_males.size();
        n_females += j->second->mblok_fmals.size();
        for(map<int, agent*>::iterator k = j->second->mblok_males.begin(); k != j->second->mblok_males.end(); ++k){ //for all males in that village
            double age = k->second->age/365.0;
            if(age<0.75) ++under_nine_months; // Count number of recent births (proxy estimate of number of women currntly pregnant)
            if(age < strat.MinAge1){
                ++under_min_age1; // Count number of people under the lowest age group (these will get no drugs)
            }
        }
        for(map<int, agent*>::iterator k = j->second->mblok_fmals.begin(); k != j->second->mblok_fmals.end(); ++k){ //for all females in that village
            double age = k->second->age/365.0;
            if(age<0.75) ++under_nine_months; // Count number of recent births (proxy estimate of number of women currntly pregnant)
            if(age < strat.MinAge1){
                ++under_min_age1; // Count number of people under the lowest age group (these will get no drugs)
            }else if(age>=15 && age<49) ++child_bearing_age_women;
/*            if(k ->second -> chdr.size()){ //if they have children
                for(map<int, agent*>::iterator c = k->second->chdr.begin(); c !=  k->second->chdr.end(); ++c){
                    cout << "child is age " << c->second->age/365.0 << endl;
                }
                cout << "birth window is " << k->second->bth_wind << endl;

                if(k->second->bth_wind) ++breast_feeding; //if youngest child is under 1
            }
*/
            //if(k->second->bth_wind) ++breast_feeding; //if they gave birth less than 1 year ago assume breastfeeding
        }
    }
    //double target_prop = 1 - (under_min_age + under_nine_months + breast_feeding)/(double)cpop;
    double target_prop = 1 - (under_min_age1 + under_nine_months)/(double)cpop;
    double prop_preg_childbearing = under_nine_months/(double)child_bearing_age_women; //estimates the number of women that are pregnant now by the number of infants under 9 months now
    
    cout << endl << prop_preg_childbearing*100 << "% of women of childbearing age are pregnant" << endl;
    cout << target_prop*100 << "% of people are in the target population" << endl;
    
    for(map<int, mblok*>::iterator j = mbloks.begin(); j != mbloks.end(); ++j){ //for every village
        for(map<int, agent*>::iterator k = j->second->mblok_males.begin(); k != j->second->mblok_males.end(); ++k){ //for all males in that village
            double age = k->second->age/365.0;
            if(drand48() <= strat.Coverage/(double)target_prop){
                if(age >= strat.MinAge1){
                    if(age >= strat.MinAge2){
                    k->second->get_drugs(strat.drug2); //administer drug combination 2 with probability coverage/target_prop if over second minimum age
                    }else{
                        k->second->get_drugs(strat.drug1); //administer drug combination 1 with probability coverage/target_prop if over first minimum age but over first minimum age
                    }
                    ++treat_m;
                }
            }
        }
        for(map<int, agent*>::iterator k = j->second->mblok_fmals.begin(); k != j->second->mblok_fmals.end(); ++k){ //for all females in that village
            double age = k->second->age/365.0;
            //if(age >= strat.MinAge && !(k->second->bth_wind)){ // if over min age for drugs and not breastfeeding (i.e. have not given birth less than one year ago)
                if(age >= 49 || age < 15){ //non child-bearing age
                    if(drand48() <= strat.Coverage/(double)target_prop){
                        if(age >= strat.MinAge1){
                            if(age >= strat.MinAge2){
                                k->second->get_drugs(strat.drug2); //administer drug combination 2 with probability coverage/target_prop if over second minimum age
                            }else{
                                k->second->get_drugs(strat.drug1); //administer drug combination 1 with probability coverage/target_prop if over first minimum age but over first minimum age
                            }
                            ++treat_f;
                        }
                    }
                } else if(drand48() <= (1-prop_preg_childbearing) * strat.Coverage/(double)target_prop){ // for women of child-bearing age
                    if(age >= strat.MinAge1){
                        if(age >= strat.MinAge2){
                            k->second->get_drugs(strat.drug2); //administer drug combination 2 with probability coverage/target_prop if over second minimum age
                        }else{
                            k->second->get_drugs(strat.drug1); //administer drug combination 1 with probability coverage/target_prop if over first minimum age but over first minimum age
                        }
                        ++treat_f;
                    }
                }
            
        }
    }
    
    // The actually achieved coverage proportions
    achieved_coverage[year] = (treat_m + treat_f)/(double)cpop;
    achieved_coverage_m[year] = treat_m/(double)n_males;
    achieved_coverage_f[year] = treat_f/(double)n_females;
}

void cblok::selective_MDA(int year, mda_strat strat) {
    /*
     * Need to decide best way to choose who gets MDA, lots of schemes we can use to analyse pop
     * First iteration going to look at 21 random villages and then treat everyone in said villages
     * Then want to to do the same but check Antigen status
     * Possibly then look at school children - would be nice to have different built in regimes
     * Could also target known villages where there is high prev
     *
     */

    //This section we are choosing who we gets the MDA
    char scheme;
    scheme = toupper(strat.Ad_Scheme);

    string village_output;
    village_output = "../output/targeted_villages.txt";

    //Regular MDA done a second time
    if (scheme == 'A'){
        strat.Coverage = strat.Ad_C_Workplace;
        implement_MDA(year, strat);
    }

    //treating n villages randomly
    if (scheme == 'R') {
        cout << "Targeting Randomly Selected Villages" << endl;
        int tot_villages = mbloks.size();
        vector<unsigned> keys; // Vector that is used to choose the villages that we want (keys for the map of villages)

        for (unsigned int i = 0; i < tot_villages; ++i) //Filling vector with numbers from 1-number of villages
        {
            keys.push_back(i + 1);
        }

        random_shuffle(keys.begin(),
                       keys.end()); // Randomising the vector to give random villages and selecting the desired number
        keys.resize(strat.Ad_N_Buildings); // Number of villages we want to look at

        cout << "Randomly Selected Villages: " << endl;
        for (auto const &i : keys) {
            cout << mbloksIndexB[i] << endl;
        }

        village_mda(year, strat, keys);

        //saving villages used
        ofstream fout;
        fout.open(village_output, ios::out | ios::app);
        fout << seed << endl;
        fout << 2009+ year << '\n';
        fout << "Scheme: " << scheme <<'\n';
        for(auto const& i: keys){
            fout << mbloksIndexB[i] << '\n';
        }
        fout << '\n';
        fout.close();
    }

    //treating n villages with highest prev
    if (scheme == 'P') {
        cout<<"Targeting " << strat.Ad_N_Buildings << " Villages with Highest Prev" << endl;
        int tot_villages = mbloks.size();
        vector<unsigned> keys; // Vector that is used to choose the villages that we want (keys for the map of villages)

        multimap<double, int, greater<double>> inf_villages;

        int village_number = 1;
        double vill_prev, num_people, num_inf;

        for (auto const &i : mbloks) {
            num_people = 0;
            num_inf = 0;
            for (auto const &j : i.second->mblok_males) {
                ++num_people;
                if (j.second->epids == 'i') {
                    ++num_inf;
                }
            }
            for (auto const &j : i.second->mblok_fmals) {
                ++num_people;
                if (j.second->epids == 'i') {
                    ++num_inf;
                }
            }
            vill_prev = num_inf / num_people;
            inf_villages.insert(make_pair(vill_prev, village_number)); //going to sort by prev
            //inf_villages.insert(make_pair(village_number,num_inf)); //going to sort by number infected
            ++village_number;
        }


        for (auto const& i : inf_villages){
            keys.push_back(i.second);
        }
        keys.resize(strat.Ad_N_Buildings);

        cout << strat.Ad_N_Buildings << " with highest prev are: " << endl;
        for (auto const& i : keys){
            cout << mbloksIndexB[i] << endl;
        }
    village_mda(year, strat, keys);
    //saving villages used
    ofstream fout;
    fout.open(village_output, ios::out | ios::app);
    fout << seed << endl;
    fout << 2009+ year << '\n';
    fout << "Scheme: " << scheme <<'\n';
    for(auto const& i: keys){
        fout << mbloksIndexB[i] << '\n';
    }
    fout << '\n';
    fout.close();
    }

    /*treating families of school aged children- village based (not the best should test at schools not home)
    if (scheme == 'S') {
        cout << "Targetting Households of Positive TAS Children" << endl;
        multimap<unsigned, unsigned> mda_houses;

        for (auto const &i : mbloks) { //looping through villages

            for (auto const &j : i.second->mblok_hholds) { //looping through households
                    //j.second->rdg->r_neigh.
                for (auto const &k: j.second->mmbrs) { // looping through household members
                    double year_age;
                    year_age = k.second->age / 365; //the age of household member

                    if (year_age >= 6 && year_age <= 17 &&
                        k.second->epids == 'i') { //if household member is of tas age and infected
                        if (drand48() <= strat.Ad_C_School) {
                            mda_houses.emplace(i.first,j.first); // storing household that will recieve mda- (village id, household id)
                            goto household_loop;
                        } else { //this household will not recieve MDA (assuming if dont want one child wont change mind for multiple houses)
                            goto household_loop;
                        }
                    }
                }
                household_loop:;
            }
        }

        cout << "Number of Villages with MDA: " << mda_houses.size() << endl;
        
        if (mda_houses.size() != 0){
            refined_household_mda(mda_houses, strat);
        } else {
            cout << "No MDA Conducted This Year" << endl;
        }
        
    }*/

    /*treating n workplaces with highest prev- randomly selected not the best
    if (scheme == 'W'){
        cout<<"Targetting " << strat.Ad_N_Buildings << " Workplaces with Highest Prev" << endl;
        //currently setup to treat n work places with most infections
        multimap<unsigned, pair<unsigned,unsigned>, greater<>> mda_workplaces;
        pair<unsigned, unsigned> vil_and_w_id;


        for (auto const& i : mbloks){ //villages
            for (auto const& j : i.second->mblok_workps){ //places of work
                int infected_workers = 0;
                for (auto const& k : j.second->workers){ //workers
                    if (k.second->epids=='i')
                        ++ infected_workers;
                }
                vil_and_w_id.first = i.first; // village_id
                vil_and_w_id.second = j.first; //workplace id
                mda_workplaces.insert(make_pair(infected_workers,vil_and_w_id));
            }
        }
        mda_workplaces.erase(prev(mda_workplaces.end(), mda_workplaces.size()-strat.Ad_N_Buildings), mda_workplaces.end()); //cutting down list to required people
        workplace_mda(year, strat, mda_workplaces);
    }*/
    
    //treating n workplaces with over a thresh-hold amount of workers
    if (scheme == 'X'){
        
        int cutoff = 5; //min workplace size we will test
        int no_workers;

        multimap<unsigned, unsigned> mda_workplaces;

        
        for (auto const& i : mbloks){ //Looping through villages
            for (auto const& j : i.second->mblok_workps){
                no_workers =  j.second->workers.size();
                if (no_workers > cutoff){

                    cout << "Village ID: " << i.second->mid << endl;
                    cout << "Workplace ID: " << j.second->wid << endl;
                    mda_workplaces.emplace(i.second->mid,j.second->wid);
                }
            }
        }
        
         workplace_mda(year, strat, mda_workplaces);
    }

    //treating n workplaces over thresh hold of workers and their families
    if (scheme == 'F'){
        int cutoff = 5;
        int no_workers;

        multimap<unsigned,unsigned> workplaces;

        //Finding all the workplaces that we want to test
        for (auto const& i : mbloks){ //Looping through villages
            for (auto const& j : i.second->mblok_workps){
                no_workers =  j.second->workers.size();
                if (no_workers > cutoff){
                    workplaces.emplace(i.second->mid,j.second->wid);
                }
            }
        }

        //Now we have found the workplaces we need to find the 'positive' households
        cout << "Now testing workers at most populace workplaces" << endl;
        
        multimap<unsigned, unsigned> will_take; //map to store households that will take MDA
        vector<unsigned > will_not_take;  //vector to store households that will not take MDA- vector as do not need Village ID

        for (auto const& i : workplaces){ //looping through workplaces
            for (auto const& j : mbloks[i.first]->mblok_workps[i.second]->workers){ //looping through workers
                unsigned house_id;
                unsigned village_id;

                house_id = j.second->h_d->hid;
                village_id = j.second->h_d->rdg->mbk->mid;


                if (find(will_not_take.begin(), will_not_take.end(), house_id) != will_not_take.end()){ //first we check if the household has already refused MDA

                } else {
                    if (drand48() <= strat.Ad_C_Workplace) { //if will accept test
                        if (j.second->epids == 'i' || j.second->epids == 'u'|| drand48() < pow(DailyProbLoseAntigen, year*365 - j.second->LastDayWithAdultWorm)) { //if found to be infected
                            will_take.emplace(village_id,
                                              house_id); //Storing house data (village id, house id) as infected person found and tested
                        }
                    } else {
                        will_not_take.push_back(house_id); //household has refused test
                    }
                }
            }
        }
        cout << "Treating positive workers' families" << endl;
        cout << "Houses to treat: " << will_take.size() << endl;

        if (will_take.size() > 0 ){
            refined_household_mda(will_take, strat);
        } else {
            cout << "No Need to treat" << endl;
        }
       
        will_not_take.clear();
        will_take.clear();
    }

    //Families of positive elementary or high school students
    if (scheme == 'E' || scheme == 'H'){

        bool TAS = false; // if only want tas age children
        bool Both = false; //if want both high school and elementary

        int number_schools = 0;
        map <int, schol *>  school_type;

        multimap<unsigned, unsigned> will_take; //map to store households of children that will take MDA
        vector<unsigned > will_not_take;  //vector to store households that will not take MDA- vector as do not need Village ID
        //I am assuming if child will get tested then will take MDA

        //Determining schools to analyse
        if (scheme == 'E' || Both == true){
            for (auto const& v : cblok_e_schols){ // looping through all elementary school
                ++number_schools;
                for (auto const& j : v.second->student) { // looping through all students of selected school

                    unsigned house_id = j.second->h_d->hid;
                    unsigned village_id = j.second->h_d->rdg->mbk->mid;

                    double age = j.second->age/365.0;

                    if (TAS == true){
                        if (6 <= age && age <= 7){
                            if (find(will_not_take.begin(), will_not_take.end(), house_id) != will_not_take.end()){ //first we check if the household has already refused MDA
                                cout << "Household Has Previously Refused MDA" << endl;
                            } else {
                                if (drand48() <= strat.Ad_C_Workplace) { //if will accept test

                                    if (j.second->epids == 'i' || j.second->epids == 'u'|| drand48() < pow(DailyProbLoseAntigen, year*365 - j.second->LastDayWithAdultWorm)) { //if found to be infected
                                        cout << "Will take infected" << endl;
                                        will_take.emplace(village_id,
                                                          house_id); //Storing house data (village id, house id) as infected person found and tested
                                    }
                                } else {

                                    if (j.second->epids == 'i') cout << "Will not take pos" << endl;
                                    will_not_take.push_back(house_id); //household has refused test
                                }
                            }
                        }
                    }else{
                        if (find(will_not_take.begin(), will_not_take.end(), house_id) != will_not_take.end()){ //first we check if the household has already refused MDA
                            //cout << "Household Has Previously Refused MDA" << endl;
                        } else {
                            if (drand48() <= strat.Ad_C_Workplace) { //if will accept test
                                if (j.second->epids == 'i' || j.second->epids == 'u'|| drand48() < pow(DailyProbLoseAntigen, year*365 - j.second->LastDayWithAdultWorm)) { //if found to be infected
                                    will_take.emplace(village_id,
                                                      house_id); //Storing house data (village id, house id) as infected person found and tested
                                }
                            } else {
                                will_not_take.push_back(house_id); //household has refused test
                            }
                        }
                    }
                }
            }
        }
        if (scheme == 'H' || Both == true){
            for (auto const& v : cblok_h_schols){ // looping through all elementary schools
                ++number_schools;
                for (auto const& j : v.second->student) { // looping through all students of selected school

                    unsigned house_id = j.second->h_d->hid;
                    unsigned village_id = j.second->h_d->rdg->mbk->mid;

                    double age = j.second->age/365.0;

                    if (find(will_not_take.begin(), will_not_take.end(), house_id) != will_not_take.end()){ //first we check if the household has already refused MDA
                        //cout << "Household Has Previously Refused MDA" << endl;
                    } else {
                        if (drand48() <= strat.Ad_C_Workplace) { //if will accept test
                            if (j.second->epids == 'i' || j.second->epids == 'u'|| drand48() < pow(DailyProbLoseAntigen, year*365 - j.second->LastDayWithAdultWorm)) { //if found to be infected
                                will_take.emplace(village_id,
                                                  house_id); //Storing house data (village id, house id) as infected person found and tested
                            }
                        } else {
                            will_not_take.push_back(house_id); //household has refused test
                        }
                    }
                }
            }
        }
        cout << "Testing at " << number_schools << ", schools!" << endl;

        cout << "Treating positive childrens' families" << endl;
        cout << "Houses to treat: " << will_take.size() << endl;

        if (will_take.size() > 0 ){
            refined_household_mda(will_take, strat);
        } else {
            cout << "No Need to treat" << endl;
        }

        will_not_take.clear();
        will_take.clear();
    }

}

void cblok::village_mda(int year, mda_strat strat, vector<unsigned>keys){

    int under_min_age1 = 0;
    int child_bearing_age_women = 0;
    int under_nine_months = 0;

    int n_males = 0;
    int n_females = 0;

    int treat_m = 0; //number of males treated this year
    int treat_f = 0; //number of females treated this year


    for (auto const& i: keys){
        n_males += mbloks[i]->mblok_males.size();
        n_females += mbloks[i]->mblok_fmals.size();

        for (auto const& k: mbloks[i]->mblok_males){ // Males in selected villages
            double age = k.second->age/365.0;
            if(age<0.75) ++under_nine_months; // Count number of recent births
            if(age < strat.MinAge1){
                ++under_min_age1; // Count number of people under the lowest age group (these will get no drugs)
            }
        }

        for (auto const& k: mbloks[i]->mblok_fmals){
            double age = k.second->age/365.0;
            if(age<0.75) ++under_nine_months; // Count number of recent births (proxy estimate of number of women currntly pregnant)
            if(age < strat.MinAge1){
                ++under_min_age1; // Count number of people under the lowest age group (these will get no drugs)
            }else if(age>=15 && age<49) ++child_bearing_age_women;
        }
    }

    double target_pop = n_males + n_females;

    double target_prop = 1 - (under_min_age1 + under_nine_months)/target_pop;
    cout << "Target prop: " << target_prop << endl;
    double prop_preg_childbearing = under_nine_months/(double)child_bearing_age_women; //estimates the number of women that are pregnant now by the number of infants under 9 months now

    for (auto const& i : keys){
        for (auto const& k : mbloks[i]->mblok_males){
            double age = k.second->age/365.0;
            if(drand48() <= strat.Coverage/(double)target_prop) {
                if (age >= strat.MinAge1) {
                    if (age >= strat.MinAge2) {
                        k.second->get_drugs(
                                strat.drug2); //administer drug combination 2 with probability coverage/target_prop if over second minimum age
                    } else {
                        k.second->get_drugs(
                                strat.drug1); //administer drug combination 1 with probability coverage/target_prop if over first minimum age but over first minimum age
                    }
                    ++treat_m;
                }
            }
        }
        for (auto const& k : mbloks[i]->mblok_fmals){
            double age = k.second->age/365.0;
            if(age >= 49 || age < 15){ //non child-bearing age
                if(drand48() <= strat.Coverage/(double)target_prop){
                    if(age >= strat.MinAge1){
                        if(age >= strat.MinAge2){
                            k.second->get_drugs(strat.drug2); //administer drug combination 2 with probability coverage/target_prop if over second minimum age
                        }else{
                            k.second->get_drugs(strat.drug1); //administer drug combination 1 with probability coverage/target_prop if over first minimum age but over first minimum age
                        }
                        ++treat_f;
                    }
                }
            } else if(drand48() <= (1-prop_preg_childbearing) * strat.Coverage/(double)target_prop){ // for women of child-bearing age
                if(age >= strat.MinAge1){
                    if(age >= strat.MinAge2){
                        k.second->get_drugs(strat.drug2); //administer drug combination 2 with probability coverage/target_prop if over second minimum age
                    }else{
                        k.second->get_drugs(strat.drug1); //administer drug combination 1 with probability coverage/target_prop if over first minimum age but over first minimum age
                    }
                    ++treat_f;
                }
            }
        }
    }

    // The actually achieved coverage proportions
    achieved_coverage[year] = (treat_m + treat_f)/(double)target_pop;
    achieved_coverage_m[year] = treat_m/(double)n_males;
    achieved_coverage_f[year] = treat_f/(double)n_females;
}

void cblok::refined_household_mda(multimap<unsigned, unsigned> mda_houses, mda_strat strat){

    unsigned village_id;
    unsigned house_id;

    int under_min_age1 = 0;
    int child_bearing_age_women = 0;
    int under_nine_months = 0;

    int n_pop = 0;
    int n_houses = 0;

    int treat_ed = 0; 
    
    for (auto const& i: mda_houses){ //villages

        village_id = i.first;
        house_id = i.second;
      
        ++n_houses;

        n_pop += mbloks[village_id]->mblok_hholds[house_id]->size;

    
        for (auto const& k : mbloks[village_id]->mblok_hholds[house_id]->mmbrs){
            double age = k.second->age/365.0;
            if(age<0.75) ++under_nine_months;
            if(age < strat.MinAge1) {
                ++under_min_age1;
            }
            else if (k.second->gendr == 'f' && age>=15 && age<49) {
                ++child_bearing_age_women;
            }
        }
    }

    cout <<"Number of Houses with MDA: " << n_houses << endl;
    cout <<"Number of People with MDA: " << n_pop << endl;

    double target_prop = 1 - (under_min_age1 + under_nine_months)/n_pop;
    double prop_preg_childbearing = under_nine_months/(double)child_bearing_age_women; //estimates the number of women that are pregnant now by the number of infants under 9 months now

    cout << "Proportion of woman who are pregnant: " << prop_preg_childbearing << endl;

    for (auto const& i : mda_houses){ //villages

        village_id = i.first;
        house_id = i.second;

        for (auto const& k : mbloks[village_id]->mblok_hholds[house_id]->mmbrs){// members of said household
            double age = k.second->age/365.0;
            if (k.second->gendr=='m'){
                if (age >= strat.MinAge1){
                    if (age >= strat.MinAge2){
                        k.second->get_drugs(strat.drug2);
                    } else {
                        k.second->get_drugs(strat.drug1);
                    }
                    ++treat_ed;
                }
            }
            if (k.second->gendr=='f'){
                if(age >= 49 || age < 15){ //non child-bearing age
                    if(age >= strat.MinAge1){
                        if(age >= strat.MinAge2){
                            k.second->get_drugs(strat.drug2);
                        }else{
                            k.second->get_drugs(strat.drug1);
                        }
                        ++treat_ed;
                    }
                } else if(drand48() <= (1-prop_preg_childbearing)){ // for women of child-bearing age
                    if(age >= strat.MinAge1){
                        if(age >= strat.MinAge2){
                            k.second->get_drugs(strat.drug2);
                        }else{
                            k.second->get_drugs(strat.drug1);
                        }
                        ++treat_ed;
                    }
                }
            }
        }
    }
    cout << "Treated: " << treat_ed << endl;
}

void cblok::workplace_mda(int year, mda_strat strat, multimap<unsigned, unsigned> mda_workplaces){
    int under_min_age1 = 0;
    int child_bearing_age_women = 0;
    int under_nine_months = 0;


    int n_pop = 0;
    int n_males = 0;
    int n_females = 0;

    int treat_m = 0; //number of males treated this year
    int treat_f = 0; //number of females treated this year

    for (auto const& i: mda_workplaces){ //looping through villages

        n_pop += mbloks[i.first]->mblok_workps[i.second]->workers.size(); //number of workers in work place

        for (auto const& j: mbloks[i.first]->mblok_workps[i.second]->workers){
            double age = j.second->age/365.0;

            if(age<0.75) ++under_nine_months;
            if(age < strat.MinAge1) {
                ++under_min_age1;
            }
            else if (j.second->gendr == 'f' && age>=15 && age<49) {
                ++child_bearing_age_women;
            }

        }

    }


    double target_prop = 1 - (under_min_age1 + under_nine_months)/n_pop;
    cout << "Target prop: " << target_prop << endl;
    double prop_preg_childbearing = under_nine_months/(double)child_bearing_age_women; //estimates the number of women that are pregnant now by the number of infants under 9 months now

    for (auto const& i : mda_workplaces){ //looping through workplaces
        for (auto const& j : mbloks[i.first]->mblok_workps[i.second]->workers){ //going through workers
            double age = j.second->age/365.0;
            if (j.second->epids == 'i' || j.second->epids == 'u'|| drand48() < pow(DailyProbLoseAntigen, year*365 - j.second->LastDayWithAdultWorm)) { //if test positive
                if (j.second->gendr == 'm') {
                    if (drand48() <= strat.Ad_C_Workplace / (double) target_prop) {
                        if (age >= strat.MinAge1) {
                            if (age >= strat.MinAge2) {
                                j.second->get_drugs(
                                        strat.drug2); //administer drug combination 2 with probability coverage/target_prop if over second minimum age
                            } else {
                                j.second->get_drugs(
                                        strat.drug1); //administer drug combination 1 with probability coverage/target_prop if over first minimum age but over first minimum age
                            }
                            ++treat_m;
                        }
                    }
                }

                if (j.second->gendr == 'f') {
                    if (age >= 49 || age < 15) { //non child-bearing age
                        if (drand48() <= strat.Ad_C_Workplace / (double) target_prop) {
                            if (age >= strat.MinAge1) {
                                if (age >= strat.MinAge2) {
                                    j.second->get_drugs(
                                            strat.drug2); //administer drug combination 2 with probability coverage/target_prop if over second minimum age
                                } else {
                                    j.second->get_drugs(
                                            strat.drug1); //administer drug combination 1 with probability coverage/target_prop if over first minimum age but over first minimum age
                                }
                                ++treat_f;
                            }
                        }
                    } else if (drand48() <= (1 - prop_preg_childbearing) * strat.Ad_C_Workplace /
                                            (double) target_prop) { // for women of child-bearing age
                        if (age >= strat.MinAge1) {
                            if (age >= strat.MinAge2) {
                                j.second->get_drugs(
                                        strat.drug2); //administer drug combination 2 with probability coverage/target_prop if over second minimum age
                            } else {
                                j.second->get_drugs(
                                        strat.drug1); //administer drug combination 1 with probability coverage/target_prop if over first minimum age but over first minimum age
                            }
                            ++treat_f;
                        }
                    }
                }
            }
        }
    }
    cout << "Treated: " << treat_f +treat_m << endl;
    // The actually achieved coverage proportions
    achieved_coverage[year] = (treat_m + treat_f)/(double)n_pop;
    achieved_coverage_m[year] = treat_m/(double)n_pop;
    achieved_coverage_f[year] = treat_f/(double)n_pop;
}

void cblok::continuous_mda(int year, int day, mda_strat strat, targeted_mda *data) {\

 if (data->days_in_village==0){

     unsigned time;
     unsigned n_villages;
     unsigned n_houses;
     unsigned n_test;
     unsigned village_id; //ID of village that "team is in"

     vector<unsigned> houses_to_test; //houses that we are going to 'test' for mda.
     vector<unsigned> infected_houses;
     multimap<unsigned,unsigned> mda_houses;


     while (mbloks[data->village_path[data->treated_v]]->mda != 0 ){
         cout << "\n"<< mbloksIndexB[data->village_path[data->treated_v]]<< " Already Treated!" << endl;
         if (data->treated_v<(data->n_villages-1)){
             data->treated_v += 1; //moving onto the next village
         } else if (data -> treated_v == (data->n_villages-1)){
             data-> treated_v = 0; // resetting count when reaches last village
         }
         cout << "Trying: " << mbloksIndexB[data->village_path[data->treated_v]] << endl;
     }


     village_id = data->village_path[data->treated_v]; //picking out the village we are in


     int mda_id;
     mda_id = rand() % 10000; //generating an ID which we will use to keep track of houses that take MDA

     cout << "\n" <<"Team "<< data->team_number <<" Currently In: "<<mbloksIndexB[village_id] << endl;

     householdistances(data->max_distance,village_id); //working out all households within x distance of all houses in our selected village

     n_houses = mbloks[village_id]->mblok_hholds.size();
     n_test  = n_houses*data->village_test; //Try to test so much of village

     if (n_test <data->min_test){ //ensuring enough households are test
         n_test=data->min_test;
     }

     for (auto const& u : mbloks[village_id]->mblok_hholds){ // making vector of all household ids
         houses_to_test.push_back(u.first);
     }

     random_shuffle(houses_to_test.begin(), houses_to_test.end()); // Randomising the vector to give random houses and selecting the desired number

     if (n_test > n_houses){
         cout << "Due to small village size will attempt to test all households" << endl;
     }else{
         houses_to_test.resize(n_test);
     }
     cout  << "Number of houses will attempt to test: " << houses_to_test.size() << endl;

     time = houses_to_test.size()/3; //TODO realistic timing param
     if (time < data->min_time){
         time = data->min_time; //minimum time in each village
     }

     data->days_in_village=time;

     //Here we are testing the houses in the village
     for (auto const& y : houses_to_test){
         if ((mbloks[village_id]->mblok_hholds[y]->mda == 0) & (drand48() < data->coverage) ) {// likelyhood that household will get tested and household has not had mda already (no ID)
             for (auto const &x : mbloks[village_id]->mblok_hholds[y]->mmbrs) {
                 if (x.second->age / 365 > 6 && (x.second->epids == 'i' || x.second->epids == 'u'|| drand48() < pow(DailyProbLoseAntigen, year*365 - x.second->LastDayWithAdultWorm))) {
                     infected_houses.push_back(y);
                     mbloks[village_id]->mblok_hholds[y]->mda = mda_id; //assigning unique ID to positive houses in this round
                     goto backtohouses; //to avoid household being recorded multiple times
                 }
             }
         }
         backtohouses:;
     }

     //Now we need find and store all the neighbours of the infected houses (could be in different villages)

     if(!infected_houses.empty()) { // only doing this if there have been infected houses found
         cout << "Infection Detected" << endl;
         for (auto const &x : infected_houses) { //looping through infected houses
             for (auto const&z : house_distance[x]){ //looping through neighbours of infected houses
                 if (mbloks[z.first]->mblok_hholds[z.second]->mda == 0){
                     if (drand48() < data->coverage) { // prob that will allow mda
                         mda_houses.emplace(z.first, z.second);
                     }//adding in houses that we need to treat
                     mbloks[z.first]->mblok_hholds[z.second]->mda = mda_id; //recording that the house has been found (to avoid doubling) CAN ADD special TAG for non compliance
                 }
             }
             mda_houses.emplace(village_id, x); //Now we also have to include the houses that actually had the positive! No new prob as already accepted test
         }


         cout << mda_houses.size() << " Households to receive MDA" << endl;
         data->days_in_village += mda_houses.size()/3;
         cout << "Spending "<< data->days_in_village << " Days" << endl;

         //Now we need to treat the people in the selected households
         refined_household_mda(mda_houses, strat);

     }else{
         cout << "No Infection Detected"<< endl;
         cout << "Spending "<< data->days_in_village << " Days" << endl;
     }
     mbloks[village_id]->mda = mda_id;
     mbloks[village_id]->days_before_mda = data->days_before_return;

     if (data->treated_v<(data->n_villages-1)){
         data->treated_v += 1; //moving onto the next village
     } else if (data -> treated_v == (data->n_villages-1)){

         data-> treated_v = 0; // resetting count when reaches last village
     }
 }else{
     data->days_in_village -= 1; //counting down days left in village (time before next village)
 }
}

void cblok::mda_countdown(){
    for (auto const &x : mbloks) { //counting down timers in villages till they can get mda again
        if (x.second->days_before_mda >0){
            x.second->days_before_mda -= 1;
        }
        else if (x.second->days_before_mda == 0 & x.second->mda !=0 ){
            int house_id = x.second->mda;
            //now looping through all villages to find houses with said marker
            for (auto const& y : mbloks){
                for (auto const& z : y.second->mblok_hholds){
                    if (z.second->mda == house_id){
                        z.second->mda = 0; //resetting houses
                    }
                }
            }

            x.second->mda = 0; //resetting MDA ID as village can now have MDA again
            //reseting house markers
        }
    }
}

void cblok::householdistances(int max_distance, int village_number) {

    house_distance.clear(); //getting rid of previous

    unsigned secondary_village_id, main_house_id, secondary_house_id;


    for (auto const& i: mbloks[village_number]->mblok_hholds) { //looping through households in primary village each map entry is a different house

        vector<pair<unsigned,unsigned>> a_house;

        main_house_id = i.first;

        for (auto const &j: mbloks) { //looping through secondary villages

            secondary_village_id = j.first;

            for (auto const &k: j.second->mblok_hholds) { //looping through houses in secondary villages

                double distance;
                pair<int, int> vil_house_id;

                secondary_house_id = k.first;

                distance = sqrt(pow((i.second->lat-k.second->lat),2)+pow((i.second->log-k.second->log),2)); // distance between houses

                if (distance >0 & distance < max_distance){//making sure not choosing same house again and house not too fair away
                    vil_house_id.first = secondary_village_id;
                    vil_house_id.second = secondary_house_id;
                    a_house.push_back(vil_house_id); //storing the data of the house
                }
            }
        }
        if (!a_house.empty()){
            house_distance.insert(make_pair(main_house_id,a_house));
        }

        a_house.clear();
    }

}