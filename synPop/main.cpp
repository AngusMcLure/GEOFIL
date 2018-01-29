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

int main(int argc, const char * argv[]) {
	unsigned seed = (unsigned)std::chrono::system_clock::now().time_since_epoch().count();
	srand(seed);
	std::default_random_engine generator(seed);
	srand48(seed);

    // insert code here...
    cblok *cbk = new cblok(as_cid, "American Samoa", as_lat, as_long);
    cout << "total pop: " << cbk->cpop << endl;
    cout << "married female: " << cbk->fmal_marrd.size() << endl;
    cout << "total villages: " << cbk->mbloks.size() << endl;
    cout << "available aid: " << cbk->next_aid << endl;
    cout << "available hid: " << cbk->next_hid << endl;
    cout << "available mid: " << cbk->next_mid << endl;
    cout << "vacant blding: " << cbk->cblok_vcnt_rbldgs.size() << endl;
    
    cbk->calc_marital_prob();
    
    return 0;
}
