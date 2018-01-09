//
//  blockMBlok.cpp
//  synPop
//
//  Created by Sting Xu on 7/11/17.
//  Copyright © 2017 Sting Xu. All rights reserved.
//

#include "blocks.h"

mblok::mblok(int mid, cblok *cbk, double lat, double log){
    this->mid = mid;
    this->cbk = cbk;
    this->lat = lat;
    this->log = log;
}

mblok::~mblok(){
    cbk = NULL;
    mblok_pop.clear();
    
    for(map<int, hhold*>::iterator j = mblok_hholds.begin(); j != mblok_hholds.end(); ++j)
        delete j->second;
    mblok_hholds.clear();
    
    for(map<int, rbldg*>::iterator j = mblok_rbldgs.begin(); j != mblok_rbldgs.end(); ++j)
        delete j->second;
    mblok_rbldgs.clear();
    
    for(map<int, wbldg*>::iterator j = mblok_wbldgs.begin(); j != mblok_wbldgs.end(); ++j)
        delete j->second;
    mblok_wbldgs.clear();
}

void mblok::intlz_pop(){
    if(!cbk->init){
        int mm = cbk->mblok_mpops[mid], ff = cbk->mblok_fpops[mid];
        agrps *pp = cbk->mblok_agrps[mid];
        bld_pop(mm, ff, pp);
    }
    else{
        string file = config;
        file = file + cbk->mbloksIndexB[mid];
        file = file + "_pop";
        
        ifstream in(file.c_str());
        
        string line;
        while(getline(in, line)){
            char *str = new char[line.size()+1];
            std::strcpy(str, line.c_str());
            
            char *p = std::strtok(str, ",");
            int aid = atoi(p);
            
            p = std::strtok(NULL, ",");
            int age = std::atoi(p);
            
            p = std::strtok(NULL, ",");
            char gender = p[0];
            
            agent *g = new agent(aid, age, gender);
            add_agent(g);
            
            delete []str;
        }
        in.close();
    }
}

void mblok::add_hhold(hhold *p){
    mblok_hholds.insert(pair<int, hhold*>(p->hid, p));
}

void mblok::add_agent(agent *p){
    mblok_pop.insert(pair<int, agent*>(p->aid, p));
}

void mblok::rmv_hhold(hhold *p){
    mblok_hholds.erase(p->hid);
    p->rdg->hd = NULL;
    cbk->cblok_rbldgs_vcnt.insert(pair<int, rbldg*>(p->rdg->bid, p->rdg));
    p->rdg = NULL;
    
    delete p;
}

void mblok::rmv_agent(agent *p){
    mblok_pop.erase(p->aid);
}

void mblok::rnd_margs(agent *p){
    int index = int((int(p->age/365)-15)/5);
    index = index>10? 10 : index;
    
    double s_p, m_p, w_p, d_p;
    if(p->gendr == 'm'){
        s_p = cbk->male_single[index];
        m_p = cbk->male_married[index];
        w_p = cbk->male_widowed[index];
        d_p = cbk->male_divorce[index];
    }
    else{
        s_p = cbk->fmal_single[index];
        m_p = cbk->fmal_married[index];
        w_p = cbk->fmal_widowed[index];
        d_p = cbk->fmal_divorce[index];
    }
    
    double r = drandom();
    if(r <= s_p) p->margs = 's';
    else if(r <= s_p + m_p) p->margs = 'm';
    else if(r <= s_p + m_p + w_p) p->margs = 'w';
    else p->margs = 'd';
}

