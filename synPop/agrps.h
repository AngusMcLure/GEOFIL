//
//  agroups.h
//  synPop
//
//  Created by Sting Xu on 5/11/17.
//  Copyright © 2017 Sting Xu. All rights reserved.
//

#ifndef agrps_h
#define agrps_h
#include <vector>
#include <cstring>
#include "paras.h"
using namespace std;

// age groups
class agrps{
public:
    int age_dn[vg_agrps];
    int age_up[vg_agrps];
    int pop[vg_agrps];
    int male[vg_agrps];
    int female[vg_agrps];
    
    agrps(){
        memset(age_dn, 0, sizeof(int)*vg_agrps);
        memset(age_up, 0, sizeof(int)*vg_agrps);
        memset(pop, 0, sizeof(int)*vg_agrps);
        memset(male, 0, sizeof(int)*vg_agrps);
        memset(female, 0, sizeof(int)*vg_agrps);
    }
    ~agrps(){
        memset(age_dn, 0, sizeof(int)*vg_agrps);
        memset(age_up, 0, sizeof(int)*vg_agrps);
        memset(pop, 0, sizeof(int)*vg_agrps);
        memset(male, 0, sizeof(int)*vg_agrps);
        memset(female, 0, sizeof(int)*vg_agrps);
    }
};

#endif /* agroups_h */
