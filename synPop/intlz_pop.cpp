//
//  blockInitialize.cpp
//  synPop
//
//  Created by Sting Xu on 6/11/17.
//  Copyright © 2017 Sting Xu. All rights reserved.
//

#include "blocks.h"

using namespace std;

cblok::cblok(int cid, string cname, double lat, double log){
    this->cid = cid;
    this->cname = cname;
    this->lat = lat;
    this->log = log;
    
    cpop = 0;
    next_aid = 1;
    next_hid = 1;
    next_mid = 1;
    meshblocks = 0;
    
    read_demgrphcs();
    read_parmtrs();
}

void cblok::reset_cpop(){
    //clear population
    for(map<int, agent*>::iterator j = cblok_pop.begin(); j != cblok_pop.end(); ++j)
        delete j->second;
    fmal_marrd.clear();
    cblok_pop.clear();
    
    //clear meshblocks, households, buildings
    for(map<int, mblok*>::iterator j = mbloks.begin(); j != mbloks.end(); ++j)
        delete j->second;
    mbloks.clear();
    cblok_rbldgs_vcnt.clear();
    
    //clear coordiantes
    for(map<int, double*>::iterator j = mblok_crdnt.begin(); j != mblok_crdnt.end(); ++j)
        delete [] j->second;
    mblok_crdnt.clear();
    
    //clear meshblock index
    mbloksIndexA.clear();
    mbloksIndexB.clear();
    
    //clear meshblock distance matrix
    for(int i = 0; i < meshblocks; ++i){
        delete [] euclid_dist[i];
        delete [] road_dist[i];
    }
    
    //clear age groups data
    for(map<int, agrps*>::iterator j = mblok_agrps.begin(); j != mblok_agrps.end(); ++j)
        delete j->second;
    mblok_agrps.clear();
    
    //clear others
    mblok_fpops.clear();
    mblok_mpops.clear();
    mblok_hholds.clear();
    
    //re-call all the contruction functions
    cpop = 0;
    next_aid = 1;
    next_hid = 1;
    next_mid = 1;
    
    read_demgrphcs();
    read_parmtrs();
}

