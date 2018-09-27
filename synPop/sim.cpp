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
        //Scenario A
        /*for(map<int, string>::iterator j = mbloksIndexB.begin(); j != mbloksIndexB.end(); ++j){
             if(j->second != "Fagalii"){
             seed_epidemics(0.0047, 15, 100, j->second);
             seed_epidemics(0.0047/2, 8, 14, j->second);
             }
         
             if(j->second == "Fagalii"){
             seed_epidemics(0.0476, 15, 100, "Fagalii");
             seed_epidemics(0.0476/2, 8, 14, "Fagalii");
             }
         }*/
        
        //Scenario B
        /*for(map<int, string>::iterator j = mbloksIndexB.begin(); j != mbloksIndexB.end(); ++j){
             if(j->second != "Fagalii" && j->second != "Iliili"){
             seed_epidemics(0.0039, 15, 100, j->second);
             seed_epidemics(0.0039/2, 8, 14, j->second);
             }
         
             if(j->second == "Fagalii"){
             seed_epidemics(0.0476, 15, 100, "Fagalii");
             seed_epidemics(0.0476/2, 8, 14, "Fagalii");
             }
         
             if(j->second == "Iliili"){
             seed_epidemics(0.0269, 15, 100, "Iliili");
             seed_epidemics(0.0269/2, 8, 14, "Iliili");
             }
         }*/

        //Scenario C
        seed_epidemics(0.0516, 15, 100, "Atuu");
        seed_epidemics(0.0476, 15, 100, "Fagalii");
        seed_epidemics(0.0310, 15, 100, "Amaua");
        seed_epidemics(0.0269, 15, 100, "Iliili");
        seed_epidemics(0.0221, 15, 100, "Vaitogi");
        seed_epidemics(0.0221, 15, 100, "Alega");
        seed_epidemics(0.0155, 15, 100, "Malaeloa/Aitulagi");
        seed_epidemics(0.0155, 15, 100, "Malaeloa/Ituau");
        seed_epidemics(0.0155, 15, 100, "Amanave");
        seed_epidemics(0.0111, 15, 100, "Afao");
        seed_epidemics(0.0111, 15, 100, "Vatia");
        seed_epidemics(0.0097, 15, 100, "Pavaiai");
        seed_epidemics(0.0086, 15, 100, "Amouli");
        seed_epidemics(0.0074, 15, 100, "Aoloau");
        seed_epidemics(0.0070, 15, 100, "Aua");
        seed_epidemics(0.0055, 15, 100, "Malaeimi");
        seed_epidemics(0.0043, 15, 100, "Asili");
        seed_epidemics(0.0042, 15, 100, "Pago");
        
        seed_epidemics(0.0516/2, 8, 14, "Atuu");
        seed_epidemics(0.0476/2, 8, 14, "Fagalii");
        seed_epidemics(0.0310/2, 8, 14, "Amaua");
        seed_epidemics(0.0269/2, 8, 14, "Iliili");
        seed_epidemics(0.0221/2, 8, 14, "Vaitogi");
        seed_epidemics(0.0221/2, 8, 14, "Alega");
        seed_epidemics(0.0155/2, 8, 14, "Malaeloa/Aitulagi");
        seed_epidemics(0.0155/2, 8, 14, "Malaeloa/Ituau");
        seed_epidemics(0.0155/2, 8, 14, "Amanave");
        seed_epidemics(0.0111/2, 8, 14, "Afao");
        seed_epidemics(0.0111/2, 8, 14, "Vatia");
        seed_epidemics(0.0097/2, 8, 14, "Pavaiai");
        seed_epidemics(0.0086/2, 8, 14, "Amouli");
        seed_epidemics(0.0074/2, 8, 14, "Aoloau");
        seed_epidemics(0.0070/2, 8, 14, "Aua");
        seed_epidemics(0.0055/2, 8, 14, "Malaeimi");
        seed_epidemics(0.0043/2, 8, 14, "Asili");
        seed_epidemics(0.0042/2, 8, 14, "Pago");
    }
    
    get_epidemics(year);
    get_cpop(year);
    get_sexratiob(year);
    
    cout << "year = " << year+2010 << " cpop = " << cpop << endl;
    get_students(year);
    get_works(year);
    
    for(int day = 0; day < 365; ++day){
        //if((year*365+day) % 60 == 0) out_epidemics(year, day);
        if(year == 20 && day == 364) out_riskmap(year, day);
        //get_epidemics(year);
        calc_risk(year, day);
        renew_epidemics(year, day);
        renew_pop(year, day);
        hndl_birth(year, day);
    }
    if(year == 6) get_mosquitoes(year);
    cout << "max prevalence of positive mosquitos = " << max_prv*100 << "%" << endl;
    
    hndl_marrg(year);
    hndl_divrc(year);
    hndl_migrt(year);
    hndl_hold_rupt(year);
}

void cblok::seed_epidemics(double p, int age_dn, int age_up, string village){
    if(village != "all" && mbloksIndexA.find(village) == mbloksIndexA.end()){
        cout << village << " not found" << endl;
        exit(1);
    }
    cchild = 0;
    for(map<int, mblok*>::iterator j = mbloks.begin(); j != mbloks.end(); ++j){
        mblok *mbk = j->second;
        if(village == "all" || mbloksIndexB[mbk->mid] == village){
            for(map<int, agent*>::iterator k = mbk->mblok_males.begin(); k != mbk->mblok_males.end(); ++k){
                agent *cur = k->second;
                int age = int(cur->age/365);
                
                if(age < 15) ++cchild;
                
                if(age >= age_dn && age <= age_up){
                    if(drand48() <= p){
                        cur->epids = 'i';
                        cur->clock_inf = 0;
                        cur->wvec.push_back(new worm('m', 0, drand48()*6*365));
                        
                        inf_indiv.insert(pair<int, agent*>(cur->aid, cur));
                        cur->h_d->rdg->mbk->sum_mf++;
                    }
                    else if(drand48() < p/6){
                        cur->epids = 'e';
                        cur->clock_inf = 0;
                        cur->wvec.push_back(new worm('p', drand48()*365, 4*365 + int(drand48()*2*365)));
                        
                        pre_indiv.insert(pair<int, agent*>(cur->aid, cur));
                    }
                }
            }
            
            for(map<int, agent*>::iterator k = mbk->mblok_fmals.begin(); k != mbk->mblok_fmals.end(); ++k){
                agent *cur = k->second;
                int age = int(cur->age/365);
                
                if(age < 15) ++cchild;
                
                if(age >= age_dn && age <= age_up){
                    if(drand48() <= p){
                        cur->epids = 'i';
                        cur->clock_inf = 0;
                        cur->wvec.push_back(new worm('m', 0, drand48()*6*365));
                        
                        inf_indiv.insert(pair<int, agent*>(cur->aid, cur));
                        cur->h_d->rdg->mbk->sum_mf++;
                    }
                    else if(drand48() < p/6){
                        cur->epids = 'e';
                        cur->clock_inf = 0;
                        cur->wvec.push_back(new worm('p', drand48()*365, 4*365 + int(drand48()*2*365)));
                        
                        pre_indiv.insert(pair<int, agent*>(cur->aid, cur));
                    }
                }
            }
        }
    }
}
