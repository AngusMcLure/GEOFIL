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
        
        //cout << mbloksIndexB[mbk->mid] << " " << worker << " " << int(mbk->jobs) << endl;
        t_1 += worker;
        t_2 += int(mbk->jobs);
        t_3 += int(mbk->labors);
    }
    cout << "total employment: wokers = " << t_1 << " jobs = " << t_2 << " labors = " << t_3 <<  endl;
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

void cblok::get_epidemics(int year){
    double total = 0, adults = 0, child = 0;
    map<int, hhold*> vec;
    for(map<int, agent*>::iterator j = inf_indiv.begin(); j != inf_indiv.end(); ++j){
        if(int(j->second->age/365) >= 15) ++adults;
        else ++child;
        
        hhold *hd = j->second->h_d;
        vec.insert(pair<int, hhold*>(hd->hid, hd));
    }
    cout << year+2010 << ": " << "prepatent = " << pre_indiv.size() << " infectious = " << inf_indiv.size() << " removed = " << rmv_indiv.size() << endl;
    cout << "adult prevalence = " << fixed << setprecision(2) << adults/(double)(cpop-cchild)*100 << "%" << endl;
    cout << "child prevalence = " << fixed << setprecision(2) << child/(double)cchild*100 << "%" << endl;
    cout << "overall prevalence = " << fixed << setprecision(2) << inf_indiv.size()/(double)cpop*100 << "%" << endl;
    
    for(map<int, hhold*>::iterator j = vec.begin(); j != vec.end(); ++j){
        total += j->second->mmbrs.size();
    }
    vec.clear();
    
    if(inf_indiv.size() == 0) total = 1;
    cout << "hhold prevalence = " << fixed << setprecision(2) << inf_indiv.size()/total*100 << "%" << endl;
}

void cblok::out_epidemics(int year, int day){
    string file = outdir;   file = file + to_string(year+2010); file = file + "_";
    file = file + to_string(day);    file = file + "_epidemics.csv";
    ofstream out(file.c_str());
    
    map<int, hhold*> vec;
    for(map<int, agent*>::iterator j = inf_indiv.begin(); j != inf_indiv.end(); ++j){
        hhold *hd = j->second->h_d;
        vec.insert(pair<int, hhold*>(hd->hid, hd));
    }
    
    out << "year,hhold_id,hhold_lat,hhold_log" << endl;
    for(map<int, hhold*>::iterator j = vec.begin(); j != vec.end(); ++j){
        hhold *p = j->second;
        out << year << "," << p->hid << ",";
        out << std::setprecision(2) << std::setiosflags(std::ios::fixed);
        out << p->lat << "," << p->log << endl;
    }
    out.close();
    
    vec.clear();
    
    /*file = outdir;   file = file + to_string(year+2010);    file = file + "_";
    file = file + to_string(day);    file = file + "_comm_graph.csv";
    out.open(file.c_str());
    
    out << "year,id,age,hhold_lat,hhold_log,dest_lat,dest_log" << endl;
    for(map<int, agent*>::iterator j = inf_indiv.begin(); j != inf_indiv.end(); ++j){
        agent *p = j->second;
        hhold *hd = p->h_d;
        if(p->s_h != NULL){
            schol *sh = p->s_h;
            out << year+2010 << "," << p->aid << "," << int(p->age/365) << ",";
            out << std::setprecision(2) << std::setiosflags(std::ios::fixed);
            out << hd->lat << "," << hd->log << "," << sh->lat << "," << sh->log << endl;
        }
        else if(p->w_p != NULL){
            workp *wp = p->w_p;
            out << year+2010 << "," << p->aid << "," << int(p->age/365) << ",";
            out << std::setprecision(2) << std::setiosflags(std::ios::fixed);
            out << hd->lat << "," << hd->log << "," << wp->lat << "," << wp->log << endl;
        }
    }
    out.close();*/
}

void cblok::prt_hhold(std::ofstream &out, hhold* hh){
    
}
