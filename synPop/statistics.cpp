//
//  blockStat.cpp
//  synPop
//
//  Created by Sting Xu on 6/11/17.
//  Copyright © 2017 Sting Xu. All rights reserved.
//

#include "block.h"

void cblok::reset_prv(){
    memset(adult_prv, 0, sizeof(double)*40);
    memset(child_prv, 0, sizeof(double)*40);
    memset(all_prv, 0, sizeof(double)*40);
    memset(hhold_prv, 0, sizeof(double)*40);
}

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

void cblok::get_cpop(int year){
    string outdata = outdir;   outdata = outdata + syn_pop_stat;
    ofstream out;
    
    ifstream in;
    in.open(outdata.c_str());
    if(!in){
        out.open(outdata.c_str());
        out << "age group,0-4,5-9,10-14,15-19,20-24,25-34,35-44,45-54,55-59,60-64,65-74,75-79,80-84,85+" << endl;
        out.close();
    }
    else in.close();
    
    out.open(outdata.c_str(), std::ios::app);
    
    int cc[14];     memset(cc, 0, sizeof(int)*14);
    int index = -1;
    int max_age = 0;
    for(map<int, mblok*>::iterator k = mbloks.begin(); k != mbloks.end(); ++k){
        for(map<int, agent*>::iterator j = k->second->mblok_males.begin(); j != k->second->mblok_males.end(); ++j){
            if(max_age < j->second->age) max_age = j->second->age;
            
            if(j->second->age < 25*365) index = int(int(j->second->age/365)/5);
            else if(j->second->age >= 25*365 && j->second->age < 35*365) index = 5;
            else if(j->second->age >= 35*365 && j->second->age < 45*365) index = 6;
            else if(j->second->age >= 45*365 && j->second->age < 55*365) index = 7;
            else if(j->second->age >= 55*365 && j->second->age < 60*365) index = 8;
            else if(j->second->age >= 60*365 && j->second->age < 65*365) index = 9;
            else if(j->second->age >= 65*365 && j->second->age < 75*365) index = 10;
            else if(j->second->age >= 75*365 && j->second->age < 80*365) index = 11;
            else if(j->second->age >= 80*365 && j->second->age < 85*365) index = 12;
            else if(j->second->age >= 85*365) index = 13;
            
            cc[index]++;
        }
        
        for(map<int, agent*>::iterator j = k->second->mblok_fmals.begin(); j != k->second->mblok_fmals.end(); ++j){
            if(max_age < j->second->age) max_age = j->second->age;
            
            if(j->second->age < 25*365) index = int(int(j->second->age/365)/5);
            else if(j->second->age >= 25*365 && j->second->age < 35*365) index = 5;
            else if(j->second->age >= 35*365 && j->second->age < 45*365) index = 6;
            else if(j->second->age >= 45*365 && j->second->age < 55*365) index = 7;
            else if(j->second->age >= 55*365 && j->second->age < 60*365) index = 8;
            else if(j->second->age >= 60*365 && j->second->age < 65*365) index = 9;
            else if(j->second->age >= 65*365 && j->second->age < 75*365) index = 10;
            else if(j->second->age >= 75*365 && j->second->age < 80*365) index = 11;
            else if(j->second->age >= 80*365 && j->second->age < 85*365) index = 12;
            else if(j->second->age >= 85*365) index = 13;
            
            cc[index]++;
        }
    }
    out << year+2010 << ",";
    for(int j = 0; j < 14; ++j){
        out << cc[j]/(double)cpop << ",";
    }
    out << endl;
    out.close();
}

void cblok::get_sexratio(int year){
    
}

