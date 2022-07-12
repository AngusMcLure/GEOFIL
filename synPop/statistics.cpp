//
//  blockStat.cpp
//  synPop
//
//  Created by Sting Xu on 6/11/17.
//  Copyright © 2017 Sting Xu. All rights reserved.
//

#include "block.h"
#include <cstring>
extern int SimulationNumber;
extern unsigned seed;
//extern __iom_t10<char> SimulationDateStr;
extern string prv_out_loc;
int data_ant_u;
int data_ant_o;
int data_mf;
// reset pop prevalence
void cblok::reset_prv(){
    memset(adult_prv, 0, sizeof(double)*40);
    memset(child_6_7_prv, 0, sizeof(double)*40);
    memset(all_prv, 0, sizeof(double)*40);
    memset(fagalli, 0, sizeof(double)*40);
    memset(iliili, 0, sizeof(double)*40);
    memset(hhold_prv, 0, sizeof(double)*40);
}

// get number of workers
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

// get number of households
void cblok::get_hhold(int year){
    chold = 0;
    for(map<int, mblok*>::iterator j = mbloks.begin(); j != mbloks.end(); ++j){
        chold += j->second->mblok_hholds.size();
    }
    cout << "hhold = " << chold << endl;
}

// get number of students
void cblok::get_students(int year){
    int t_1 = 0, t_2 = 0, t_3 = 0, t = 0; // t_1 Elementary, t_2 High School, t_3 College, t All
    for(int i = 0; i < cblok_schols.size(); ++i){ //for each school
        schol *sh = cblok_schols[i];
        
        t += sh->student.size(); // tally total school population
        
        switch(sh->level){
            case 'E': t_1 += sh->student.size(); break; // tally elementary school population
            case 'H': t_2 += sh->student.size(); break; // tally hihgschool school population
            case 'B': // if a school with both elementary and highschool students
                for(map<int, agent*>::iterator j = sh->student.begin(); j != sh->student.end(); ++j){ //for each student in the school
                    int age = int(j->second->age/365);
                    if(age <= 13) ++t_1; //add them to the appropriate count by age
                    else ++t_2;
                }
                break;
            case 'C': t_3 += sh->student.size(); break; // tally college school population
        }
    }
    cout << "total students " << year+sim_bg << ": " << t << " " << t_1 << " " << t_2 << " " << t_3 << endl;
}

// get average household size
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

// get pop by age group
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
    out << year+sim_bg << ",";
    for(int j = 0; j < 14; ++j){
        out << cc[j]/(double)cpop << ",";
    }
    out << endl;
    out.close();
}

void cblok::get_sexratio(int year){
    
}

// get sex ratio by age group
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
    out << year+sim_bg << ",";
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

