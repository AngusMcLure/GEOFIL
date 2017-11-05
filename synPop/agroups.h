//
//  agroups.h
//  synPop
//
//  Created by Sting Xu on 5/11/17.
//  Copyright © 2017 Sting Xu. All rights reserved.
//

#ifndef agroups_h
#define agroups_h

#include "classes.h"
using namespace std;

class agrp{
public:
    int age_down;
    int age_up;
    int number;
    
    agrp(int dn = 0, int up = 150, int num = 0): age_down(dn), age_up(up), number(num){}
};

class agrps{
public:
    vector<agrps*> agroups;
    
    agrps(){ agroups.clear();}
    ~agrps(){
        for(int i = 0; i < agroups.size(); ++i)
            delete agroups[i];
        
        age_agroupsgroups.clear();
        agroups.shrink_to_fit();
    }
};

#endif /* agroups_h */