void cblok::get_sexratiob(int year){
    ofstream out;
    ifstream in;
    
    string outdata = outdir;
    outdata = outdata + syn_sex_ratios_broad;
    in.open(outdata.c_str());
    if(!in){
        out.open(outdata.c_str());
        out << "age group,0-14,15-64,65+" << endl;
        out.close();
    }
    else in.close();
    
    double mm[3], ff[3];
    memset(mm, 0, sizeof(double)*3);
    memset(ff, 0, sizeof(double)*3);
    
    int index = -1;
    int max_age = 0;
    for(map<int, mblok*>::iterator k = mbloks.begin(); k != mbloks.end(); ++k){
        for(map<int, agent*>::iterator j = k->second->mblok_males.begin(); j != k->second->mblok_males.end(); ++j){
            if(max_age < j->second->age) max_age = j->second->age;
            
            if(j->second->age < 15*365) index = 0;
            else if(j->second->age >= 15*365 && j->second->age < 65*365) index = 1;
            else if(j->second->age >= 65*365) index = 2;
            
            mm[index]++;
        }
        
        for(map<int, agent*>::iterator j = k->second->mblok_fmals.begin(); j != k->second->mblok_fmals.end(); ++j){
            if(max_age < j->second->age) max_age = j->second->age;
            
            if(j->second->age < 15*365) index = 0;
            else if(j->second->age >= 15*365 && j->second->age < 65*365) index = 1;
            else if(j->second->age >= 65*365) index = 2;
            
            ff[index]++;
        }
    }
    
    outdata = outdir;
    outdata = outdata + syn_sex_ratios_broad;
    out.open(outdata.c_str(), std::ios::app);
    
    //cout << t << endl;
    out << year+2010 << ",";
    for(int j = 0; j < 3; ++j){
        out << (double)100*mm[j]/ff[j] << ",";
        //cout << mm[j] << " " << ff[j] << endl;
    }
    out << cpop << endl;
    out.close();
}

void cblok::get_geographic(int year){
    
}

void cblok::get_bbldgarea(int year){
    
}

void cblok::get_epidemics(int year){
    double n_1 = 0, n_2 = 0;
    for(map<int, mblok*>::iterator j = mbloks.begin(); j != mbloks.end(); ++j){
        for(map<int, agent*>::iterator k = j->second->mblok_males.begin(); k != j->second->mblok_males.end(); ++k){
            int age = int(k->second->age/365);
            if(age >= 15) ++n_1;
            else if(age >=6 && age <= 7) ++n_2;
        }
        
        for(map<int, agent*>::iterator k = j->second->mblok_fmals.begin(); k != j->second->mblok_fmals.end(); ++k){
            int age = int(k->second->age/365);
            if(age >= 15) ++n_1;
            else if(age >=6 && age <= 7) ++n_2;
        }
    }
    
    double i_1 = 0, i_2 = 0;
    map<int, hhold*> vec;
    for(map<int, agent*>::iterator j = inf_indiv.begin(); j != inf_indiv.end(); ++j){
        int age = int(j->second->age/365);
        if(age >= 15) ++i_1;
        else if(age >=6 && age <= 7) ++i_2;
        
        hhold *hd = j->second->h_d;
        vec.insert(pair<int, hhold*>(hd->hid, hd));
    }
    
    cout << year+2010 << ": " << "prepatent = " << pre_indiv.size() << " infectious = " << inf_indiv.size() << endl;
    cout << ">=15 years' prevalence = " << fixed << setprecision(2) << i_1/(double)n_1*100 << "%" << endl;
    cout << "6-7 years' prevalence = " << fixed << setprecision(2) << i_2/(double)n_2*100 << "%" << endl;
    cout << "overall prevalence = " << fixed << setprecision(2) << inf_indiv.size()/(double)cpop*100 << "%" << endl;
    
    adult_prv[year] = i_1/(double)n_1;
    child_prv[year] = i_2/(double)n_2;
    all_prv[year] = inf_indiv.size()/(double)cpop;
}

void cblok::out_epidemics(int year, int day){
    string file = outdir;   file = file + "data_files/";
    file = file + to_string(year+2010); file = file + "_";
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
    
    string mapping = outdir;    mapping = mapping + "risk_mapping.csv";
    ifstream in;
    in.open(mapping.c_str());
    if(!in){
        out.open(mapping.c_str());
        out << "village,";
        
        for(map<int, mblok*>::iterator j = mbloks.begin(); j != mbloks.end(); ++j){
            out << mbloksIndexB[j->first] << ",";
        }
        out << endl;
        
        out.close();
    }
    else in.close();
    
    double sum = 0;
    for(map<int, mblok*>::iterator j = mbloks.begin(); j != mbloks.end(); ++j){
        sum += j->second->sum_mf;
    }
    
    out.open(mapping.c_str(), ios::app);
    out << year << "-" << day;
    for(map<int, mblok*>::iterator j = mbloks.begin(); j != mbloks.end(); ++j){
        out << "," << j->second->sum_mf/sum;
    }
    out << endl;
    out.close();
}

void cblok::prt_hhold(std::ofstream &out, hhold* hh){
    
}
