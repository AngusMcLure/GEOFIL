//
//  agent.cpp
//  synPop
//
//  Created by Sting Xu on 2/11/17.
//  Copyright © 2017 Sting Xu. All rights reserved.
//

#include "agent.h"

agent::agent(int aid, int age, char gendr, char margs, hhold *hd, workp *wp, schol *sh, mblok *mbk, cblok *cbk){
    this->aid = aid;
    this->age = age;
    this->gendr = gendr;
    this->margs = margs;
    this->hd = hd;
    this->wp = wp;
    this->sh = sh;
    this->mbk = mbk;
    this->cbk = cbk;
}
