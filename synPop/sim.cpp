//
//  sim.cpp
//  synPop
//
//  Created by Sting Xu on 2/3/18.
//  Copyright © 2018 Sting Xu. All rights reserved.
//

#include "block.h"
#include "mda.h"


double max_prv;     // max prevalence of infective mosquitoes

// simulate the population & transmission
void cblok::sim_pop(int year, mda_strat strategy, default_random_engine* generator_path){
    
    max_prv = 0;

    hndl_jobs(year);
    hndl_schol(year);

    // initialise pop
    if(year == 0){
        //custom
        if(strategy.InitType == 'C'){
            //Scenario 0
            for(map<int, string>::iterator j = mbloksIndexB.begin(); j != mbloksIndexB.end(); ++j){
                
                seed_epidemics(strategy.InitPrev, 15, 100, j->second);
                seed_epidemics(strategy.InitPrev/2, 8, 14, j->second);
            }
            //default - based on survey data
        }else if(strategy.InitType == 'A'){
            //Scenario A
            for(map<int, string>::iterator j = mbloksIndexB.begin(); j != mbloksIndexB.end(); ++j){
                if(j->second != "Fagalii"){
                    seed_epidemics(0.0047, 15, 100, j->second);
                    seed_epidemics(0.0047/2, 8, 14, j->second);
                }
                
                if(j->second == "Fagalii"){
                    seed_epidemics(0.0476, 15, 100, "Fagalii");
                    seed_epidemics(0.0476/2, 8, 14, "Fagalii");
                }
            }
        }
    }
    
    achieved_coverage[year] = 0;
    achieved_coverage_m[year] = 0;
    achieved_coverage_f[year] = 0;

    if(strategy.is_mda_year(year+sim_bg)){
        implement_MDA(year, strategy);
        cout << endl << year+sim_bg << " is a MDA year" << endl;
    }

    if(toupper(strategy.Ad_MDA) == 'Y') {

        if (strategy.is_additonal_mda_year(year + sim_bg)) {
            selective_MDA(year, strategy);
            cout << endl << year + sim_bg << " is a targeted MDA year" << endl;
        }
    }




    get_epidemics(year,strategy); //write epidemic status summary to screen and linelist to csv
    get_cpop(year); //write population breakdown by age group to csv (seems to overwrite with each new simulation?)
    get_sexratiob(year);

    cout << "year = " << year+sim_bg << " cpop = " << cpop << endl;
    get_students(year);
    get_works(year);


    number_treated[year] = 0; // resetting the number treated
    for(int day = 0; day < 365; ++day){
        if(!(inf_indiv.empty() & pre_indiv.empty() & uninf_indiv.empty())) { //If disease has not been eliminated
            calc_risk(year, day, strategy, generator_path); //Determine who gets infected with new worms today - doesn't update epi status
            update_epi_status(year, day); //update everyone's LF epi status (including the status of each of their worms)

        }
        renew_pop(year, day); //update demographic aspects of population
        hndl_birth(year, day);


        for (auto const& x : strategy.MDA_Teams){
            if (year == 0 && day ==0) cout << "Number of Teams: "<< strategy.MDA_Teams.size() <<", Max Distance: "<< x->max_distance << ", Village Test: " << 100*x->village_test << "%" << endl;
            if (year+sim_bg >= x->start_year & year+sim_bg < (x->start_year+x->years)){
                continuous_mda(year, day, strategy, x);
            }
        }
        mda_countdown();

    }

    if(year == 6) get_mosquitoes(year);
    
    //test mda
    
    cout << "max prevalence of positive mosquitos = " << max_prv*100 << "%" << endl;
    
    hndl_marrg(year);
    hndl_divrc(year);
    hndl_migrt(year);
    hndl_hold_rupt(year);
}

// seed the disease in the village, according to prevalence p in age group (age_dn, age_up)
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
                    double r = drand48();
                    if(r <= p){
                        cur->epids = 'i';
                        //add one mature worms of each gender
                        double worm_life = drand48()*max_inf_period;
                        cur->wvec.push_back(new worm('m', 0, worm_life ,'m'));
                        cur->wvec.push_back(new worm('m', 0, worm_life,'f'));
                        inf_indiv.insert(pair<int, agent*>(cur->aid, cur));
                        cur->h_d->rdg->mbk->sum_mf++;
                    }
                    else if(r < p * (1 + Init_prepatent_infect_ratio)){
                        cur->epids = 'e';
                        //add one prepatent worm of each gender
                        double worm_life_mature = min_inf_period + drand48()*(max_inf_period-min_inf_period);
                        double worm_life_prepat = drand48()*max_pre_period;
                        cur->wvec.push_back(new worm('p', worm_life_prepat, worm_life_mature, 'f'));
                        cur->wvec.push_back(new worm('p', worm_life_prepat, worm_life_mature, 'm'));

                        pre_indiv.insert(pair<int, agent*>(cur->aid, cur));
                    }
                    else if(r < p * (1 + Init_uninfect_infect_ratio + Init_prepatent_infect_ratio)){
                        cur->epids = 'u';
                        //add one mature worm of one gender
                        double worm_life = drand48() * max_inf_period;
                        if(drand48() < prob_worm_male) cur->wvec.push_back(new worm('m', 0, worm_life ,'m'));
                        else cur->wvec.push_back(new worm('m', 0, worm_life,'f'));
                        uninf_indiv.insert(pair<int, agent*>(cur->aid, cur));
                    }
                }
            }
            
            for(map<int, agent*>::iterator k = mbk->mblok_fmals.begin(); k != mbk->mblok_fmals.end(); ++k){
                agent *cur = k->second;
                int age = int(cur->age/365);
                
                if(age < 15) ++cchild;
                
                if(age >= age_dn && age <= age_up){
                    double r = drand48();
                    if(r < p){
                        cur->epids = 'i';
                        //add one mature worms of each gender
                        double worm_life = drand48()*max_inf_period;
                        cur->wvec.push_back(new worm('m', 0, worm_life ,'m'));
                        cur->wvec.push_back(new worm('m', 0, worm_life,'f'));
                        inf_indiv.insert(pair<int, agent*>(cur->aid, cur));
                        cur->h_d->rdg->mbk->sum_mf++;
                    }
                    else if(r < p * (1 + Init_prepatent_infect_ratio)){
                        cur->epids = 'e';
                        //add one prepatent worm of each gender
                        double worm_life_mature = min_inf_period + drand48()*(max_inf_period-min_inf_period);
                        double worm_life_prepat = drand48()*max_pre_period;
                        cur->wvec.push_back(new worm('p', worm_life_prepat, worm_life_mature, 'f'));
                        cur->wvec.push_back(new worm('p', worm_life_prepat, worm_life_mature, 'm'));
                        
                        pre_indiv.insert(pair<int, agent*>(cur->aid, cur));
                    }
                    else if(r < p * (1 + Init_uninfect_infect_ratio + Init_prepatent_infect_ratio)){
                        cur->epids = 'u';
                        //add one mature worm of one gender
                        double worm_life = drand48() * max_inf_period;
                        if(drand48() < prob_worm_male) cur->wvec.push_back(new worm('m', 0, worm_life ,'m'));
                        else cur->wvec.push_back(new worm('m', 0, worm_life,'f'));
                        uninf_indiv.insert(pair<int, agent*>(cur->aid, cur));
                    }
                }
            }
        }
    }
}