void cblok::read_demgrphcs(){
    ifstream in;
    string line, file;
    //1. read villages and create index
    file = datadir;    file = file + villages;
    in.open(file.c_str());
    
    //skip the description
    while(getline(in, line)){
        if(line[0] == '*') continue;
        if(line.length() <= 1) continue;  //empty line with carriage return
        break;
    }
    
    while(getline(in, line)){
        if(mbloksIndexA.find(line) == mbloksIndexA.end()){
            mbloksIndexA.insert(pair<string, int>(line, next_mid));
            mbloksIndexB.insert(pair<int, string>(next_mid++, line));
        }
    }
    in.close();
    
    //2. reading sex ratio by age group
    file = datadir;    file = file + sex_ratio_age;
    in.open(file.c_str());
    
    //skip the description
    while(getline(in, line)){
        if(line[0] == '*') continue;
        if(line.length() <= 1) continue;  //empty line with carriage return
        break;
    }
    
    int ii = 0;
    double pp = 0;
    while(in >> pp){
        sex_ratio[ii++] = pp/(pp+100);
    }
    in.close();
    
    //3. reading village pop by age group
    file = datadir;    file = file + village_pop_age;
    in.open(file.c_str());
    
    //skip the description
    while(getline(in, line)){
        if(line[0] == '*') continue;
        if(line.length() <= 1) continue;  //empty line with carriage return
        break;
    }
    
    char *str = new char[line.size()+1];
    std::strcpy(str, line.c_str());
    char *p = std::strtok(str, " ,");
    
    //age group segments
    vector<int> age_seg_dn;
    vector<int> age_seg_up;
    
    p = std::strtok(NULL, ", ");
    while(p){
        string s = p;
        int age_dn = atoi(&s.substr(0, s.find('-'))[0]);
        int age_up = atoi(&s.substr(s.find('-')+1, s.size()-s.find('-')-1)[0]);
        age_seg_dn.push_back(age_dn);
        age_seg_up.push_back(age_up);
        p = std::strtok(NULL, ", ");
    }
    delete [] str;
    
    while(getline(in,line)){
        str = new char[line.size()+1];
        std::strcpy(str, line.c_str());
        p = std::strtok(str, ",");  //village name may have space
        
        //deal with age group record
        int mid = mbloksIndexA[p];
        if(mblok_agrps.find(mid) != mblok_agrps.end()){
            cout << "village name: " << p << " already exist!" << endl;
            exit(1);
        }
        mblok_agrps.insert(pair<int, agrps*>(mid, new agrps()));
        
        //reading age group population
        p = std::strtok(NULL, ", ");
        int i = 0;
        while(p){
            mblok_agrps[mid]->agroups.push_back(new agrp(age_seg_dn[i], age_seg_up[i], atoi(p)));
            p = std::strtok(NULL, ", ");
            ++i;
        }
        delete []str;
    }
    age_seg_dn.clear(); age_seg_dn.shrink_to_fit();
    age_seg_up.clear(); age_seg_up.shrink_to_fit();
    in.close();
    
    //4. read village pop by gender, male first
    file = datadir;    file = file + village_pop_gender;
    in.open(file.c_str());
    
    //skip the description
    while(getline(in, line)){
        if(line[0] == '*') continue;
        if(line.length() <= 1) continue;  //empty line with carriage return
        break;
    }
    
    getline(in, line);          //skip header
    while(getline(in,line)){
        str = new char[line.size()+1];
        std::strcpy(str, line.c_str());
        p = std::strtok(str, ",");  //village name may have space
        
        //deal with gender record
        int mid = mbloksIndexA[p];
        if(mblok_mpops.find(mid) != mblok_mpops.end()){
            cout << "village name: " << p << " already exist, read gender data!" << endl;
            exit(1);
        }
        
        int males = 0, females = 0;
        p = std::strtok(NULL, ", ");    males = atoi(p);
        p = std::strtok(NULL, ", ");    females = atoi(p);
        
        mblok_mpops.insert(pair<int, int>(mid, males));
        mblok_fpops.insert(pair<int, int>(mid, females));
        
        delete []str;
    }
    in.close();
    
    //5. read population by age group
    file = datadir;    file = file + population_age;
    in.open(file.c_str());
    
    //skip the description
    while(getline(in, line)){
        if(line[0] == '*') continue;
        if(line.length() <= 1) continue;  //empty line with carriage return
        break;
    }
    
    getline(in, line);          //header
    
    ii = 0;
    while(getline(in, line)){
        str = new char[line.size()+1];
        std::strcpy(str, line.c_str());
        p = std::strtok(str, ", ");     male_by_agrp[ii] = atoi(p);
        p = std::strtok(NULL, ", ");    fmal_by_agrp[ii++] = atoi(p);
        delete []str;
    }
    in.close();
    
    //6. read household units
    file = datadir;    file = file + village_units;
    in.open(file.c_str());
    
    //skip the description
    while(getline(in, line)){
        if(line[0] == '*') continue;
        if(line.length() <= 1) continue;  //empty line with carriage return
        break;
    }
    
    while(getline(in, line)){
        str = new char[line.size()+1];
        std::strcpy(str, line.c_str());
        
        p = std::strtok(str, ",");  //village name may have space
        int mid = mbloksIndexA[p];
        
        if(mblok_hholds.find(mid) != mblok_hholds.end()){
            cout << "village already exist, in reading housing units data" << endl;
            cout << "village name: " << p << endl;
            exit(1);
        }
        
        p = std::strtok(NULL, " ,");
        mblok_hholds.insert(pair<int, int>(mid, atoi(p)));
        
        delete []str;
    }
    in.close();
    
    //7. read household types
    file = datadir;    file = file + household_types;
    in.open(file.c_str());
    
    //skip the description
    while(getline(in, line)){
        if(line[0] == '*') continue;
        if(line.length() <= 1) continue;  //empty line with carriage return
        break;
    }
    
    while(getline(in, line)){
        str = new char[line.size()+1];
        std::strcpy(str, line.c_str());
        
        p = std::strtok(str, " ,");
        if(std::strlen(p) > 1){
            cout << "hhold type length > 1, in reading hhold type data" << endl;
            cout << "hhold type: " << p << endl;
            exit(1);
        }
        int t = std::atoi(p);
        p = std::strtok(NULL, " ,");
        int count = atoi(p);
        hhold_types[t] = count;
        
        delete []str;
    }
    in.close();
    
    //8. read marital by age group
    file = datadir;    file = file + marital_age;
    in.open(file.c_str());
    
    //skip the description
    while(getline(in, line)){
        if(line[0] == '*') continue;
        if(line.length() <= 1) continue;  //empty line with carriage return
        break;
    }
    
    getline(in, line);          //header
    
    ii = 0;
    while(getline(in, line)){
        str = new char[line.size()+1];
        std::strcpy(str, line.c_str());
        p = std::strtok(str, ", ");     male_marital_age[ii] = atoi(p);
        p = std::strtok(NULL, ", ");    fmal_marital_age[ii++] = atoi(p);
        delete []str;
    }
    in.close();
    
    //9. read excluded village
    file = datadir;     file = file + village_excluded;
    in.open(file.c_str());
    
    getline(in, line);  //header
    while(getline(in,line)){
        int mid = mbloksIndexA[line];
        mblok_agrps.erase(mid);
        mblok_mpops.erase(mid);
        mblok_fpops.erase(mid);
        mblok_hholds.erase(mid);
        mbloksIndexB.erase(mid);
        mbloksIndexA.erase(line);
    }
    in.close();
    meshblocks = (int)mbloksIndexA.size();
    
    //10. read meshblock lat & long
    file = datadir;     file = file + village_coordinates;
    in.open(file.c_str());
    
    getline(in, line); //header
    while(getline(in, line)){
        str = new char[line.size()+1];
        std::strcpy(str, line.c_str());
        
        p = std::strtok(str, ",");  //village name may have space
        map<string, int>::iterator k = mbloksIndexA.find(p);
        if(k == mbloksIndexA.end()) continue;
        
        int mid = k->second;
        p = std::strtok(NULL, ",");     double lat = atof(p);
        p = std::strtok(NULL, ",");     double log = atof(p);
        
        double *r = new double[2];
        r[0] = lat;     r[1] = log;
        mblok_crdnt.insert(pair<int, double*>(mid, r));
    }
    in.close();
    
    if(mblok_crdnt.size() < mbloksIndexA.size()){
        cout << "meshblock coordinates are missing in " << village_coordinates << endl;
        exit(1);
    }
    
    //calculate cpop
    for(map<int, int>::iterator j = mblok_mpops.begin(); j != mblok_mpops.end(); ++j){
        int mid = j->first;
        cpop += mblok_mpops[mid] + mblok_fpops[mid];
    }
    
    //calculate smoothed population by age group
    calc_smoothed_pop_agrp(male_by_agrp, male_by_age);
    calc_smoothed_pop_agrp(fmal_by_agrp, fmal_by_age);
    
    //calculate marital probability by age
    calc_marital_prob();
}