//get LF prevalence by age group
void cblok::get_epidemics(int year, mda_strat strategy){
    //output for calc ICC
    vector<int> village_number;
    vector<int> household_number;
    vector<float> age;
    vector<int> sex;
    vector<int> inf;
    
    //Survey Data
    //2014
    int under_2014_real = 425;
    int over_2014_real = 653;
    int mf_2014_real = 20;

    //2016
    int under_2016_real = 879;
    int over_2016_real = 1617;
    int mf_2016_real = 86;

    // Model Data
    //2016
    int under_2016 = 0;
    int over_2016 = 0;

    int over_2016_ant = 0;
    int under_2016_ant = 0;

    int mf_2016 = 0;
    int mf_2016_positive = 0;

    int icc_2016 = 0;
    //2014
    int under_2014 = 0;
    int over_2014 = 0;

    int over_2014_ant = 0;
    int under_2014_ant = 0;

    int mf_2014 = 0;
    int mf_2014_positive = 0;

    int mhpv = 25; //max houses per village
    int tot_villages = mbloks.size();

    vector<unsigned> keys; // Vector that is used to choose the villages that we want (keys for the map of villages)

    for (unsigned int i = 0; i < tot_villages; ++i) //Filling vector with numbers from 1-number of villages
    {
        keys.push_back(i + 1);
    }
    random_shuffle(keys.begin(),keys.end());

    if (year + sim_bg == 2014) {
        while (under_2014 < under_2014_real || over_2014 < over_2014_real) {
            for (auto const &i: keys) {//iterating through villages
                int n_house = 0;
                for (auto  const& k: mbloks[i]->mblok_hholds){//iterating through households selected village
                    if( drand48()>0.5) { //inducing some randomness into the process, choosing a house
                        n_house += 1;
                        for (auto const &j: k.second->mmbrs){ //now looping through house
                            if( j.second->age/365.0 < 25){
                                if (under_2014 < under_2014_real) {
                                    under_2014 += 1; //tested
                                    if(j.second->epids == 'i' || j.second->epids == 'u'|| drand48() < pow(DailyProbLoseAntigen, year*365 - j.second->LastDayWithAdultWorm) ){ //ant pos
                                        under_2014_ant += 1;
                                        if (mf_2014 < mf_2014_real) {
                                            mf_2014 += 1;
                                            if(j.second->epids == 'i'){
                                                mf_2014_positive += 1;
                                            }
                                        }

                                    }
                                }
                            }
                            else{
                                if (over_2014 < over_2014_real) {
                                    over_2014 += 1; //test
                                    if(j.second->epids == 'i' || j.second->epids == 'u'|| drand48() < pow(DailyProbLoseAntigen, year*365 - j.second->LastDayWithAdultWorm) ){ //ant pos
                                        over_2014_ant += 1;
                                        if (mf_2014 < mf_2014_real) {
                                            mf_2014 += 1;
                                            if(j.second->epids == 'i'){
                                                mf_2014_positive += 1;
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                    if (n_house == mhpv) goto next_village; 
                }
                next_village:;
            }
        }



        data_ant_u = under_2014_ant;
        data_ant_o = over_2014_ant;
        data_mf = mf_2014_positive;
        cout<< "Under 25 tests: " << under_2014 << ", of: " << under_2014_real << ", found: " << under_2014_ant << endl;
        cout << "Over 25 tests: " << over_2014 << ", of: " << over_2014_real << ", found: " << over_2014_ant << endl;
        cout << "MF tests: " << mf_2014 << ", of: "<< mf_2014_real << ", found: " << mf_2014_positive<< endl;
    }

    if (year + sim_bg == 2016) {
        while (under_2016 < under_2016_real || over_2016 < over_2016_real) {
            for (auto const &i: keys) {//iterating through villages
                int n_house = 0;
                for (auto  const& k: mbloks[i]->mblok_hholds){//iterating through households selected village
                    if( drand48()>0.5) { //inducing some randomness into the process, choosing a house
                        n_house += 1;

                        for (auto const &j: k.second->mmbrs){ //now looping through house
                            icc_2016 += 1;
                            if (icc_2016 < (under_2016_real+over_2016_real)){
                                village_number.push_back(i);
                                household_number.push_back(k.second->hid);
                                age.push_back(j.second->age/365); 

                                if (j.second->gendr == 'f'){
                                    sex.push_back(1);
                                } else {
                                    sex.push_back(0);
                                }
                                if (j.second->epids == 'i' || j.second->epids == 'u'|| drand48() < pow(DailyProbLoseAntigen, year*365 - j.second->LastDayWithAdultWorm)){
                                    inf.push_back(1);
                                } else{
                                    inf.push_back(0);
                                }
                            }
                            
                            if( j.second->age/365.0 < 20){
                                if (under_2016 < under_2016_real) {
                                    under_2016 += 1; //tested
                                    if(j.second->epids == 'i' || j.second->epids == 'u'|| drand48() < pow(DailyProbLoseAntigen, year*365 - j.second->LastDayWithAdultWorm) ){ //ant pos
                                        under_2016_ant += 1;
                                        if (mf_2016 < mf_2016_real) {
                                            mf_2016 += 1;
                                            if(j.second->epids == 'i'){
                                                mf_2016_positive += 1;
                                            }
                                        }

                                    }
                                }
                            }
                            else{
                                if (over_2016 < over_2016_real) {
                                    over_2016 += 1; //test
                                    if(j.second->epids == 'i' || j.second->epids == 'u'|| drand48() < pow(DailyProbLoseAntigen, year*365 - j.second->LastDayWithAdultWorm) ){ //ant pos
                                        over_2016_ant += 1;
                                        if (mf_2016 < mf_2016_real) {
                                            mf_2016 += 1;
                                            if(j.second->epids == 'i'){
                                                mf_2016_positive += 1;
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }

                    if (n_house == mhpv) goto next_village2016; 
                }
                next_village2016:;
            }
        }
        cout << "Under 20 tests: " << under_2016 << ", of: " << under_2016_real << ", found: " << under_2016_ant << endl;
        cout << "Over 20 tests: " << over_2016 << ", of: " << over_2016_real << ", found: " << over_2016_ant << endl;
        cout << "MF tests: " << mf_2016 << ", of: "<< mf_2016_real << ", found: " << mf_2016_positive<< endl;
    }

    string survey_out = outdir;    survey_out = survey_out + "survey_" + prv_out_loc;
    string icc_out = outdir;   icc_out = icc_out + "icc_" + prv_out_loc; 

    int number_rows = sex.size();

    if (year+sim_bg == 2016){
        ofstream out;   ifstream in;
        out.open(survey_out.c_str());
        out <<  "Antigen_2014_Under,";
        out <<  "Antigen_2014_Over,";
        out <<  "MF_2014_Total,";
        out <<  "Antigen_2016_Under,";
        out <<  "Antigen_2016_Over,";
        out <<  "MF_2016_Total";
        out << endl;
        out << data_ant_u;
        out << ',' << data_ant_o;
        out << ',' << data_mf;
        out << ',' << under_2016_ant;
        out << ',' << over_2016_ant;
        out << ',' << mf_2016_positive;
        out << endl;
        out.close();
    
        out.open(icc_out.c_str());
        out << "Village,";
        out << "Household,";
        out << "Status,";
        out << "Age,";
        out << "Sex";
        out << endl;
        for (int i = 0; i < number_rows; i++){
            out << village_number[i] << "," << household_number[i] << "," << inf[i] << "," << age[i] << "," << sex[i] << endl;
        }
        out << endl;
        out.close();
    }
}

// output epidemic map (household with infectious individuals) to draw map
void cblok::out_epidemics(int year, int day){
    string file = outdir;   file = file + "data_files/";
    file = file + to_string(year+sim_bg); file = file + "_";
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
}

// output risk map
void cblok::out_riskmap(int year){
    string mapping = outdir;    mapping = mapping + "risk_mapping.csv";
    ifstream in;
    in.open(mapping.c_str());
    
    ofstream out;
    if(!in){
        out.open(mapping.c_str());
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
    for(map<int, mblok*>::iterator j = mbloks.begin(); j != mbloks.end(); ++j){
        out << j->second->sum_mf/sum << ",";
    }
    out << endl;
    out << endl;
    out.close();
}

// get village prevalence
void cblok::out_vg_prv(int year){
    //output village prevalence
    map<int, double> vg_prv;    //output village prevalence
    for(map<int, mblok*>::iterator j = mbloks.begin(); j != mbloks.end(); ++j)
        vg_prv[j->first] = 0;
    
    for(map<int, agent*>::iterator j = inf_indiv.begin(); j != inf_indiv.end(); ++j){
        int mid = j->second->h_d->rdg->mbk->mid;
        vg_prv[mid]++;
    }
    
    string mapping = outdir;    mapping = mapping + "vg_prv.csv";
    ifstream in;
    in.open(mapping.c_str());
    
    ofstream out;
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
    
    out.open(mapping.c_str(), ios::app);
    out << year+sim_bg << ",";
    for(map<int, mblok*>::iterator j = mbloks.begin(); j != mbloks.end(); ++j){
        out << vg_prv[j->first]/(j->second->mblok_fmals.size() + j->second->mblok_males.size()) << ",";
    }
    out << endl;
    out << endl;
    out.close();
}

// get prevalence of infective mosquitoes
void cblok::get_mosquitoes(int year){
    int pools = 5;
    vector<rbldg*> rbldg_ids;
    for(map<int, mblok*>::iterator j = mbloks.begin(); j != mbloks.end(); ++j){
        if(j->second->mblok_ocpy_rbldgs.size() <= pools){
            for(map<int, rbldg*>::iterator k = j->second->mblok_ocpy_rbldgs.begin(); k != j->second->mblok_ocpy_rbldgs.end(); ++k)
                rbldg_ids.push_back(k->second);
        }
        else{
            for(int i = 0; i < pools; ++i){
                int index = rand() % j->second->mblok_ocpy_rbldgs.size();
                map<int, rbldg*>::iterator it = j->second->mblok_ocpy_rbldgs.begin();
                while(index-- > 0) ++it;
                
                rbldg_ids.push_back(it->second);
            }
        }
    }
    
    map<int, vector<double>> mp;
    for(int i = 0; i < rbldg_ids.size(); ++i){
        rbldg *rg = rbldg_ids[i];
        
        double prv_day = 0, prv_night = 0;
        
        rg->t_f = 1.0;
        double t_f2 = 1.0;
        
        //r_neigbors
        for(int i = 0; i < rg->r_neigh.size(); ++i){
            rbldg *r_2 = rg->r_neigh[i];
            if(r_2->h_d == NULL) continue;
            rg->t_f += 1 - rg->r_neigh_d[i]/r_r;
            t_f2 += 1 - rg->r_neigh_d[i]/r_r;
        }
        
        //s_neigbors
        for(int i = 0; i < rg->s_neigh.size(); ++i){
            schol *sh = rg->s_neigh[i];
            if(sh->student.size() == 0) continue;
            rg->t_f += 1 - rg->s_neigh_d[i]/r_r;
        }
        
        //w_neigbors
        for(int i = 0; i < rg->w_neigh.size(); ++i){
            workp *wp = rg->w_neigh[i];
            if(wp->workers.size() == 0) continue;
            rg->t_f += 1 - rg->w_neigh_d[i]/r_r;
        }
        
        double f = 1.0 / rg->t_f;
        prv_day += f * rg->day_p;
        
        f = 1.0 / t_f2;
        prv_night += f * rg->night_p;
        
        for(int i = 0; i < rg->r_neigh.size(); ++i){
            rbldg *r_2 = rg->r_neigh[i];
            if(r_2->h_d == NULL) continue;
            
            f = (1 - rg->r_neigh_d[i]/r_r) / rg->t_f;
            prv_day += f * r_2->day_p;
            
            f = (1 - rg->r_neigh_d[i]/r_r) / t_f2;
            prv_night += f * r_2->night_p;
        }
        
        for(int i = 0; i < rg->s_neigh.size(); ++i){
            schol *sh = rg->s_neigh[i];
            if(sh->student.size() == 0) continue;
            f = (1 - rg->s_neigh_d[i]/r_r) / rg->t_f;
            prv_day += f * sh->day_p;
        }
        
        for(int i = 0; i < rg->w_neigh.size(); ++i){
            workp *wp = rg->w_neigh[i];
            if(wp->workers.size() == 0) continue;
            f = (1 - rg->w_neigh_d[i]/r_r) / rg->t_f;
            prv_day += f * wp->day_p;
        }
        
        prv_day *= (r_i * s_l3);
        prv_night *= (r_i * s_l3);
        
        double pp = (prv_day + prv_night) / 2;
        mp[rg->mbk->mid].push_back(pp);
    }
    
    string data = outdir;
    data = data + syn_mosquitoes;
    
    ofstream out(data.c_str(), ios::app);
    
    for(map<int, mblok*>::iterator j = mbloks.begin(); j != mbloks.end(); ++j){
        map<int, vector<double>>::iterator it = mp.find(j->first);
        
        if(it == mp.end()) out << " ,";
        else{
            double p = 0;
            for(int i = 0; i < it->second.size(); ++i){
                p += it->second[i];
            }
            
            out << p/it->second.size() << ",";
        }
    }
    out << endl;
    out.close();
}