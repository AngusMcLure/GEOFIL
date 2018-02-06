//
//  blockInitialize.cpp
//  synPop
//
//  Created by Sting Xu on 6/11/17.
//  Copyright © 2017 Sting Xu. All rights reserved.
//

#include "block.h"
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
    
    euclid_dst = NULL;
    road_dst = NULL;
    
    init = pop_reload();
    
    if(!init){
        read_demgrphcs(); //population not created
        bld_mbloks();
        bld_cblok_pop();
        bld_cblok_hhold();
        
        string file = config;   file = file + cname;    file = file + ".init";
        ofstream out(file.c_str());
        
        out << std::setprecision(2) << std::setiosflags(std::ios::fixed);
        
        out << "TRUE" << endl;
        out << cpop << endl;
        out << next_aid << endl;
        out << next_hid << endl;
        out << next_mid << endl;
        out << meshblocks << endl;
        
        for(map<int, mblok*>::iterator j = mbloks.begin(); j != mbloks.end(); ++j){
            out << j->first << "," << mbloksIndexB[j->first] << "," << j->second->log << "," << j->second->lat << endl;
        }
        out.close();
        
        for(map<int, mblok*>::iterator j = mbloks.begin(); j != mbloks.end(); ++j){
            mblok *mbk = j->second;
            
            string mbk_str = mbloksIndexB[mbk->mid];
            int pos = int(mbk_str.find('/'));
            if(pos != string::npos) mbk_str[pos] = '_';
            
            //out population
            file = config;  file = file + mbk_str;  file = file + "_pop.init";
            
            out.open(file.c_str());
            out << "ID,age,gender,marriage,births" << endl;
            for(map<int, agent*>::iterator k = mbk->mblok_males.begin(); k != mbk->mblok_males.end(); ++k){
                agent *cur = k->second;
                out << cur->aid << "," << cur->age << "," << cur->gendr << "," << cur->margs << "," << cur->births << endl;
            }
            
            for(map<int, agent*>::iterator k = mbk->mblok_fmals.begin(); k != mbk->mblok_fmals.end(); ++k){
                agent *cur = k->second;
                out << cur->aid << "," << cur->age << "," << cur->gendr << "," << cur->margs << "," << cur->births << endl;
            }
            out.close();
            
            file = config;  file = file + mbk_str;   file = file + "_hhold.init";
            out.open(file.c_str());
            
            out << "hhold_ID,size,holder_ID,gender" << endl;
            for(map<int, hhold*>::iterator k = mbk->mblok_hholds.begin(); k != mbk->mblok_hholds.end(); ++k){
                hhold *cur = k->second;
                out << cur->hid << "," << cur->size << "," << cur->hldr->aid << "," << cur->hldr->gendr << endl;
                for(map<int, agent*>::iterator r = cur->mmbrs.begin(); r != cur->mmbrs.end(); ++r){
                    out << r->first << "," << r->second->gendr << endl;
                }
            }
            out.close();
            
            //out family unit
            file = config;  file = file + mbk_str;   file = file + "_unit.init";
            out.open(file.c_str());
            out << "ID,gender,spw_ID,gender,child_num" << endl;
            for(map<int, agent*>::iterator k = mbk->mblok_males.begin(); k != mbk->mblok_males.end(); ++k){
                agent *cur = k->second;
                out << cur->aid << "," << cur->gendr << ",";
                
                if(cur->spw != NULL) out << cur->spw->aid << "," << cur->spw->gendr << ",";
                else out << "-" << "," << "-" << ",";
                
                out << cur->chdr.size() << endl;
                
                for(map<int, agent*>::iterator r = cur->chdr.begin(); r != cur->chdr.end();++r){
                    out << r->first << "," << r->second->gendr << "," << mbloksIndexB[mbk->mid] << endl;
                }
            }
            
            for(map<int, agent*>::iterator k = mbk->mblok_fmals.begin(); k != mbk->mblok_fmals.end(); ++k){
                agent *cur = k->second;
                if(cur->spw != NULL) continue;      //already output by males
                
                out << cur->aid << "," << cur->gendr << ",-,-,";
                
                out << cur->chdr.size() << endl;
                
                for(map<int, agent*>::iterator r = cur->chdr.begin(); r != cur->chdr.end();++r){
                    out << r->first << "," << r->second->gendr << "," << mbloksIndexB[mbk->mid] << endl;
                }
            }
            out.close();
        }
    }
    
    hndl_land_data();
    read_parmtrs();
}

bool cblok::pop_reload(){
    //calculate hhold_threshold
    hhold_threshold = 1;
    while(true){
        bool rupture = false;
        for(int j = 1; j <= hhold_threshold/2; ++j){
            double p_1 = ztpoisson(hhold_threshold, lambda);
            double p_2 = ztpoisson(j, lambda);
            double p_3 = ztpoisson(hhold_threshold-j, lambda);
            
            if(p_1 <= p_2*p_3){
                rupture = true;
                break;
            }
        }
        
        if(rupture == true) break;
        else ++hhold_threshold;
    }
    cout << "hhold threshold rupture: " << hhold_threshold << endl;
    
    string file = config;   file = file + cname;    file = file + ".init";
    ifstream in(file.c_str());
    
    if(!in) return false;
    
    string line;
    getline(in, line);
    if(line != "TRUE") return false;
    
    getline(in, line);      cpop = atoi(line.c_str());
    getline(in, line);      next_aid = atoi(line.c_str());
    getline(in, line);      next_hid = atoi(line.c_str());
    getline(in, line);      next_mid = atoi(line.c_str());
    getline(in, line);      meshblocks = atoi(line.c_str());
    
    while(getline(in, line)){
        char *str = new char[line.size()+1];
        std::strcpy(str, line.c_str());
        
        char *p = std::strtok(str, ",");        int id = atoi(p);
        p = std::strtok(NULL, ",");             string mbk = p;
        p = std::strtok(NULL, ",");             double log = atof(p);
        p = std::strtok(NULL, ",");             double lat = atof(p);
        
        mbloksIndexA.insert(pair<string, int>(mbk, id));
        mbloksIndexB.insert(pair<int, string>(id, mbk));
        mbloks.insert(pair<int, mblok*>(id, new mblok(id, this, lat, log)));
        
        delete []str;
    }
    in.close();
    
    for(map<int, mblok*>::iterator j = mbloks.begin(); j != mbloks.end(); ++j){
        string mbk_str = mbloksIndexB[j->first];
        int pos = int(mbk_str.find('/'));
        if(pos != string::npos) mbk_str[pos] = '_';
        
        file = config;   file = file + mbk_str;   file = file + "_pop.init";
        in.open(file.c_str());
        
        string line;
        getline(in, line);
        while(getline(in, line)){
            char *str = new char[line.size()+1];
            std::strcpy(str, line.c_str());
            
            char *p = std::strtok(str, ",");        int id = atoi(p);
            p = std::strtok(NULL, ",");             int age = atoi(p);
            p = std::strtok(NULL, ",");             char gender = p[0];
            p = std::strtok(NULL, ",");             char mrgs = p[0];
            p = std::strtok(NULL, ",");             int births = atoi(p);
            
            agent *pp = new agent(id, age, gender, mrgs);
            pp->births = births;
            j->second->add_member(pp);
            
            if(mrgs == 'm' && gender == 'f'){
                if(int(age/365) < 50) fmal_cbrs[births].insert(pair<int, agent*>(id, pp));
                else fmal_marry.insert(pair<int, agent*>(id, pp));
            }
                
            delete []str;
        }
        in.close();
    }
    
    for(map<int, mblok*>::iterator j = mbloks.begin(); j != mbloks.end(); ++j){
        string mbk_str = mbloksIndexB[j->first];
        int pos = int(mbk_str.find('/'));
        if(pos != string::npos) mbk_str[pos] = '_';
        
        file = config;   file = file + mbk_str;   file = file + "_unit.init";
        in.open(file.c_str());
        
        string line;
        getline(in, line);
        while(getline(in, line)){
            char *str = new char[line.size()+1];
            std::strcpy(str, line.c_str());
            
            char *p = std::strtok(str, ",");    int id = atoi(p);
            p = std::strtok(NULL, ",");         char gender = p[0];
            p = std::strtok(NULL, ",");         int spw_id = -1;        if(p[0] != '-') spw_id = atoi(p);
            p = std::strtok(NULL, ",");         char spw_gender = p[0];
            
            agent *p_m = NULL;
            if(gender == 'm') p_m = j->second->mblok_males[id];
            else p_m = j->second->mblok_fmals[id];
            
            agent *p_s = NULL;
            if(spw_id != -1){
                if(spw_gender == 'm') p_s = j->second->mblok_males[spw_id];
                else p_s = j->second->mblok_fmals[spw_id];
                
                p_m->spw = p_s;
                p_s->spw = p_m;
            }
            
            p = std::strtok(NULL, ",");
            int child_num = atoi(p);
            
            delete []str;
            
            while(child_num-- > 0){
                getline(in, line);
                str = new char[line.size()+1];
                std::strcpy(str, line.c_str());
                
                p = std::strtok(str, ",");      int c_id = atoi(p);
                p = std::strtok(NULL, ",");     char gender = p[0];
                p = std::strtok(NULL, ",");     string mbk = p;
                
                agent *p_c = NULL;
                if(gender == 'm') p_c = mbloks[mbloksIndexA[mbk]]->mblok_males[c_id];
                else p_c = mbloks[mbloksIndexA[mbk]]->mblok_fmals[c_id];
                
                p_m->add_child(p_c);
                if(p_m->gendr == 'm') p_c->dad = p_m;
                else p_c->mom = p_m;
                
                if(p_s != NULL){
                    p_s->add_child(p_c);
                    if(p_s->gendr == 'm') p_c->dad = p_s;
                    else p_c->mom = p_s;
                }
                
                delete []str;
            }
        }
        in.close();
    }
    
    for(map<int, mblok*>::iterator j = mbloks.begin(); j != mbloks.end(); ++j){
        string mbk_str = mbloksIndexB[j->first];
        int pos = int(mbk_str.find('/'));
        if(pos != string::npos) mbk_str[pos] = '_';
        
        file = config;   file = file + mbk_str;   file = file + "_hhold.init";
        in.open(file.c_str());
        
        string line;
        getline(in, line);
        
        while(getline(in, line)){
            char *str = new char[line.size()+1];
            std::strcpy(str, line.c_str());
            
            char *p = std::strtok(str, ",");        int id = atoi(p);
            p = std::strtok(NULL, ",");             int size = atoi(p);
            p = std::strtok(NULL, ",");             int holder = atoi(p);
            p = std::strtok(NULL, ",");             char gender = p[0];
            
            agent *h_holder = NULL;
            if(gender == 'm') h_holder = j->second->mblok_males[holder];
            else h_holder = j->second->mblok_fmals[holder];
            
            hhold *h_hold = new hhold(id, size, h_holder);
            
            j->second->add_hhold(h_hold);
            delete []str;
            
            while(size-- > 0){
                getline(in, line);
                char *str = new char[line.size()+1];
                std::strcpy(str, line.c_str());
                
                char *p = std::strtok(str, ",");        int m_id = atoi(p);
                p = std::strtok(NULL, ",");             char m_gender = p[0];
                
                agent *p_m = NULL;
                if(m_gender == 'm') p_m = j->second->mblok_males[m_id];
                else p_m = j->second->mblok_fmals[m_id];
                
                h_hold->add_member(p_m);
                
                delete []str;
            }
            
            h_hold->hldr = h_holder;
            h_hold->exp = false;
        }
        in.close();
    }
    
    return true;
}

