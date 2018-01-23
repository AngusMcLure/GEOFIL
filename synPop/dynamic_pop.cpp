//
//  blocks.cpp
//  synPop
//
//  Created by Sting Xu on 5/11/17.
//  Copyright © 2017 Sting Xu. All rights reserved.
//

#include "block.h"

void cblok::add_vcnt_rbldg(rbldg *p){
    cblok_vcnt_rbldgs.insert(pair<int, rbldg*>(p->bid, p));
}

void cblok::rmv_vcnt_rbldg(rbldg *p){
    cblok_vcnt_rbldgs.erase(p->bid);
}

void cblok::hndl_birth(int t){
    
}

void cblok::hndl_death(int t){
    /*if(gendr == 'F') cbk->fmal_marrd.erase(aid);
    else cbk->fmal_marrd.erase(spw->aid);*/
}

void cblok::hndl_marrg(int t){
    
}

void cblok::hndl_divrc(int t){
    /*if(gendr == 'F') cbk->fmal_marrd.erase(aid);
     else cbk->fmal_marrd.erase(spw->aid);*/
}

void cblok::hndl_ploss(int t){
    /*if(gendr == 'F') cbk->fmal_marrd.erase(aid);
     else cbk->fmal_marrd.erase(spw->aid);*/
}

void cblok::hndl_mvout(int t){
    
}

void cblok::hndl_mvgin(int t){
    
}

void cblok::hndl_hldfrc(int t){
    
}

