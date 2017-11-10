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
    
    cout << asamoa->cpop << endl;
    cout << asamoa->cblok_pop.size() << endl;
    
    
    return 0;
}
