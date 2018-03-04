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
        seed_epidemics();
        seed_epidemics(prl, 15, 69);
    }
    
    get_epidemics(year);
    cout << "year = " << year+2010 << " cpop = " << cpop << endl;
    get_students(year);
    get_works(year);
    
    for(int day = 0; day < 365; ++day){
        if(day % 60 == 0) out_epidemics(year, day);
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
