//
//  blocks.cpp
//  synPop
//
//  Created by Sting Xu on 5/11/17.
//  Copyright © 2017 Sting Xu. All rights reserved.
//

#include "block.h"

void cblok::hndl_birth(int t){
    int year = int(t/365);
    
    int total_birth = 0;
    for(map<int, mblok*>::iterator j = mbloks.begin(); j != mbloks.end(); ++j){
        mblok *mbk = j->second;
        for(map<int, agent*>::iterator k = mbk->mblok_fmals.begin(); k != mbk->mblok_fmals.end(); ++k){
            
        }
    }
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