void mblok::bld_hhold(int n){
    //initialize marriage status
    vector<agent*> m_chld, m_svec, m_mvec, m_wvec, m_dvec;
    vector<agent*> f_chld, f_svec, f_mvec, f_wvec, f_dvec;
    
    for(map<int, agent*>::iterator j = mblok_pop.begin(); j != mblok_pop.end(); ++j){
        agent *p = j->second;
        if(p->age < 15*365){
            p->margs = 's';
            if(p->gendr == 'm') m_chld.push_back(p);
            else f_chld.push_back(p);
        }
        else{
            rnd_margs(p);
            if(p->margs == 's'){
                if(p->gendr == 'm') m_svec.push_back(p);
                else f_svec.push_back(p);
            }
            else if(p->margs == 'm'){
                if(p->gendr == 'm') m_mvec.push_back(p);
                else f_mvec.push_back(p);
            }
            else if(p->margs == 'w'){
                if(p->gendr == 'm') m_wvec.push_back(p);
                else f_wvec.push_back(p);
            }
            else{
                if(p->gendr == 'm') m_dvec.push_back(p);
                else f_dvec.push_back(p);
            }
        }
    }
    
    //sort agent by age to simplify building units
    struct _comp{
        bool operator() (const agent *p, const agent *q){ return (p->age < q->age);}
    } _younger;
    
    stable_sort(m_chld.begin(), m_chld.end(), _younger);
    stable_sort(m_svec.begin(), m_svec.end(), _younger);
    stable_sort(m_mvec.begin(), m_mvec.end(), _younger);
    stable_sort(m_wvec.begin(), m_wvec.end(), _younger);
    stable_sort(m_dvec.begin(), m_dvec.end(), _younger);
    
    stable_sort(f_chld.begin(), f_chld.end(), _younger);
    stable_sort(f_svec.begin(), f_svec.end(), _younger);
    stable_sort(f_mvec.begin(), f_mvec.end(), _younger);
    stable_sort(f_wvec.begin(), f_wvec.end(), _younger);
    stable_sort(f_dvec.begin(), f_dvec.end(), _younger);
    
    //adjust married males & females to be equal
    balance_cp(m_mvec, m_svec, m_dvec, m_wvec, f_mvec, f_svec, f_dvec, f_wvec);
    
    vector<unit*> family;
    for(int i = 0; i < m_mvec.size(); ++i){
        agent *p = m_mvec[i];
        agent *q = f_mvec[i];
        family.push_back(new unit(p, q));
    }
    
    if(cbk->mbloksIndexB[mid] == "Avaio"){
        cout << "Avaio:" << endl;
        cout << "units = " << n << endl;
        cout << "pop = " << mblok_pop.size() << endl;
        cout << "cps = " << family.size() << endl;
        for(int i = 0; i < family.size(); ++i){
            cout << int(family[i]->father->age/365) << " " << int(family[i]->mother->age/365) << endl;
        }
    }
    
    for(int i = 0; i < family.size(); ++i){
        delete family[i];
    }
    family.clear();
    family.shrink_to_fit();
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
            int index = irandom() % cbk->mvec[i].size();
            agent *cur = cbk->mvec[i][index];
            
            cur->aid = cbk->next_aid++;
            add_agent(cur);
            cbk->add_agent(cur);
            cbk->mvec[i].erase(cbk->mvec[i].begin()+index);
            
            if(cbk->mvec[i].size() == 0 && cc > 0){
                cout << cbk->mbloksIndexB[mid] << " " << i << " " << cc << endl;
                cout << "err: no enough males to build village pop" << endl;
                exit(1);
            }
        }
        
        cc = female_vg[i];
        while(cc-- > 0){
            int index = irandom() % cbk->fvec[i].size();
            agent *cur = cbk->fvec[i][index];
            
            cur->aid = cbk->next_aid++;
            add_agent(cur);
            cbk->add_agent(cur);
            cbk->fvec[i].erase(cbk->fvec[i].begin()+index);
            
            if(cbk->fvec[i].size() == 0 && cc > 0){
                cout << cbk->mbloksIndexB[mid] << " " << i << " " << cc << endl;
                cout << "err: no enough females to build village pop" << endl;
                exit(1);
            }
        }
    }
}

void mblok::balance_cp(vector<agent*> &m_mvec, vector<agent*> &m_svec, vector<agent*> &m_dvec, vector<agent*> &m_wvec,
                       vector<agent*> &f_mvec, vector<agent*> &f_svec, vector<agent*> &f_dvec, vector<agent*> &f_wvec){
    
    int len = int(m_mvec.size() - f_mvec.size());
    while(len < 0){
        optim_cp_diff(f_mvec, f_svec, f_dvec, f_wvec, m_mvec, m_svec, m_dvec, m_wvec);
        len = int(m_mvec.size() - f_mvec.size());
    }
    
    while(len > 0){
        optim_cp_diff(m_mvec, m_svec, m_dvec, m_wvec, f_mvec, f_svec, f_dvec, f_wvec);
        len = int(m_mvec.size() - f_mvec.size());
    }
}

