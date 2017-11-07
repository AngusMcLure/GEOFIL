//
//  blockMBlok.cpp
//  synPop
//
//  Created by Sting Xu on 7/11/17.
//  Copyright © 2017 Sting Xu. All rights reserved.
//

#include "blocks.h"

void mblok::rmv_hhold(hhold *p){
    mblok_hholds.erase(p->hid);
    p->rdg->hd = NULL;
    cbk->cblok_rbldgs_vcnt.insert(pair<int, rbldg*>(p->rdg->bid, p->rdg));
    p->rdg = NULL;
    
    delete p;
}

