//
//  main.cpp
//  synPop
//
//  Created by Sting Xu on 2/11/17.
//  Copyright © 2017 Sting Xu. All rights reserved.
//

#include <iostream>
#include <ctime>
#include "main.h"
using namespace std;

int deaths;
int births;
int rup;

int main(int argc, const char * argv[]) {
	unsigned seed = (unsigned)std::chrono::system_clock::now().time_since_epoch().count();
	srand(seed);
	std::default_random_engine generator(seed);
	srand48(seed);
    
    // insert code here...
    cblok *cbk = new cblok(as_cid, "American Samoa", as_lat, as_long);
    cout << "total pop: " << cbk->cpop << endl;
    cout << "total villages: " << cbk->mbloks.size() << endl;
    cout << "available aid: " << cbk->next_aid << endl;
    cout << "available hid: " << cbk->next_hid << endl;
    cout << "available mid: " << cbk->next_mid << endl;
    cout << "vacant blding: " << cbk->cblok_vcnt_rbldgs.size() << endl;

    for(int year = 0; year < 40; ++year){
        births = 0;
        deaths = 0;
        rup = 0;
        cbk->sim_pop(year);
        cout << "year = " << year+2011 << " birth = " << births << " death = " << deaths  << " mgrt = " << int(cbk->cpop*cbk->pop_loss[year]/1000) << " hhold rup = " << rup << endl;
        cbk->get_pop(year);
        cbk->get_hhold(year);
        cout << "vant bldg = " << cbk->cblok_vcnt_rbldgs.size() << endl;
        
        if(year == 0) cbk->get_hhold_size(year);
        if((year+1)%10 == 0) cbk->get_hhold_size(year);
    }
    
    return 0;
}
