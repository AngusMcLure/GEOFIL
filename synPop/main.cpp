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
    // insert code here...
    cblok *asamoa = new cblok(as_cid, "American Samoa", as_lat, as_long);
    cout << "total pop: " << asamoa->cpop << endl;
    cout << "total villages: " << asamoa->mbloks.size() << endl;
    cout << "total agents: " << asamoa->cblok_pop.size() << endl;
    
    cout << "available aid: " << asamoa->next_aid << endl;
    cout << "available hid: " << asamoa->next_hid << endl;
    cout << "available mid: " << asamoa->next_mid << endl;
    
    return 0;
}
