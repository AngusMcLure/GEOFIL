//
//  blockStat.cpp
//  synPop
//
//  Created by Sting Xu on 6/11/17.
//  Copyright © 2017 Sting Xu. All rights reserved.
//

#include "block.h"

void cblok::get_pop(int year){
    cpop = 0;
    for(map<int, mblok*>::iterator j = mbloks.begin(); j != mbloks.end(); ++j){
        cpop += j->second->mblok_males.size();
        cpop += j->second->mblok_fmals.size();
    }
    cout << "pop = " << cpop << endl;
}

void cblok::get_hhold(int year){
    chold = 0;
    for(map<int, mblok*>::iterator j = mbloks.begin(); j != mbloks.end(); ++j){
        chold += j->second->mblok_hholds.size();
    }
    cout << "hhold = " << chold << endl;
}

void cblok::get_hhold_size(int year){
    map<int, int> vec;
    for(map<int, mblok*>::iterator j = mbloks.begin(); j != mbloks.end(); ++j){
        mblok *mbk = j->second;
        for(map<int, hhold*>::iterator k = mbk->mblok_hholds.begin(); k != mbk->mblok_hholds.end(); ++k){
            hhold *cur = k->second;
            if(vec.find(cur->size) != vec.end()) ++vec[cur->size];
            else vec.insert(pair<int, int>(cur->size, 1));
        }
    }
    
    string file = outdir;   file = file + syn_hhold_stat;
    ofstream out(file.c_str(), ios::app);
    
    out << year << ",";
    for(map<int, int>::iterator j = vec.begin(); j != vec.end(); ++j){
        out << j->first << ",";
    }
    out << endl;
    
    out << year << ",";
    for(map<int, int>::iterator j = vec.begin(); j != vec.end(); ++j){
        out << j->second << ",";
    }
    out << endl;
    
    out.close();
    vec.clear();
}

void cblok::get_sexratio(int year){
    
}

void cblok::get_sexratiob(int year){
    
}

void cblok::get_geographic(int year){
    
}

void cblok::get_bbldgarea(int year){
    
}

void cblok::prt_hhold(std::ofstream &out, hhold* hh){
    
}
