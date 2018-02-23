//
//  blockStat.cpp
//  synPop
//
//  Created by Sting Xu on 6/11/17.
//  Copyright © 2017 Sting Xu. All rights reserved.
//

#include "block.h"

void cblok::get_works(int year){
    int t_1 = 0, t_2 = 0, t_3 = 0;
    for(map<int, mblok*>::iterator j = mbloks.begin(); j != mbloks.end(); ++j){
        mblok *mbk = j->second;
        
        int worker = 0;
        for(map<int, workp*>::iterator k = mbk->mblok_workps.begin(); k != mbk->mblok_workps.end(); ++k)
            worker += k->second->workers.size();
        
        cout << mbloksIndexB[mbk->mid] << " " << worker << " " << int(mbk->jobs) << endl;
        t_1 += worker;
        t_2 += int(mbk->jobs);
        t_3 += int(mbk->labors);
    }
    cout << "total employment: " << t_1 << " " << t_2 << " " << t_3 <<  endl;
}

void cblok::get_hhold(int year){
    chold = 0;
    for(map<int, mblok*>::iterator j = mbloks.begin(); j != mbloks.end(); ++j){
        chold += j->second->mblok_hholds.size();
    }
    cout << "hhold = " << chold << endl;
}

void cblok::get_students(int year){
    int t_1 = 0, t_2 = 0, t_3 = 0, t = 0;
    for(int i = 0; i < cblok_schols.size(); ++i){
        schol *sh = cblok_schols[i];
        
        t += sh->student.size();
        
        switch(sh->level){
            case 'E': t_1 += sh->student.size(); break;
            case 'H': t_2 += sh->student.size(); break;
            case 'B':
                for(map<int, agent*>::iterator j = sh->student.begin(); j != sh->student.end(); ++j){
                    int age = int(j->second->age/365);
                    if(age <= 13) ++t_1;
                    else ++t_2;
                }
                break;
            case 'C': t_3 += sh->student.size(); break;
        }
    }
    cout << "total students " << year+2010 << ": " << t << " " << t_1 << " " << t_2 << " " << t_3 << endl;
}

void cblok::get_hhold_size(int year){
    map<int, double> vec;
    for(map<int, mblok*>::iterator j = mbloks.begin(); j != mbloks.end(); ++j){
        mblok *mbk = j->second;
        for(map<int, hhold*>::iterator k = mbk->mblok_hholds.begin(); k != mbk->mblok_hholds.end(); ++k){
            hhold *cur = k->second;
            if(vec.find(cur->size) != vec.end()) ++vec[cur->size];
            else vec.insert(pair<int, int>(cur->size, 1));
        }
    }
    
    double total = 0;
    for(map<int, double>::iterator j = vec.begin(); j != vec.end(); ++j)
        total += j->second;
    
    string file = outdir;   file = file + syn_hhold_stat;
    ofstream out(file.c_str(), ios::app);
    
    out << year << ",";
    for(map<int, double>::iterator j = vec.begin(); j != vec.end(); ++j){
        out << j->first << ",";
    }
    out << endl;
    
    out << year << ",";
    out << std::setprecision(4) << std::setiosflags(ios::fixed);
    for(map<int, double>::iterator j = vec.begin(); j != vec.end(); ++j){
        out << j->second/(double)total << ",";
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