void mblok::optim_cp_diff(vector<agent *> &m_vec, vector<agent *> &s_vec, vector<agent *> &d_vec, vector<agent *> &w_vec,
                          vector<agent *> &mvec, vector<agent *> &svec, vector<agent *> &dvec, vector<agent *> &wvec){
    int t = int(svec.size() + dvec.size() + wvec.size());
    if(t == 0){
        int df_1, df_2;
        agent *cur = NULL;
        
        cur = m_vec.front();
        m_vec.erase(m_vec.begin());
        df_1 = match_cp_diff(mvec, m_vec);
        m_vec.insert(m_vec.begin(), cur);
        
        cur = m_vec.back();
        m_vec.pop_back();
        df_2 = match_cp_diff(mvec, m_vec);
        m_vec.insert(m_vec.end(), cur);
        
        if(df_1 <= df_2){
            cur = m_vec.front();
            m_vec.erase(m_vec.begin());
        }
        else{
            cur = m_vec.back();
            m_vec.pop_back();
        }
        
        //insert cur to other vec
        while(cur->margs == 'm') rnd_margs(cur);
        
        if(cur->margs == 's'){
            int pos = locate_age_pos(s_vec, cur);
            s_vec.insert(s_vec.begin()+pos, cur);
        }
        else if(cur->margs == 'd'){
            int pos = locate_age_pos(d_vec, cur);
            d_vec.insert(d_vec.begin()+pos, cur);
        }
        else if(cur->margs == 'w'){
            int pos = locate_age_pos(w_vec, cur);
            w_vec.insert(w_vec.begin()+pos, cur);
        }
        
        m_vec.shrink_to_fit();
        s_vec.shrink_to_fit();
        d_vec.shrink_to_fit();
        w_vec.shrink_to_fit();
    }
    else{
        int sign = -1, diff = -1;
        
        int p[6];
        int df_1 = 0, df_2 = 0;
        if(svec.size() > 0){                //test single vec
            p[0] = locate_age_pos(mvec, svec.front());
            mvec.insert(mvec.begin()+p[0], svec.front());
            df_1 = match_cp_diff(mvec, m_vec);
            mvec.erase(mvec.begin()+p[0]);
            
            if(svec.size() != 1){
                p[1] = locate_age_pos(mvec, svec.back());
                mvec.insert(mvec.begin()+p[1], svec.back());
                df_2 = match_cp_diff(mvec, m_vec);
                mvec.erase(mvec.begin()+p[1]);
            }
            
            if(svec.size() == 1 || df_1 <= df_2){
                sign = 0;
                diff = df_1;
            }
            else{
                sign = 1;
                diff = df_2;
            }
        }
        
        if(dvec.size() > 0){                //test divorce vec
            p[2] = locate_age_pos(mvec, dvec.front());
            mvec.insert(mvec.begin()+p[2], dvec.front());
            df_1 = match_cp_diff(mvec, m_vec);
            mvec.erase(mvec.begin()+p[2]);
            
            if(dvec.size() != 1){
                p[3] = locate_age_pos(mvec, dvec.back());
                mvec.insert(mvec.begin()+p[3], dvec.back());
                df_2 = match_cp_diff(mvec, m_vec);
                mvec.erase(mvec.begin()+p[3]);
            }
            
            if(dvec.size() == 1 || df_1 <= df_2){
                if(df_1 < diff || sign == -1){
                    sign = 2;
                    diff = df_1;
                }
            }
            else{
                if(df_2 < diff || sign == -1){
                    sign = 3;
                    diff = df_2;
                }
            }
        }
        
        if(wvec.size() > 0){
            p[4] = locate_age_pos(mvec, wvec.front());
            mvec.insert(mvec.begin()+p[4], wvec.front());
            df_1 = match_cp_diff(mvec, m_vec);
            mvec.erase(mvec.begin()+p[4]);
            
            if(wvec.size() != 1){
                p[5] = locate_age_pos(mvec, wvec.back());
                mvec.insert(mvec.begin()+p[5], wvec.back());
                df_2 = match_cp_diff(mvec, m_vec);
                mvec.erase(mvec.begin()+p[5]);
            }
            
            if(wvec.size() == 1 || df_1 <= df_2){
                if(df_1 < diff || sign == -1){
                    sign = 4;
                    diff = df_1;
                }
            }
            else{
                if(df_2 < diff || sign == -1){
                    sign = 5;
                    diff = df_2;
                }
            }
        }
        
        switch (sign) {
            case 0:
                mvec.insert(mvec.begin()+p[0], svec.front());
                svec.front()->margs = 'm';
                svec.erase(svec.begin());
                break;
            case 1:
                mvec.insert(mvec.begin()+p[1], svec.back());
                svec.back()->margs = 'm';
                svec.pop_back();
                break;
            case 2:
                mvec.insert(mvec.begin()+p[2], dvec.front());
                dvec.front()->margs = 'm';
                dvec.erase(dvec.begin());
                break;
            case 3:
                mvec.insert(mvec.begin()+p[3], dvec.back());
                dvec.back()->margs = 'm';
                dvec.pop_back();
                break;
            case 4:
                mvec.insert(mvec.begin()+p[4], wvec.front());
                wvec.front()->margs = 'm';
                wvec.erase(wvec.begin());
                break;
            case 5:
                mvec.insert(mvec.begin()+p[5], wvec.back());
                wvec.back()->margs = 'm';
                wvec.pop_back();
                break;
            default:
                break;
        }
        
        mvec.shrink_to_fit();
        svec.shrink_to_fit();
        dvec.shrink_to_fit();
        wvec.shrink_to_fit();
    }
}