void cblok::read_parmtrs(){
    ifstream in;
    string line, file;
    
    //1. reading daily birth rate for females [18, 44]
    file = parameters;     file = file + fertility;
    in.open(file.c_str());
    if(!in){
        cout << "open " << file << " failed" << endl;
        exit(1);
    }
    getline(in, line);
    while(getline(in, line)){
        char *str = new char[line.size()+1];
        std::strcpy(str, line.c_str());
        char *p = NULL;
        
        p = std::strtok(str, ",");      int yy = atoi(p) - 2010;
        p = std::strtok(NULL, ",");     double asfr15_19 = atof(p);
        p = std::strtok(NULL, ",");     double asfr20_24 = atof(p);
        p = std::strtok(NULL, ",");     double asfr25_29 = atof(p);
        p = std::strtok(NULL, ",");     double asfr30_34 = atof(p);
        p = std::strtok(NULL, ",");     double asfr35_39 = atof(p);
        p = std::strtok(NULL, ",");     double asfr40_44 = atof(p);
        
        fertlty[yy][0] = asfr15_19;
        fertlty[yy][1] = asfr20_24;
        fertlty[yy][2] = asfr25_29;
        fertlty[yy][3] = asfr30_34;
        fertlty[yy][4] = asfr35_39;
        fertlty[yy][5] = asfr40_44;
        
        delete []str;
    }
    in.close();
    
    //2. reading annual net migrants
    file = parameters;    file = file + net_pop_loss;
    in.open(file.c_str());
    if(!in){
        cout << "open " << file << " failed" << endl;
        exit(1);
    }
    getline(in, line);
    
    while(getline(in,line)){
        char *str = new char[line.size()+1];
        std::strcpy(str, line.c_str());
        char *p = NULL;
        p = std::strtok(str, ",");      int yy = atoi(p) - 2010;
        p = std::strtok(NULL, ",");     int net = atoi(p);
        pop_loss[yy] = net;
        
        delete []str;
    }
    in.close();
    
    //reading daily death rate for male
    file = parameters;     file = file + "mortality_male.csv";
    in.open(file.c_str());
    if(!in){
        cout << "open " << file << " failed" << endl;
        exit(1);
    }
    getline(in, line);
    
    int ii = 0;
    while(getline(in, line)){
        char *str = new char[line.size()+1];
        std::strcpy(str, line.c_str());
        char *p = NULL;
        p = std::strtok(str, ",");
        p = std::strtok(NULL, ",");
        p = std::strtok(NULL, ",");
        mmortlty[ii++] = atof(p);
        
        delete []str;
    }
    in.close();
    
    //3. reading daily death rate for female
    file = parameters;     file = file + "mortality_female.csv";
    in.open(file.c_str());
    if(!in){
        cout << "open " << file << " failed" << endl;
        exit(1);
    }
    
    getline(in, line);
    
    ii = 0;
    while(getline(in, line)){
        char *str = new char[line.size()+1];
        std::strcpy(str, line.c_str());
        char *p = NULL;
        p = std::strtok(str, ",");
        p = std::strtok(NULL, ",");
        p = std::strtok(NULL, ",");
        fmortlty[ii++] = atof(p);
        
        delete []str;
    }
    in.close();
}

