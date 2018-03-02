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
    /*cout << "total pop: " << cbk->cpop << endl;
    cout << "total villages: " << cbk->mbloks.size() << endl;
    cout << "total school: " << cbk->cblok_schols.size() << endl;
    cout << "total elementary: " << cbk->cblok_e_schols.size() << endl;
    cout << "total high: " << cbk->cblok_h_schols.size() << endl;
    cout << "total college: " << cbk->cblok_c_schols.size() << endl;*/
    for(int year = 0; year < 40; ++year){
        births = 0;
        deaths = 0;
        rup = 0;
        cbk->sim_pop(year);
    }
    
    return 0;
}
