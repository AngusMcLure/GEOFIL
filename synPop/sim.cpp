//
//  sim.cpp
//  synPop
//
//  Created by Sting Xu on 2/3/18.
//  Copyright © 2018 Sting Xu. All rights reserved.
//

#include "block.h"

double max_prv;

void cblok::sim_pop(int year){
    max_prv = 0;
    
    hndl_jobs(year);
    hndl_schol(year);
    
    if(year == 0){
        //seed_epidemics();
        seed_epidemics(0.0025, 8, 14);
        seed_epidemics(0.0050, 15, 100);
    }
    
    get_epidemics(year);
    get_cpop(year);
    get_sexratiob(year);
    
    cout << "year = " << year+2010 << " cpop = " << cpop << endl;
    get_students(year);
    get_works(year);
    
    for(int day = 0; day < 365; ++day){
        //if((year*365+day) % 60 == 0) out_epidemics(year, day);
        //get_epidemics(year);
        renew_epidemics(year, day);
        calc_risk(year, day);
        renew_pop(year, day);
        hndl_birth(year, day);
    }
    cout << "max prevalence of positive mosquitos = " << max_prv*100 << "%" << endl;
    
    hndl_marrg(year);
    hndl_divrc(year);
    hndl_migrt(year);
    hndl_hold_rupt(year);
}

void cblok::seed_epidemics(double p, int age_dn, int age_up){
    cchild = 0;
    for(map<int, mblok*>::iterator j = mbloks.begin(); j != mbloks.end(); ++j){
        mblok *mbk = j->second;
        for(map<int, agent*>::iterator k = mbk->mblok_males.begin(); k != mbk->mblok_males.end(); ++k){
            agent *cur = k->second;
            int age = int(cur->age/365);
            
            if(age < 15) ++cchild;
            
            if(age >= age_dn && age <= age_up && drand48() <= p){
                cur->epids = 'i';
                cur->clock_inf = 0;
                cur->active_len = 4*365 + drand48()*2*365;
                inf_indiv.insert(pair<int, agent*>(cur->aid, cur));
            }
        }
        
        for(map<int, agent*>::iterator k = mbk->mblok_fmals.begin(); k != mbk->mblok_fmals.end(); ++k){
            agent *cur = k->second;
            int age = int(cur->age/365);
            
            if(age < 15) ++cchild;
            
            if(age >= age_dn && age <= age_up && drand48() <= p){
                cur->epids = 'i';
                cur->clock_inf = 0;
                cur->clock_pre = 0;
                cur->active_len = 4*365 + drand48()*2*365;
                inf_indiv.insert(pair<int, agent*>(cur->aid, cur));
            }
        }
    }
}

void cblok::seed_epidemics(){
    int sid = 10;
    if(cblok_e_schols.find(sid) == cblok_e_schols.end()){
        cout << "err: schol not found" << endl;
        exit(1);
    }
    schol *sh = cblok_e_schols[sid];
    
    int num = 2;
    while(num-- > 0){
        int rnd = rand() % sh->student.size();
        map<int, agent*>::iterator j = sh->student.begin();
        while(rnd-- > 0) ++j;
        agent *p = j->second;
        
        int age = int(p->age/365);
        if(age != 6 && age != 7) continue;
        
        if(p->epids == 'i') continue;
        
        p->epids = 'i';
        p->clock_inf = 0;
        p->clock_pre = 0;
        p->active_len = drand48()*6*365;
        inf_indiv.insert(pair<int, agent*>(p->aid, p));
    }
}