void cblok::bld_mbloks(){
    for(map<int, double*>::iterator j = mblok_crdnt.begin(); j != mblok_crdnt.end(); ++j){
        int mid = j->first;
        double lat = j->second[0], log = j->second[1];
        mbloks.insert(pair<int, mblok*>(mid, new mblok(mid, this, lat, log)));
    }
}

void cblok::bld_cblok_pop(){
    for(map<int, mblok*>::iterator j = mbloks.begin(); j != mbloks.end(); ++j){
        mblok *mbk = j->second;
        mbk->intlz_pop();
    }
}

void cblok::bld_cblok_hhold(){
    
}

void cblok::hndl_land_data(){
    
}

void cblok::hndl_rbldg(string ff, int low, int upper, int min_dist){
    
}

void cblok::hndl_bbldg(string ff, int low, int upper, int min_dist){
    
}

void cblok::allct_rbldgs(){
    /*bb->vg->vacant_rbldgs.erase(bb->bldg_id);
    bb->vg->occupied_rbldgs.insert(pair<int, resident_bldg*>(bb->bldg_id, bb));
    ct->vacant_rbldgs.erase(bb->bldg_id);*/
}

void cblok::allct_bbldgs(){
    
}

void cblok::calc_bldg_dist(){
    
}

void cblok::calc_smoothed_pop_agrp(int *p, int *res){
    //calculate basic probability
    double *tmp = new double[80];
    for(int i = 0; i < 80; ++i)
        tmp[i] = (double)p[int(i/5)]/5;
    
    double l = 0, m = 0, h = 0;
    int t;
    for(int i = 0; i < 16; ++i){
        t = p[i];
        m = tmp[i*5+2];
        if(i > 0) l = tmp[i*5-3];
        if(i < 15) h = tmp[i*5+7];
        
        double r = (m-l)/5, s = (h-m)/5;
        if(i == 0) r = s;
        if(i == 15) s = r;
        
        res[i*5+2] = int((t+3*r-3*s)/5+0.5);      t -= res[i*5+2];
        res[i*5+1] = int(res[i*5+2]-r+0.5);       t -= res[i*5+1];
        res[i*5] = int(res[i*5+2]-2*r+0.5);       t -= res[i*5];
        res[i*5+3] = int(res[i*5+2]+s+0.5);       t -= res[i*5+3];
        res[i*5+4] = int(res[i*5+2]+2*s+0.5);     t -= res[i*5+4];
        
        while(t < 0){
            -- res[i*5 + rand()%5];
            ++t;
        }
        
        while(t > 0){
            ++ res[i*5 + rand()%5];
            --t;
        }
    }
}

void cblok::calc_marital_prob(){
    int L = (max_marital_age-min_marital_age)+1;   //45
    for(int i = 0; i < L; ++i){
        male_marital_prob[i] = 1 - marital_b_m;
        fmal_marital_prob[i] = 1 - marital_b_f;
    }
    
    for(int i = 0; i < L; ++i){
        if(i < 2){
            male_marital_prob[i] = 1 - sqrt(male_marital_age[0]);
            fmal_marital_prob[i] = 1 - sqrt(fmal_marital_age[0]);
        }
        else if(i < 42){
            int j = int((i-2)/5), k = j + 1;
            if(male_marital_age[k] < male_marital_age[j]) male_marital_prob[i] = 1 - pow(male_marital_age[k]/male_marital_age[j], 0.2);
            if(fmal_marital_age[k] < fmal_marital_age[j]) fmal_marital_prob[i] = 1 - pow(fmal_marital_age[k]/fmal_marital_age[j], 0.2);
        }
    }
}
