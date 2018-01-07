//
//  blocks.cpp
//  synPop
//
//  Created by Sting Xu on 5/11/17.
//  Copyright © 2017 Sting Xu. All rights reserved.
//

#include "blocks.h"

void cblok::add_agent(agent *p){
    cblok_pop.insert(pair<int, agent*>(p->aid, p));
}

void rmv_agent(agent *p){
    
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