int mblok::locate_age_pos(vector<agent *> &m_mvec, agent *p){
    int index = -1;
    if(m_mvec.size() == 0) index = 0;
    else{
        if(p->age <= m_mvec.front()->age) index = 0;
        else if(p->age >= m_mvec.back()->age) index = int(m_mvec.size());
        else{
            for(index = 1; index < m_mvec.size()-1; ++index){
                if(p->age > m_mvec[index-1]->age && p->age <= m_mvec[index]->age)
                    break;
            }
        }
    }
    
    if(index == -1){
        cout << "err: unable to locate pos" << endl;
        exit(1);
    }
    
    return index;
}

int mblok::match_cp_diff(vector<agent*> &m_mvec, vector<agent*> &f_mvec){
    //minimize total age diff of all couples
    int index = -1, diff = -1;
    if(m_mvec.size() >= f_mvec.size()){
        for(int i = 0; i < m_mvec.size()-f_mvec.size(); ++i){
            int df = 0;
            for(int j = 0; j < f_mvec.size(); ++j){
                df += int(abs(m_mvec[i+j]->age - f_mvec[j]->age)/365);
            }
            
            if(diff == -1){
                diff = df;
                index = i;
                continue;
            }
            
            if(diff > df){
                diff = df;
                index = i;
            }
        }
    }
    else{
        for(int i = 0; i < f_mvec.size()-m_mvec.size(); ++i){
            int df = 0;
            for(int j = 0; j < m_mvec.size(); ++j){
                df += int(abs(f_mvec[i+j]->age - m_mvec[j]->age)/365);
            }
            
            if(diff == -1){
                diff = df;
                index = i;
                continue;
            }
            
            if(diff > df){
                diff = df;
                index = i;
            }
        }
    }
    return diff;
}

void mblok::adpt_chldrs(hhold *p){
    agent *pp = NULL;
    for(map<int, agent*>::iterator j = cbk->fmal_marrd.begin(); j != cbk->fmal_marrd.end(); ++j){
        if(j->second->chldr.size() == 0){
            pp = j->second;
            break;
        }
    }
    
    if(pp == NULL){
        cout << "err no suitable family for adopted kids in mblok::adpt_chldrs" << endl;
        exit(1);
    }
    
    if(pp->spw == NULL){
        cout << "err married female's spw == NULL in mblok::adpt_chldrs" << endl;
        exit(1);
    }
    
    while(p->mmbrs.size() > 0){
        agent *cur = p->mmbrs.begin()->second;
        
        pp->add_child(cur);
        pp->spw->add_child(cur);
        
        cur->add_parent(pp);
        cur->add_parent(pp->spw);
        
        p->rmv_mmbr(cur);
        pp->hd->add_mmbr(cur);
    }
    
    pp->hd->updt_hhold();
}