void cblok::reset_cpop(){
    //clear population
    fmal_marry.clear();
    for(int i = 0; i < 11; ++i) fmal_cbrs[i].clear();
    
    //clear meshblocks, households, buildings
    for(map<int, mblok*>::iterator j = mbloks.begin(); j != mbloks.end(); ++j)
        delete j->second;
    mbloks.clear();
    cblok_vcnt_rbldgs.clear();
    
    //clear coordiantes
    for(map<int, double*>::iterator j = mblok_crdnt.begin(); j != mblok_crdnt.end(); ++j)
        delete [] j->second;
    mblok_crdnt.clear();
    
    //clear meshblock index
    mbloksIndexA.clear();
    mbloksIndexB.clear();
    
    //clear meshblock distance matrix
    /*for(int i = 0; i < meshblocks; ++i){
        delete [] euclid_dist[i];
        delete [] road_dist[i];
    }*/
    
    //clear age groups data
    for(map<int, agrps*>::iterator j = mblok_agrps.begin(); j != mblok_agrps.end(); ++j)
        delete j->second;
    mblok_agrps.clear();
    
    //clear others
    mblok_fpops.clear();
    mblok_mpops.clear();
    
    //re-call all the contruction functions
    cpop = 0;
    next_aid = 1;
    next_hid = 1;
    next_mid = 1;
    meshblocks = 0;
    
    if(!pop_reload()){
        cout << "reload pop err" << endl;
        exit(1);
    }
    
    hndl_land_data();
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
    
    getline(in, line);
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
    
    int last_vg = -1;
    char sign = 'p';
    while(getline(in,line)){
        str = new char[line.size()+1];
        std::strcpy(str, line.c_str());
        p = std::strtok(str, ",");  //village name may have space
        
        //deal with age group record
        int mid = mbloksIndexA[p];
        
        if(mid == last_vg){
            if(sign == 'p') sign = 'm';
            if(sign == 'm') sign = 'f';
        }
        else{
            //a new village information
            sign = 'p';
            mblok_agrps.insert(pair<int, agrps*>(mid, new agrps()));
        }
        
        //reading age group population
        p = std::strtok(NULL, ", ");
        int ii = 0;
        while(p){
            if(sign == 'p'){
                mblok_agrps[mid]->age_dn[ii] = age_seg_dn[ii];
                mblok_agrps[mid]->age_up[ii] = age_seg_up[ii];
                mblok_agrps[mid]->pop[ii] += atoi(p);
            }
            else if(sign == 'm') mblok_agrps[mid]->male[ii] += atoi(p);
            else if(sign == 'f') mblok_agrps[mid]->female[ii] += atoi(p);
            
            p = std::strtok(NULL, ", ");
            
            if(age_seg_dn[ii] < max_ages-4) ++ii;       //last age group is 75-79
        }
        
        if(sign == 'p') mblok_agrps[mid]->age_up[vg_agrps-1] = max_ages;      //adjust to be consistent with population by age group
        
        delete []str;
        
        last_vg = mid;
    }
    age_seg_dn.clear(); age_seg_dn.shrink_to_fit();
    age_seg_up.clear(); age_seg_up.shrink_to_fit();
    in.close();
    
    //4. read village pop by gender, male first
    file = datadir;    file = file + village_pop_gender;
    in.open(file.c_str());
    
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
    
    //7.1 read marital male by age group
    file = datadir;    file = file + marital_male;
    in.open(file.c_str());
    
    //skip the description
    getline(in, line);          //header
    
    ii = 0;
    while(getline(in, line)){
        str = new char[line.size()+1];
        std::strcpy(str, line.c_str());
        p = std::strtok(str, ",");
        p = std::strtok(NULL, ",");     male_single[ii] = atof(p);
        p = std::strtok(NULL, ",");     male_married[ii] = atof(p);
        p = std::strtok(NULL, ",");     male_widowed[ii] = atof(p);
        p = std::strtok(NULL, ",");     male_divorce[ii] = atof(p);
        ++ii;
        delete []str;
    }
    in.close();
    
    //7.2 read marital female by age group
    file = datadir;    file = file + marital_female;
    in.open(file.c_str());
    
    //skip the description
    getline(in, line);          //header
    
    ii = 0;
    while(getline(in, line)){
        str = new char[line.size()+1];
        std::strcpy(str, line.c_str());
        p = std::strtok(str, ",");
        p = std::strtok(NULL, ",");     fmal_single[ii] = atof(p);
        p = std::strtok(NULL, ",");     fmal_married[ii] = atof(p);
        p = std::strtok(NULL, ",");     fmal_widowed[ii] = atof(p);
        p = std::strtok(NULL, ",");     fmal_divorce[ii] = atof(p);
        ++ii;
        delete []str;
    }
    in.close();
    
    //8. read children ever born by age groups
    file = datadir;    file = file + number_of_children;
    in.open(file.c_str());
    
    //skip the description
    getline(in, line);          //header
    
    ii = 0;
    while(getline(in, line)){
        str = new char[line.size()+1];
        std::strcpy(str, line.c_str());
        p = std::strtok(str, ",");
        p = std::strtok(NULL, ",");     child_number_by_agrps[ii] = atof(p);
        ++ii;
        delete []str;
    }
    in.close();
    
    //calculate smoothed children ever born
    calc_smoothed_agrp(child_number_by_agrps, 10, child_number_by_age, 50);
    
    //9. read live birth by age of mother and birth-order
    file = datadir;    file = file + live_birth;
    in.open(file.c_str());
    
    getline(in, line);
    
    while(getline(in, line)){
        str = new char[line.size()+1];
        std::strcpy(str, line.c_str());
        
        p = std::strtok(str, ",");      int i = atoi(p)-1;
        
        ii = 0;
        p = std::strtok(NULL, ",");
        while(p){
            live_birth_order_by_agrps[i][ii++] = atoi(p);
            p = std::strtok(NULL, ",");
        }
        
        delete []str;
    }
    in.close();
    //calculate smoothed live birth
    for(int i = 0; i < max_births; ++i){
        calc_smoothed_pop_agrp(live_birth_order_by_agrps[i], 7, live_birth_order_by_age[i], 35);
        
        //calculate age interval
        int age_dn = 0, age_up = 34;
        while(live_birth_order_by_age[i][age_dn] == 0) ++age_dn;
        while(live_birth_order_by_age[i][age_up] == 0) --age_up;
        
        live_birth_order_interval[i][0] = age_dn;
        live_birth_order_interval[i][1] = age_up;
    }
    
    int t_1 = 0;
    for(int i = 0; i < 10; ++i){
        t_1 += live_birth_order_by_agrps[i][7];
        
        for(int j = 0; j < 7; ++j){
            live_birth_age_pro[i][j] = live_birth_order_by_agrps[i][j]/(double)live_birth_order_by_agrps[i][7];
        }
    }
    for(int i = 0; i < 10; ++i) live_birth_order_pro[i] = live_birth_order_by_agrps[i][7]/(double)t_1;
    
    //10. read excluded village
    file = datadir;     file = file + village_excluded;
    in.open(file.c_str());
    
    getline(in, line);  //header
    while(getline(in,line)){
        int mid = mbloksIndexA[line];
        mblok_agrps.erase(mid);
        mblok_mpops.erase(mid);
        mblok_fpops.erase(mid);
        mbloksIndexB.erase(mid);
        mbloksIndexA.erase(line);
    }
    in.close();
    meshblocks = (int)mbloksIndexA.size();
    
    //re-id all mbloks
    int cur_mid = 1, count = 0;
    while(count < meshblocks){
        if(mbloksIndexB.find(cur_mid) == mbloksIndexB.end()){
            int mid = cur_mid + 1;
            while(mbloksIndexB.find(mid) == mbloksIndexB.end()) ++mid;
            
            string vg = mbloksIndexB[mid];
            agrps *p = mblok_agrps[mid];
            int mpop = mblok_mpops[mid];
            int fpop = mblok_fpops[mid];
            
            mblok_agrps.erase(mid);     mblok_agrps.insert(pair<int, agrps*>(cur_mid, p));
            mblok_mpops.erase(mid);     mblok_mpops.insert(pair<int, int>(cur_mid, mpop));
            mblok_fpops.erase(mid);     mblok_fpops.insert(pair<int, int>(cur_mid, fpop));
            mbloksIndexA.erase(vg);     mbloksIndexA.insert(pair<string, int>(vg, cur_mid));
            mbloksIndexB.erase(mid);    mbloksIndexB.insert(pair<int, string>(cur_mid, vg));
        }
        
        ++cur_mid;
        ++count;
    }
    
    next_mid = cur_mid;
    
    //11. read meshblock lat & long
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
        p = std::strtok(NULL, ",");     double asfr45_49 = atof(p);
        
        fertlty[yy][0] = asfr15_19;
        fertlty[yy][1] = asfr20_24;
        fertlty[yy][2] = asfr25_29;
        fertlty[yy][3] = asfr30_34;
        fertlty[yy][4] = asfr35_39;
        fertlty[yy][5] = asfr40_44;
        fertlty[yy][6] = asfr45_49;
        
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
        p = std::strtok(NULL, ",");     double net = atof(p);
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
    
    //reading daily death rate for female
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
    
    //read marital probability for male and female
    file = datadir;    file = file + marital_male;
    in.open(file.c_str());
    getline(in, line);          //header
    
    ii = 0;
    while(getline(in, line)){
        char *str = new char[line.size()+1];
        std::strcpy(str, line.c_str());
        
        char *p = std::strtok(str, ",");
        p = std::strtok(NULL, ",");     male_single[ii] = atof(p);
        p = std::strtok(NULL, ",");     male_married[ii] = atof(p);
        p = std::strtok(NULL, ",");     male_widowed[ii] = atof(p);
        p = std::strtok(NULL, ",");     male_divorce[ii] = atof(p);
        ++ii;
        delete []str;
    }
    in.close();

    file = datadir;    file = file + marital_female;
    in.open(file.c_str());
    getline(in, line);          //header
    
    ii = 0;
    while(getline(in, line)){
        char *str = new char[line.size()+1];
        std::strcpy(str, line.c_str());
        char *p = std::strtok(str, ",");
        p = std::strtok(NULL, ",");     fmal_single[ii] = atof(p);
        p = std::strtok(NULL, ",");     fmal_married[ii] = atof(p);
        p = std::strtok(NULL, ",");     fmal_widowed[ii] = atof(p);
        p = std::strtok(NULL, ",");     fmal_divorce[ii] = atof(p);
        ++ii;
        delete []str;
    }
    in.close();
    
    //calculate marital probability by age
    calc_marital_prob();
    
    //read live birth by age of mother and birth-order
    file = datadir;    file = file + live_birth;
    in.open(file.c_str());
    
    getline(in, line);
    
    while(getline(in, line)){
        char *str = new char[line.size()+1];
        std::strcpy(str, line.c_str());
        
        char *p = std::strtok(str, ",");      int i = atoi(p)-1;
        
        ii = 0;
        p = std::strtok(NULL, ",");
        while(p){
            live_birth_order_by_agrps[i][ii++] = atoi(p);
            p = std::strtok(NULL, ",");
        }
        
        delete []str;
    }
    in.close();
    
    for(int i = 0; i < max_births; ++i){        //calculate smoothed live birth
        calc_smoothed_pop_agrp(live_birth_order_by_agrps[i], 7, live_birth_order_by_age[i], 35);
        
        //calculate age interval
        int age_dn = 0, age_up = 34;
        while(live_birth_order_by_age[i][age_dn] == 0) ++age_dn;
        while(live_birth_order_by_age[i][age_up] == 0) --age_up;
        
        live_birth_order_interval[i][0] = age_dn;
        live_birth_order_interval[i][1] = age_up;
    }
    
    int t_1 = 0;
    for(int i = 0; i < 10; ++i){
        t_1 += live_birth_order_by_agrps[i][7];
        
        for(int j = 0; j < 7; ++j){
            live_birth_age_pro[i][j] = live_birth_order_by_agrps[i][j]/(double)live_birth_order_by_agrps[i][7];
        }
    }
    for(int i = 0; i < 10; ++i) live_birth_order_pro[i] = live_birth_order_by_agrps[i][7]/(double)t_1;
    
    //reading road_dst & euclid_dst
    int len = meshblocks*(meshblocks-1)/2;
    road_dst = new double[len];     memset(road_dst, 0, sizeof(double)*len);
    euclid_dst = new double[len];   memset(euclid_dst, 0, sizeof(double)*len);
    
    file = datadir;     file = file + vil_road;
    in.open(file.c_str());
    
    getline(in, line);
    vector<string> mbk_vec;
    {
        char *str = new char[line.size()+1];
        std::strcpy(str, line.c_str());
        
        char *p = std::strtok(str, ",");
        while(p != NULL){
            mbk_vec.push_back(p);
            p = std::strtok(NULL, ",");
        }
        delete []str;
        
        while(getline(in, line)){
            str = new char[line.size()+1];
            std::strcpy(str, line.c_str());
            
            p = std::strtok(str, ",");
            string src = p;
            int src_id = mbloksIndexA[src];
            
            int index = 0;
            p = std::strtok(NULL, ",");
            while(p != NULL){
                string tag = mbk_vec[index++];
                int tag_id = mbloksIndexA[tag];
                
                if(tag_id > src_id){
                    double dd = atof(p);
                    
                    int ii = (src_id-1)*(meshblocks*2-src_id)/2 + tag_id-src_id - 1;
                    road_dst[ii] = dd;
                }
                
                p = std::strtok(NULL, ",");
            }
            delete []str;
        }
    }
    mbk_vec.clear();
    mbk_vec.shrink_to_fit();
    in.close();
    
    file = datadir;     file = file + vil_euclid;
    in.open(file.c_str());
    
    getline(in, line);
    {
        char *str = new char[line.size()+1];
        std::strcpy(str, line.c_str());
        
        char *p = std::strtok(str, ",");
        while(p != NULL){
            mbk_vec.push_back(p);
            p = std::strtok(NULL, ",");
        }
        delete []str;
        
        while(getline(in, line)){
            str = new char[line.size()+1];
            std::strcpy(str, line.c_str());
            
            p = std::strtok(str, ",");
            string src = p;
            int src_id = mbloksIndexA[src];
            
            int index = 0;
            p = std::strtok(NULL, ",");
            while(p != NULL){
                string tag = mbk_vec[index++];
                int tag_id = mbloksIndexA[tag];
                
                if(tag_id > src_id){
                    double dd = atof(p);
                    
                    int ii = (src_id-1)*(meshblocks*2-src_id)/2 + tag_id-src_id - 1;
                    euclid_dst[ii] = dd;
                }
                
                p = std::strtok(NULL, ",");
            }
            delete []str;
        }
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
    //create cpop based on smoothed male/female by age group
    if(!init){
        //calculate smoothed population by age group
        calc_smoothed_pop_agrp(male_by_agrp, age_grps, male_by_age, max_ages+1);
        calc_smoothed_pop_agrp(fmal_by_agrp, age_grps, fmal_by_age, max_ages+1);
        
        for(int i = 0; i < max_ages+1; ++i){
            //calculate index in village population age group
            int index = 0;
            int *age_dn = mblok_agrps.begin()->second->age_dn;
            int *age_up = mblok_agrps.begin()->second->age_up;
            
            while(true){
                if(i >= age_dn[index] && i <= age_up[index]) break;
                ++index;
            }
            
            //create pop vector
            int mm = male_by_age[i], ff = fmal_by_age[i];
            while(mm-- > 0){
                int id = -1;        //id not initialized
                int age = i*365 + rand()%365;
                char gender = 'm';
                
                agent *p = new agent(id, age, gender);
                mvec[index].push_back(p);
            }
            
            while(ff-- > 0){
                int id = -1;        //id not initialized
                int age = i*365 + rand()%365;
                char gender = 'f';
                
                agent *p = new agent(id, age, gender);
                fvec[index].push_back(p);
            }
        }
    }
    
    for(map<int, mblok*>::iterator j = mbloks.begin(); j != mbloks.end(); ++j){
        mblok *mbk = j->second;
        mbk->bld_mblok_pop();
    }
    
    for(int i = 0; i < vg_agrps; ++i){
        while(mvec[i].size() > 0){
            delete mvec[i].back();
            mvec[i].pop_back();
        }
        
        while(fvec[i].size() > 0){
            delete fvec[i].back();
            fvec[i].pop_back();
        }
    }
}

void cblok::bld_cblok_hhold(){
    for(map<int, mblok*>::iterator j = mbloks.begin(); j != mbloks.end(); ++j){
        mblok *mbk = j->second;
        mbk->bld_hhold();
    }
}

void cblok::hndl_land_data(){
    string data = config;  data = data + AS_rbldgs;
    ifstream in(data.c_str());
    
    if(!in){
        data = datadir;     data = data + AS_rbldgs_origin;
        hndl_rbldg(data, 30, 300, 20);
        allct_rbldgs();
        
        data = config;  data = data + AS_rbldgs;
        ofstream out(data.c_str());
        out << "bldg_id,log,lat,area,meshblock,hhold_id" << endl;
        out << std::setprecision(2) << std::setiosflags(std::ios::fixed);
        
        for(map<int, mblok*>::iterator j = mbloks.begin(); j != mbloks.end(); ++j){
            mblok *mbk = j->second;
            for(map<int, rbldg*>::iterator k = mbk->mblok_ocpy_rbldgs.begin(); k != mbk->mblok_ocpy_rbldgs.end(); ++k){
                rbldg *p = k->second;
                out << p->bid << "," << p->log << "," << p->lat << "," << p->area << "," << mbk->cbk->mbloksIndexB[mbk->mid] << "," << p->h_d->hid << endl;
            }
            
            for(map<int, rbldg*>::iterator k = mbk->mblok_vcnt_rbldgs.begin(); k != mbk->mblok_vcnt_rbldgs.end(); ++k){
                rbldg *p = k->second;
                out << p->bid << "," << p->log << "," << p->lat << "," << p->area << "," << mbk->cbk->mbloksIndexB[mbk->mid] << "," << "-1" << endl;
            }
        }
        out.close();
    }
    else{
        string line;
        getline(in, line);  //skip the header
        
        while(getline(in, line)){
            char *str = new char[line.size()+1];
            std::strcpy(str, line.c_str());
            
            char *p = std::strtok(str, ",");    int bid = atoi(p);
            p = std::strtok(NULL, ",");         double log = atof(p);
            p = std::strtok(NULL, ",");         double lat = atof(p);
            p = std::strtok(NULL, ",");         double area = atof(p);
            p = std::strtok(NULL, ",");         string mbk = p;
            p = std::strtok(NULL, ",");         int hid = atoi(p);
            
            mblok *mbk_p = mbloks[mbloksIndexA[mbk]];
            rbldg *bg = new rbldg(bid, log, lat, area, mbk_p, this);
            
            if(hid != -1) mbk_p->mblok_hholds[hid]->asg_rbldg(bg);
            
            mbk_p->add_rbldg(bg, bg->h_d);
            delete []str;
        }
        in.close();
    }
}

void cblok::hndl_rbldg(string ff, int low, int upper, int min_dist){
    ifstream in(ff.c_str());
    
    string line;
    getline(in, line);  //skip the header
    
    while(getline(in, line)){
        char *str = new char[line.size()+1];
        std::strcpy(str, line.c_str());
        
        char *p = std::strtok(str, ",");
        int bid = atoi(p);
        
        p = std::strtok(NULL, ",");
        string mbk = p;
        
        if(mbloksIndexA.find(mbk) == mbloksIndexA.end()){
            cout << "err: mblock " << mbk << " non exist" << endl;
            exit(1);
        }
        
        p = std::strtok(NULL, ",");      double area = atof(p);
        p = std::strtok(NULL, ",");      double log = atof(p);
        p = std::strtok(NULL, ",");      double lat = atof(p);
        
        mblok *mbk_p = mbloks[mbloksIndexA[mbk]];
        rbldg *bg = new rbldg(bid, log, lat, area, mbk_p, this);
        mbk_p->add_rbldg(bg);
        delete []str;
    }
    in.close();
    
    cout << "bldg number with unique is " << cblok_vcnt_rbldgs.size() << endl;
    
    int max_bid = 0;
    for(map<int, rbldg*>::iterator j = cblok_vcnt_rbldgs.begin(); j != cblok_vcnt_rbldgs.end();){
        if(max_bid < j->first) max_bid = j->first;
        
        rbldg *p = j->second;
        if(p->mbk->mblok_hholds.size() >= p->mbk->mblok_vcnt_rbldgs.size()){      //no enough building in current village
            ++j;    continue;
        }
        
        double dist = pow(min_dist, 2); int ii = -1;      //dist as 20^2
        for(map<int, rbldg*>::iterator k = cblok_vcnt_rbldgs.begin(); k != cblok_vcnt_rbldgs.end(); ++k){
            if(k->first == p->bid) continue;
            
            double log_dif = fabs(k->second->log - p->log);
            double lat_dif = fabs(k->second->lat - p->lat);
            if(log_dif >= dist || lat_dif >= dist) continue;
            
            if(k->second->area < p->area) continue;
            
            double diff = pow(log_dif, 2) + pow(lat_dif, 2);
            if(diff < dist){
                dist = diff;
                ii = k->first;
            }
        }
        
        if(ii == -1){
            ++j;    continue;
        }
        
        bool merging = false;
        rbldg *q = cblok_vcnt_rbldgs[ii];
        
        if(q->area + p->area >= upper){      //unreasonable size
            ++j;    continue;
        }
        
        if(fabs(p->log - q->log) < 5 || fabs(p->lat - q->lat) < 5) merging = true;
        else if(p->area < q->area*0.2) merging = true;
        
        if(merging == true){
            q->area += p->area;
            p->mbk->mblok_vcnt_rbldgs.erase(p->bid);
            cblok_vcnt_rbldgs.erase(j++);
        }
        else ++j;
    }
    cout << "bldgs number with merging is " << cblok_vcnt_rbldgs.size() << endl;
    
    for(map<int, rbldg*>::iterator j = cblok_vcnt_rbldgs.begin(); j != cblok_vcnt_rbldgs.end();){
        if(j->second->area > upper || j->second->area < low){
            j->second->mbk->mblok_vcnt_rbldgs.erase(j->first);
            cblok_vcnt_rbldgs.erase(j++);
        }
        else ++j;
    }
    cout << "bldgs number with reasonable size is " << cblok_vcnt_rbldgs.size() << endl;
    
    for(map<int, mblok*>::iterator j = mbloks.begin(); j != mbloks.end(); ++j){
        if(j->second->mblok_hholds.size() > j->second->mblok_vcnt_rbldgs.size()){
            int k = int(j->second->mblok_hholds.size() - j->second->mblok_vcnt_rbldgs.size());
            
            int loop = 0;
            while(k > 0){
                ++loop;
                if(loop > j->second->mblok_vcnt_rbldgs.size()*3) break;
                
                int ii  = rand() % j->second->mblok_vcnt_rbldgs.size(), jj = rand() % j->second->mblok_vcnt_rbldgs.size();
                while(jj == ii) jj = rand() % j->second->mblok_vcnt_rbldgs.size();
                
                map<int, rbldg*>::iterator p = j->second->mblok_vcnt_rbldgs.begin(), q = j->second->mblok_vcnt_rbldgs.begin();
                while(ii-- > 0) ++p;
                while(jj-- > 0) ++q;
                
                double d = pow(p->second->log - q->second->log, 2) + pow(p->second->lat - q->second->lat, 2);
                if(d < pow(min_dist,2)) continue;
                
                double log = (p->second->log + q->second->log)/2, lat = (p->second->lat + q->second->lat)/2, area = (p->second->area + q->second->area)/2;
        
                ++max_bid;
                rbldg *pp = new rbldg(max_bid, log, lat, area, j->second, this);
                    
                j->second->add_rbldg(pp);
                
                --k;
            }
        }
    }
    
    cout << "bldgs number with reconstruction is " << cblok_vcnt_rbldgs.size() << endl;
}

void cblok::hndl_bbldg(string ff, int low, int upper, int min_dist){
    
}

void cblok::allct_rbldgs(){
    struct _comp_bldg{
        bool operator() (const rbldg *lhs, const rbldg *rhs){
            return lhs->area > rhs->area;
        }
    } _bigger_area;
    
    struct _comp_hhold{
        bool operator() (const hhold *lhs, const hhold *rhs){
            return lhs->size > rhs->size;
        }
    } _bigger_size;
    
    for(map<int, mblok*>::iterator j = mbloks.begin(); j != mbloks.end(); ++j){
        if(j->second->mblok_vcnt_rbldgs.size() < j->second->mblok_hholds.size()){
            cout << "err: no enough bldgs in meshblock " << j->first << endl;
            exit(1);
        }
        
        vector<rbldg*> v_1;
        for(map<int, rbldg*>::iterator k = j->second->mblok_vcnt_rbldgs.begin(); k != j->second->mblok_vcnt_rbldgs.end(); ++k){
            v_1.push_back(k->second);
        }
        stable_sort(v_1.begin(), v_1.end(), _bigger_area);
        
        vector<hhold*> v_2;
        for(map<int, hhold*>::iterator k = j->second->mblok_hholds.begin(); k != j->second->mblok_hholds.end(); ++k){
            if(k->second->rdg == NULL) v_2.push_back(k->second);
        }
        stable_sort(v_2.begin(), v_2.end(), _bigger_size);
        
        int a = int((v_1.size()-v_2.size())/2), b = int((v_1.size()-v_2.size()))-a;
        while(a-- > 0) v_1.erase(v_1.begin());
        while(b-- > 0) v_1.erase(v_1.begin()+v_1.size()-1);
        
        for(int i = 0; i < v_2.size(); ++i){
            hhold *h_hold = v_2[i];
            rbldg *bb = v_1[i];
            h_hold->asg_rbldg(bb);
        }
        
        v_1.clear(); v_1.shrink_to_fit();
        v_2.clear(); v_2.shrink_to_fit();
    }
}

void cblok::allct_bbldgs(){
    
}

void cblok::calc_bldg_dist(){
    
}

void cblok::calc_smoothed_pop_agrp(int *p, int pL, int *res, int rL){
    //calculate basic probability
    double *tmp = new double[rL];
    for(int i = 0; i < rL; ++i)
        tmp[i] = (double)p[int(i/5)]/5;
    
    double l = 0, m = 0, h = 0;
    int t;
    for(int i = 0; i < pL; ++i){
        t = p[i];
        m = tmp[i*5+2];
        if(i > 0) l = tmp[i*5-3];
        if(i < pL-1) h = tmp[i*5+7];
        
        double r = (m-l)/5, s = (h-m)/5;
        if(i == 0) r = s;
        if(i == pL-1) s = r;
        
        res[i*5+2] = int((t+3*r-3*s)/5+0.5);    res[i*5+2] = res[i*5+2]<0 ? 0:res[i*5+2];      t -= res[i*5+2];
        res[i*5+1] = int(res[i*5+2]-r+0.5);     res[i*5+1] = res[i*5+1]<0 ? 0:res[i*5+1];      t -= res[i*5+1];
        res[i*5] = int(res[i*5+2]-2*r+0.5);     res[i*5] = res[i*5]<0 ? 0:res[i*5];            t -= res[i*5];
        res[i*5+3] = int(res[i*5+2]+s+0.5);     res[i*5+3] = res[i*5+3]<0 ? 0:res[i*5+3];      t -= res[i*5+3];
        res[i*5+4] = int(res[i*5+2]+2*s+0.5);   res[i*5+4] = res[i*5+4]<0 ? 0:res[i*5+4];      t -= res[i*5+4];
        
        while(t < 0){
            for(int j = 0; j < 5; ++j){
                if(res[i*5+j] > 0){
                    --res[i*5+j];
                    ++t;
                }
                if(t == 0) break;
            }
        }
        
        while(t > 0){
            for(int j = 0; j < 5; ++j){
                ++res[i*5+j];
                --t;
                
                if(t == 0) break;
            }
        }
    }
}

void cblok::calc_smoothed_agrp(double *p, int pL, double *res, int rL){
    //calculate basic probability
    double *tmp = new double[rL];
    for(int i = 0; i < rL; ++i)
        tmp[i] = (double)p[int(i/5)];
    
    double l = 0, m = 0, h = 0;
    double t;
    for(int i = 0; i < pL; ++i){
        t = p[i];
        m = tmp[i*5+2];
        if(i > 0) l = tmp[i*5-3];
        if(i < pL-1) h = tmp[i*5+7];
        
        double r = (m-l)/5, s = (h-m)/5;
        if(i == 0) r = s;
        if(i == pL-1) s = r;
        
        res[i*5+2] = (5*t+3*r-3*s)/5;    t -= res[i*5+2];
        res[i*5+1] = res[i*5+2]-r;       t -= res[i*5+1];
        res[i*5] = res[i*5+2]-2*r;       t -= res[i*5];
        res[i*5+3] = res[i*5+2]+s;       t -= res[i*5+3];
        res[i*5+4] = res[i*5+2]+2*s;     t -= res[i*5+4];
    }
    
    for(int i = 0; i < rL; ++i){
        res[i] = res[i]<0 ? 0 : res[i];
    }
}

void cblok::calc_marital_prob(){
    //calculate prob with power-law fitted line
    for(int i = 0; i < marital_ages; ++i){
        male_mrg_prob[i] = 1 - marital_b_m;
        fmal_mrg_prob[i] = 1 - marital_b_f;
    }
    
    //re-calculate with real marital data, martial at center of interval
    for(int i = 0; i < marital_ages; ++i){
        if(i < 3){
            male_mrg_prob[i] = 1 - pow(male_single[0], double(1/3));
            fmal_mrg_prob[i] = 1 - pow(fmal_single[0], double(1/3));
        }
        else if(i < 43){
            int j = int((i-3)/5), k = j + 1;
            if(male_single[k] < male_single[j]) male_mrg_prob[i] = 1 - pow(male_single[k]/male_single[j], 0.2);
            if(fmal_single[k] < fmal_single[j]) fmal_mrg_prob[i] = 1 - pow(fmal_single[k]/fmal_single[j], 0.2);
        }
    }
}

mblok::mblok(int mid, cblok *cbk, double lat, double log){
    this->mid = mid;
    this->cbk = cbk;
    this->lat = lat;
    this->log = log;
}

mblok::~mblok(){
    cbk = NULL;
    mblok_males.clear();
    mblok_fmals.clear();
    
    for(map<int, hhold*>::iterator j = mblok_hholds.begin(); j != mblok_hholds.end(); ++j)
        delete j->second;
    mblok_hholds.clear();
    
    for(map<int, rbldg*>::iterator j = mblok_ocpy_rbldgs.begin(); j != mblok_ocpy_rbldgs.end(); ++j)
        delete j->second;
    mblok_ocpy_rbldgs.clear();
    
    for(map<int, rbldg*>::iterator j = mblok_vcnt_rbldgs.begin(); j != mblok_vcnt_rbldgs.end(); ++j)
        delete j->second;
    mblok_vcnt_rbldgs.clear();
    
    for(map<int, wbldg*>::iterator j = mblok_wbldgs.begin(); j != mblok_wbldgs.end(); ++j)
        delete j->second;
    mblok_wbldgs.clear();
}

void mblok::bld_mblok_pop(){
    int mm = cbk->mblok_mpops[mid], ff = cbk->mblok_fpops[mid];
    agrps *pp = cbk->mblok_agrps[mid];
    bld_pop(mm, ff, pp);
}

void mblok::bld_hhold(){
    //initialize marriage status
    vector<agent*> m_svec, m_mvec, m_wvec, m_dvec;
    vector<agent*> f_svec, f_mvec, f_wvec, f_dvec;
    vector<agent*> chld;
    
    vector<unit*> famly;
    cout << cbk->mbloksIndexB[mid] << endl;
    bld_family_unit(m_mvec, m_svec, m_dvec, m_wvec, f_mvec, f_svec, f_dvec, f_wvec, chld, famly);
    
    struct _comp_size{
        bool operator() (unit *p, unit *q){
            return p->u_size() < q->u_size();
        }
    } _smaller;
    
    stable_sort(famly.begin(), famly.end(), _smaller);
    
    for(int j = 0; j < famly.size();){
        if(famly[j]->u_size() >= 2) break;
        
        if(int(famly[j]->mother->age/365) < 30){    //female under 30 cannot have child above 15
            m_svec.push_back(famly[j]->mother);
            
            delete famly[j];
            famly.erase(famly.begin()+j);
        }
        else ++j;
    }
    
    m_svec.insert(m_svec.end(), f_svec.begin(), f_svec.end());
    f_svec.clear(); f_svec.shrink_to_fit();
    
    struct _comp_age{
        bool operator() (const agent *p, const agent *q){ return (p->age < q->age);}
    } _younger;
    
    stable_sort(m_svec.begin(), m_svec.end(), _younger);

    for(int j = 0; j < famly.size(); ++j){
        famly[j]->mother->births = int(famly[j]->child.size()+famly[j]->avail_ages.size());
    }

    int pop = int(mblok_males.size() + mblok_fmals.size());
    int allocated = 0;
    while (famly.size() + m_svec.size() > 0){
        int h_size = ztpoisson(lambda);        //current hhold size
        //cout << cbk->mbloksIndexB[mid] << ": " << pop << " " << allocated << " " << h_size << endl;
        if (pop - allocated <= h_size){
            hhold *h_hold = new hhold(cbk->next_hid++);
            
            while (famly.size() > 0){
                unit *cur = famly.back();
                agent *p = cur->father;
                agent *q = cur->mother;
                
                while (cur->avail_ages.size() > 0){
                    int s = int(cur->avail_ages.size());
                    int age = cur->avail_ages[s - 1];
                    
                    int max = int(q->age / 365) - 15;
                    if (s >= 2) max = cur->avail_ages[s - 2];
                    
                    int min = 15;
                    if (cur->child.size() > 0) min = int(cur->child.front()->age / 365 + 1);
                    
                    int pos = binary_search(m_svec, age);
                    if (pos == -1){
                        for (int j = min; j < max; ++j){
                            pos = binary_search(m_svec, j);
                            if (pos != -1) break;
                        }
                    }
                    
                    if (pos != -1){
                        agent *b = m_svec[pos];
                        
                        cur->child.insert(cur->child.begin(), b);
                        m_svec.erase(m_svec.begin() + pos);
                    }
                    
                    cur->avail_ages.pop_back();
                }
                
                if (p != NULL){
                    if (h_hold->hldr == NULL) h_hold->hldr = p;
                    h_hold->add_member(p);
                    h_hold->add_member(q);
                }
                else{
                    if (h_hold->hldr == NULL) h_hold->hldr = q;
                    h_hold->add_member(q);
                }
                
                for (int j = 0; j < cur->child.size(); ++j){
                    agent *b = cur->child[j];
                    
                    if (p != NULL){
                        p->add_child(b);
                        b->dad = p;
                    }
                    
                    q->add_child(b);
                    b->mom = q;
                    
                    h_hold->add_member(b);
                }
                
                if(q->margs == 'm'){
                    if(int(q->age/365) < 50) cbk->fmal_cbrs[q->births].insert(pair<int, agent*>(q->aid, q));
                    else cbk->fmal_marry.insert(pair<int, agent*>(q->aid, q));
                }
                
                delete cur;
                famly.pop_back();
            }
            
            while (m_svec.size() > 0){
                agent *p = m_svec.back();
                if(h_hold->hldr == NULL) h_hold->hldr = p;
                h_hold->add_member(p);
                m_svec.pop_back();
            }
            
            h_hold->size = int(h_hold->mmbrs.size());
            allocated += h_hold->size;
            add_hhold(h_hold);
            h_hold->exp = false;
        }
        else{
            vector<unit*> u_vec;    //units in the hhold
            while (h_size > 0 && famly.size() > 0){
                unit *cur = NULL;
                int pos, s_1;
                
                if (famly.size() > 0 && h_size < famly.front()->u_size()) break;        //check first element
                
                for (pos = int(famly.size()-1); pos >= 0; --pos){
                    if (famly[pos]->u_size() <= h_size) break;
                }
                
                if (pos != -1){
                    cur = famly[pos];
                    s_1 = cur->u_size();
                    
                    famly.erase(famly.begin() + pos);
                    u_vec.push_back(cur);
                    
                    h_size -= s_1;
                }
            }
            
            agent *senior = NULL;
            vector<vector<agent*>> c_vecs;
            if (h_size > 0){
                for (int i = 0; i < u_vec.size(); ++i){
                    unit *cur = u_vec[i];
                    agent *p = cur->father;
                    agent *q = cur->mother;
                    
                    if (h_size > 0 && senior == NULL){      //add unit's senior member - p's or q's father or mother
                        if (cur->father != NULL){
                            for (int j = int(m_svec.size()-1); j >= 0; --j){
                                int age = int(m_svec[j]->age / 365);
                                if (age >= int(p->age / 365 + 15) && age <= int(p->age / 365 + 44)){
                                    senior = m_svec[j];
                                    m_svec.erase(m_svec.begin() + j);
                                    break;
                                }
                            }
                        }
                        else{
                            for (int j = int(m_svec.size()-1); j >= 0; --j){
                                int age = int(m_svec[j]->age / 365);
                                if (age >= int(q->age / 365 + 15) && age <= int(q->age / 365 + 44)){
                                    senior = m_svec[j];
                                    m_svec.erase(m_svec.begin() + j);
                                    break;
                                }
                            }
                        }
                        
                        if (senior != NULL) --h_size;
                    }
                    
                    if(h_size == 0) break;
                    
                    vector<agent*> vec;     //add adult-children for each unit
                    for (int j = int(cur->avail_ages.size()-1); j >= 0 && h_size > 0; --j){
                        int age = cur->avail_ages[j];
                        
                        int max = int(q->age / 365) - 15;
                        if (j > 0) max = cur->avail_ages[j - 1];
                        
                        int min = 15;
                        if (vec.size() > 0) min = int(vec.back()->age / 365 + 1);
                        else if (cur->child.size() > 0) min = int(cur->child.front()->age / 365 + 1);
                        
                        int pos = binary_search(m_svec, age);
                        if (pos == -1){
                            for (int j = min; j < max; ++j){
                                pos = binary_search(m_svec, j);
                                if (pos != -1) break;
                            }
                        }
                        
                        if (pos != -1){
                            agent *b = m_svec[pos];
                            vec.push_back(b);    --h_size;
                            m_svec.erase(m_svec.begin() + pos);
                        }
                    }
                    c_vecs.push_back(vec);
                    
                    if (h_size == 0) break;
                }
            }
            
            vector<agent*> a_vec;       //other members
            while (h_size > 0 && m_svec.size() > 0){
                agent *p = m_svec.back();
                a_vec.push_back(p);    --h_size;
                m_svec.pop_back();
            }
            
            if (h_size != 0){                               //not successful
                for (int j = 0; j < u_vec.size(); ++j){
                    famly.push_back(u_vec[j]);
                }
                stable_sort(famly.begin(), famly.end(), _smaller);
                u_vec.clear();    u_vec.shrink_to_fit();
                
                if (senior != NULL){
                    m_svec.push_back(senior);
                    senior = NULL;
                }
                
                for (int i = 0; i < c_vecs.size(); ++i){
                    for (int j = 0; j < c_vecs[i].size(); ++j){
                        m_svec.push_back(c_vecs[i][j]);
                    }
                    c_vecs[i].clear();    c_vecs[i].shrink_to_fit();
                }
                c_vecs.clear();    c_vecs.shrink_to_fit();
                
                while(a_vec.size() > 0){
                    m_svec.push_back(a_vec.back());
                    a_vec.pop_back();
                }
                a_vec.shrink_to_fit();
                
                stable_sort(m_svec.begin(), m_svec.end(), _younger);
            }
            else{
                hhold *h_hold = new hhold(cbk->next_hid++);
                
                for (int i = 0; i < c_vecs.size(); ++i){
                    unit *cur = u_vec[i];
                    
                    for (int j = 0; j < c_vecs[i].size(); ++j){
                        agent *b = c_vecs[i][j];
                        cur->child.push_back(b);
                    }
                    c_vecs[i].clear();    c_vecs[i].shrink_to_fit();
                }
                c_vecs.clear();    c_vecs.shrink_to_fit();
                
                for (int i = 0; i < u_vec.size(); ++i){
                    unit *cur = u_vec[i];
                    agent *p = cur->father;
                    agent *q = cur->mother;
                    
                    if (p != NULL){
                        if (h_hold->hldr == NULL) h_hold->hldr = p;
                        h_hold->add_member(p);
                        h_hold->add_member(q);
                    }
                    else{
                        if (h_hold->hldr == NULL) h_hold->hldr = q;
                        h_hold->add_member(q);
                    }
                    
                    for (int j = 0; j < cur->child.size(); ++j){
                        agent *b = cur->child[j];
                        
                        if (p != NULL){
                            p->add_child(b);
                            b->dad = p;
                        }
                        
                        q->add_child(b);
                        b->mom = q;
                        
                        h_hold->add_member(b);
                    }
                    
                    if(q->margs == 'm'){
                        if(int(q->age/365) < 50) cbk->fmal_cbrs[q->births].insert(pair<int, agent*>(q->aid, q));
                        else cbk->fmal_marry.insert(pair<int, agent*>(q->aid, q));
                    }
                    
                    delete cur;
                }
                u_vec.clear();    u_vec.shrink_to_fit();
                
                if (senior != NULL){
                    if(h_hold->hldr == NULL) h_hold->hldr = senior;
                    h_hold->add_member(senior);
                }
                
                for(int i = 0; i < a_vec.size(); ++i){
                    if(h_hold->hldr == NULL) h_hold->hldr = a_vec[i];
                    h_hold->add_member(a_vec[i]);
                }
                a_vec.clear();  a_vec.shrink_to_fit();
                
                h_hold->size = int(h_hold->mmbrs.size());
                allocated += h_hold->size;
                add_hhold(h_hold);
                h_hold->exp = false;
            }
        }
    }
    
    m_svec.clear();     m_svec.shrink_to_fit();
    famly.clear();      famly.shrink_to_fit();
}

void mblok::bld_pop(int mm, int ff, agrps *pp){
    int *pop_vg = pp->pop;
    int *male_vg = pp->male;
    int *female_vg = pp->female;
    
    bool no_data = true;
    for(int i = 0; i < vg_agrps; ++i){
        if(male_vg[i] > 0){
            no_data = false;
            break;
        }
    }
    
    //male & female pop by age group in village unavailable
    if(no_data){
        for(int i = 0; i < vg_agrps; ++i){
            if(pop_vg[i] == 0) continue;
            
            double males = cbk->mvec[i].size();
            double fmals = cbk->fvec[i].size();
            
            female_vg[i] = int(pop_vg[i]*fmals/(males+fmals)+0.5);  ff -= female_vg[i];
            male_vg[i] = pop_vg[i] - female_vg[i];                  mm -= male_vg[i];
        }
        
        while(mm < 0){
            int index = -1, diff = 0;
            for(int i = 0; i < vg_agrps; ++i){
                int df = male_vg[i]-female_vg[i];
                
                if(index == -1 && female_vg[i] < cbk->fvec[i].size() && male_vg[i] > 0){
                    diff = df;
                    index = i;
                    continue;
                }
                
                if(df > diff && female_vg[i] < cbk->fvec[i].size() && male_vg[i] > 0){
                    diff = df;
                    index = i;
                }
            }
            
            --male_vg[index];       ++mm;
            ++female_vg[index];     --ff;
        }
        
        while(mm > 0){
            int index = -1, diff = 0;
            for(int i = 0; i < vg_agrps; ++i){
                int df = female_vg[i]-male_vg[i];
                
                if(index == -1 && male_vg[i] < cbk->mvec[i].size() && female_vg[i] > 0){
                    diff = df;
                    index = i;
                    continue;
                }
                
                if(df > diff && male_vg[i] < cbk->mvec[i].size() && female_vg[i] > 0){
                    diff = df;
                    index = i;
                }
            }
            
            ++male_vg[index];       --mm;
            --female_vg[index];     ++ff;
        }
    }
    
    //build pop based on male/female villagers by age group
    for(int i = 0; i < vg_agrps; ++i){
        int cc = male_vg[i];
        while(cc-- > 0){
            int index = rand() % cbk->mvec[i].size();
            agent *cur = cbk->mvec[i][index];
            
            cur->aid = cbk->next_aid++;
            add_member(cur);
            cbk->mvec[i].erase(cbk->mvec[i].begin()+index);
            
            if(cbk->mvec[i].size() == 0 && cc > 0){
                cout << cbk->mbloksIndexB[mid] << " " << i << " " << cc << endl;
                cout << "err: no enough males to build village pop" << endl;
                exit(1);
            }
        }
        
        cc = female_vg[i];
        while(cc-- > 0){
            int index = rand() % cbk->fvec[i].size();
            agent *cur = cbk->fvec[i][index];
            
            cur->aid = cbk->next_aid++;
            add_member(cur);
            cbk->fvec[i].erase(cbk->fvec[i].begin()+index);
            
            if(cbk->fvec[i].size() == 0 && cc > 0){
                cout << cbk->mbloksIndexB[mid] << " " << i << " " << cc << endl;
                cout << "err: no enough females to build village pop" << endl;
                exit(1);
            }
        }
    }
}

void mblok::bld_family_unit(vector<agent*> &m_mvec, vector<agent*> &m_svec, vector<agent*> &m_dvec, vector<agent*> &m_wvec,
                            vector<agent*> &f_mvec, vector<agent*> &f_svec, vector<agent*> &f_dvec, vector<agent*> &f_wvec,
                            vector<agent*> &chld, vector<unit*> &famly){
    
    for(map<int, agent*>::iterator j = mblok_males.begin(); j != mblok_males.end(); ++j){
        agent *p = j->second;
        if(p->age < 15*365){
            p->margs = 's';
            chld.push_back(p);
        }
        else{
            p->margs = 's';         //all male adults are first put into single vector
            m_svec.push_back(p);
        }
    }
    
    for(map<int, agent*>::iterator j = mblok_fmals.begin(); j != mblok_fmals.end(); ++j){
        agent *q = j->second;
        if(q->age < 15*365){
            q->margs = 's';
            chld.push_back(q);
        }
        else{
            rnd_margs(q);   //random margs status for female
            
            if(q->margs == 's') f_svec.push_back(q);
            else if(q->margs == 'm') f_mvec.push_back(q);
            else if(q->margs == 'd') f_dvec.push_back(q);
            else f_wvec.push_back(q);
        }
    }
    
    //match couple to build family units
    match_couple(m_mvec, m_svec, m_dvec, m_wvec, f_mvec, f_svec, f_dvec, f_wvec, famly);
    
    //build single parent family - only female
    while(f_dvec.size() > 0){
        agent *q = f_dvec.back();
        q->spw = NULL;
        famly.push_back(new unit(NULL, q));
        f_dvec.pop_back();
    }
    
    while(f_wvec.size() > 0){
        agent *q = f_wvec.back();
        q->spw = NULL;
        famly.push_back(new unit(NULL, q));
        f_wvec.pop_back();
    }
    
    //allocate child into family units
    allocate_child(chld, famly);
    
    chld.shrink_to_fit();
    
    m_mvec.shrink_to_fit();
    m_svec.shrink_to_fit();
    m_dvec.shrink_to_fit();
    m_wvec.shrink_to_fit();
    
    f_mvec.shrink_to_fit();
    f_svec.shrink_to_fit();
    f_dvec.shrink_to_fit();
    f_wvec.shrink_to_fit();
    
}

void mblok::match_couple(vector<agent*> &m_mvec, vector<agent*> &m_svec, vector<agent*> &m_dvec, vector<agent*> &m_wvec,
                         vector<agent*> &f_mvec, vector<agent*> &f_svec, vector<agent*> &f_dvec, vector<agent*> &f_wvec,
                         vector<unit*> &famly){
    //sort agents by age
    struct _comp{
        bool operator() (const agent *p, const agent *q){ return (p->age < q->age);}
    } _younger;
    
    //sort agent by age to simplify matching couples
    stable_sort(m_svec.begin(), m_svec.end(), _younger);
    stable_sort(f_mvec.begin(), f_mvec.end(), _younger);
    
    //find female's husband
    while(f_mvec.size() > 0){
        //no available males
        if(m_svec.size() == 0 && f_mvec.size() > 0){
            while(f_mvec.size() > 0){
                agent *p = f_mvec.back();
                
                while(p->margs == 'm') rnd_margs(p);   //random margs status for female
                
                if(p->margs == 's') f_svec.push_back(p);
                else if(p->margs == 'w') f_wvec.push_back(p);
                else if(p->margs == 'd') f_dvec.push_back(p);
                
                f_mvec.pop_back();
            }
            break;
        }
        
        agent *q = f_mvec.back();   int q_age = int(q->age/365);
        agent *p = NULL;            int p_age = 0;
        
        int index = -1;
        if(q_age > int(m_svec.back()->age/365+3)){          //too old
            while(q->margs == 'm') rnd_margs(q);
            
            if(q->margs == 's') f_svec.push_back(q);
            else if(q->margs == 'w') f_wvec.push_back(q);
            else if(q->margs == 'd') f_dvec.push_back(q);
            
            f_mvec.pop_back();
            continue;
        }
        else if(q_age < int(m_svec.front()->age/365-5)){   //too young
            while(q->margs == 'm') rnd_margs(q);
            
            if(q->margs == 's') f_svec.push_back(q);
            else if(q->margs == 'w') f_wvec.push_back(q);
            else if(q->margs == 'd') f_dvec.push_back(q);
            
            f_mvec.pop_back();
            continue;
        }
        else{
            if(q_age > int(m_svec.back()->age/365)) index = int(m_svec.size()-1);
            else{
                for(index = int(m_svec.size()-1); index > 0; --index){
                    if(int(m_svec[index]->age/365) == q_age) break;
                    
                    int df_1 = int(m_svec[index]->age/365) - q_age;
                    int df_2 = int(m_svec[index-1]->age/365) - q_age;
                    
                    if(df_1 > 0 && df_2 < 0){
                        if(df_1 <= 5 || abs(df_2) > 3) break;      //prefer male > female, within 5 yrs diff
                        else{
                            --index; break;                 //if m_age - f_age > 5 && f_age - m_age <= 3
                        }
                    }
                }
            }
            p = m_svec[index];      p_age = int(p->age/365);
        }
        
        p->margs = 'm';
        famly.push_back(new unit(p, q));
        f_mvec.pop_back();
        m_svec.erase(m_svec.begin()+index);
    }
    
    stable_sort(f_svec.begin(), f_svec.end(), _younger);
    
    int jj = int(m_svec.size()-1);
    while(jj >= 0){
        agent *p = m_svec[jj];
        rnd_margs(p);
        while(p->margs == 'm') rnd_margs(p);
        --jj;
    }
    
    for(int i = 0; i < famly.size(); ++i){
        unit *cur = famly[i];
        agent *p = cur->father;
        agent *q = cur->mother;
        p->spw = q;
        q->spw = p;
    }
}

void mblok::allocate_child(vector<agent*> &chld_vec, vector<unit*> &famly){
    //sort agents by age
    struct _comp{
        bool operator() (const agent *p, const agent *q){ return (p->age < q->age);}
    } _younger;
    
    //allocate child into family unit
    stable_sort(chld_vec.begin(), chld_vec.end(), _younger);
    vector<unit*> family(famly);
    
    int birth_age[max_births];
    
    while(chld_vec.size() > 0){
        //cout << chld_vec.size() << " " << family.size() << endl;
        int ii = rand() % family.size();
        unit *cur = family[ii];
        
        agent *q = cur->mother;
        int q_age = int(q->age/365);
        
        //max age with child < 15 yrs is 49+14
        if(q_age >= 64) continue;
        
        int rr = q_age - 15;
        rr = rr>max_births ? max_births:rr;
        
        double pp = cbk->child_number_by_age[q_age-15]/(double)rr;
        int num = binomial(rr, pp);
        
        if(num <= cur->child.size()) continue;
        
        memset(birth_age, -1, sizeof(int)*max_births);
        
        int index = num-1;
        int age_dn = cbk->live_birth_order_interval[index][0];
        int age_up = min(cbk->live_birth_order_interval[index][1], q_age-15);
        
        if(age_dn > age_up) continue;
        
        double total = 0;
        for(int j = age_dn; j <= age_up; ++j) total += cbk->live_birth_order_by_age[index][j];
        
        double r = drand48(), prob = 0;
        int jj = age_dn;
        for(jj = age_dn; jj <= age_up; ++jj){
            prob += cbk->live_birth_order_by_age[index][jj]/total;
            if(r < prob) break;
        }
        
        double seg = jj/(double)num;
        int adult_chld = 0;
        for(int i = 0; i < num; ++i){
            birth_age[i] = int(seg*(i+1)+0.5);
            
            if(q_age - (birth_age[i]+15) >= 15)
                ++adult_chld;
        }
        
        if(num - adult_chld <= cur->child.size()) continue;
        
        for(int i = 0; i < cur->child.size(); ++i){
            agent *cp = cur->child[i];
            chld_vec.push_back(cp);
        }
        cur->child.clear();
        cur->avail_ages.clear();
        stable_sort(chld_vec.begin(), chld_vec.end(), _younger);
        
        for(int i = 0; i < num; ++i){
            int chld_age = q_age - (birth_age[i]+15);
            if(chld_age >= 15){
                cur->avail_ages.push_back(chld_age);
                continue;
            }
            
            int pos = binary_search(chld_vec, chld_age);
            
            if(pos == -1 && i > 0){
                for(int j = birth_age[i-1]+1; j < birth_age[i]; ++j){
                    chld_age = q_age - (j+15);
                    pos = binary_search(chld_vec, chld_age);
                    if(pos != -1){
                        birth_age[i] = j;
                        break;
                    }
                }
            }
            
            if(pos == -1 && i < num-1){
                for(int j = birth_age[i]+1; j < birth_age[i+1]; ++j){
                    chld_age = q_age - (j+15);
                    pos = binary_search(chld_vec, chld_age);
                    if(pos != -1){
                        birth_age[i] = j;
                        break;
                    }
                }
            }
            
            if(pos != -1){
                agent *cp = chld_vec[pos];
                cur->child.push_back(cp);
                chld_vec.erase(chld_vec.begin()+pos);
            }
        }
        cur->child.shrink_to_fit();
        cur->avail_ages.shrink_to_fit();
        if(cur->child.size() == rr) family.erase(family.begin()+ii);
    }
    
    /*for(int i = 0; i < famly.size(); ++i){
        unit *cur = famly[i];
        agent *p = cur->father;
        agent *q = cur->mother;
        for(int j = 0; j < cur->child.size(); ++j){
            agent *b = cur->child[j];
            
            if(p != NULL){
                p->add_child(b);
                b->dad = p;
            }
            
            q->add_child(b);
            b->mom = q;
        }
    }*/
    
    family.clear();
    family.shrink_to_fit();
    chld_vec.shrink_to_fit();
}

int mblok::binary_search(vector<agent*> &vec, int age){
    if(vec.size() == 0) return -1;
    
    if(int(vec.front()->age/365) > age || int(vec.back()->age/365) < age) return -1;
    
    int ll = 0, hh = int(vec.size()-1), mid = int((ll+hh)/2);
    while(true){
        if(int(vec[mid]->age/365) > age){
            hh = mid;
            mid = int((ll+hh)/2);
        }
        
        if(int(vec[mid]->age/365) < age){
            ll = mid;
            mid = int((ll+hh)/2);
        }
        
        if(int(vec[mid]->age/365) == age) break;
        
        if(ll == mid || hh == mid) break;
    }
    
    int pos = -1;
    if(int(vec[mid]->age/365) == age) pos = mid;
    else if(int(vec[ll]->age/365) == age) pos = ll;
    else if(int(vec[hh]->age/365) == age) pos = hh;
    
    while(pos > 0 && int(vec[pos-1]->age/365) == int(vec[pos]->age/365)) --pos;
    
    return pos;
}

