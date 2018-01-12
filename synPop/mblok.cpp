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
    vector<agent*> m_svec, m_mvec, m_wvec, m_dvec;
    vector<agent*> f_svec, f_mvec, f_wvec, f_dvec;
    vector<agent*> chld;
    
    vector<unit*> famly;
    bld_family_unit(m_mvec, m_svec, m_dvec, m_wvec, f_mvec, f_svec, f_dvec, f_wvec, chld, famly);

    
    //adjust married males & females to be equal
    //balance_cp(m_mvec, m_svec, m_dvec, m_wvec, f_mvec, f_svec, f_dvec, f_wvec);
    
    if(cbk->mbloksIndexB[mid] == "Avaio"){
        cout << "Avaio:" << endl;
        cout << "units = " << n << endl;
        cout << "pop = " << mblok_pop.size() << endl;
        cout << "cps = " << famly.size() << endl;
        for(int i = 0; i < famly.size(); ++i){
            agent *p = famly[i]->father;
            agent *q = famly[i]->mother;
            
            if(p != NULL){
                cout << "m: " << int(p->age/365) << " " << p->margs << endl;
            }
            cout << "f: " << int(q->age/365) << " " << q->margs << " " << q->chldr.size() << endl;
            for(map<int, agent*>::iterator j = q->chldr.begin(); j != q->chldr.end(); ++j){
                cout << "  c: " << int(j->second->age/365) << " " << j->second->gendr << " " << j->second->margs << endl;
            }
        }
    }
    
    for(int i = 0; i < famly.size(); ++i){
        delete famly[i];
    }
    famly.clear();
    famly.shrink_to_fit();
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

void mblok::bld_family_unit(vector<agent*> &m_mvec, vector<agent*> &m_svec, vector<agent*> &m_dvec, vector<agent*> &m_wvec,
                            vector<agent*> &f_mvec, vector<agent*> &f_svec, vector<agent*> &f_dvec, vector<agent*> &f_wvec,
                            vector<agent*> &chld, vector<unit*> &famly){
    
    for(map<int, agent*>::iterator j = mblok_pop.begin(); j != mblok_pop.end(); ++j){
        agent *p = j->second;
        if(p->age < 15*365){
            p->margs = 's';
            chld.push_back(p);
        }
        else{
            if(p->gendr == 'm'){
                p->margs = 's';
                m_svec.push_back(p);
                continue;
            }
            
            rnd_margs(p);   //random margs status for female
            if(p->margs == 'm') f_mvec.push_back(p);
            else{
                p->margs = 's';
                f_svec.push_back(p);
            }
        }
    }
    
    //match couple to build family units
    match_couple(m_mvec, m_svec, m_dvec, m_wvec, f_mvec, f_svec, f_dvec, f_wvec, famly);
    
    //build single parent family - only female
    while(f_dvec.size() > 0){
        agent *q = f_dvec.back();
        famly.push_back(new unit(NULL, q));
        f_dvec.pop_back();
    }
    
    while(f_wvec.size() > 0){
        agent *q = f_wvec.back();
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
    
    //redo for male single
    int jj = int(m_svec.size()-1);
    while(jj >= 0){
        agent *p = m_svec[jj];
        rnd_margs(p);
        if(p->margs == 'm'){
            m_mvec.push_back(p);
            m_svec.erase(m_svec.begin()+jj);
        }
        else if(p->margs == 'd'){
            m_dvec.push_back(p);
            m_svec.erase(m_svec.begin()+jj);
        }
        else if(p->margs == 'w'){
            m_wvec.push_back(p);
            m_svec.erase(m_svec.begin()+jj);
        }
        --jj;
    }
    
    stable_sort(m_mvec.begin(), m_mvec.end(), _younger);
    
    while(m_mvec.size() > 0){
        //no available females
        if(m_mvec.size() > 0 && f_svec.size() == 0){
            while(m_mvec.size() > 0){
                agent *p = m_mvec.back();
                while(p->margs == 'm') rnd_margs(p);
                
                if(p->margs == 's') m_svec.push_back(p);
                else if(p->margs == 'w') m_wvec.push_back(p);
                else if(p->margs == 'd') m_dvec.push_back(p);
                
                m_mvec.pop_back();
            }
            break;
        }
        
        agent *p = m_mvec.back();       int p_age = int(p->age/365);
        agent *q = NULL;                int q_age = 0;
        
        int index = -1;
        if(p_age > int(f_svec.back()->age/365+5)){          //too old
            while(p->margs == 'm') rnd_margs(p);
            
            if(p->margs == 's') m_svec.push_back(p);
            else if(p->margs == 'w') m_wvec.push_back(p);
            else if(p->margs == 'd') m_dvec.push_back(p);
            
            m_mvec.pop_back();
            continue;
        }
        else if(p_age < int(f_svec.front()->age/365-3)){    //too young
            while(p->margs == 'm') rnd_margs(p);
            
            if(p->margs == 's') m_svec.push_back(p);
            else if(p->margs == 'w') m_wvec.push_back(p);
            else if(p->margs == 'd') m_dvec.push_back(p);
            
            m_mvec.pop_back();
            continue;
        }
        else{
            if(p_age > int(f_svec.back()->age/365)) index = int(f_svec.size()-1);
            else{
                for(index = int(f_svec.size()-1); index > 0; --index){
                    if(int(f_svec[index]->age/365) == p_age) break;
                    int df_1 = p_age - int(f_svec[index]->age/365);
                    int df_2 = p_age - int(f_svec[index-1]->age/365);
                    
                    if(df_1 < 0 && df_2 > 0){
                        if(df_2 <= 5 || abs(df_1) > 3){
                            -- index; break;
                        }
                        else break;
                    }
                }
            }
            q = f_svec[index];      q_age = int(q->age/365);
        }
        
        q->margs = 'm';
        famly.push_back(new unit(p, q));
        m_mvec.pop_back();
        f_svec.erase(f_svec.begin()+index);
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
    
    while(chld_vec.size() > 0){
        int ii = irandom() % family.size();
        unit *cur = family[ii];
        agent *p = cur->father, *q = cur->mother;
        int q_age = int(q->age/365);
        
        //max age with child < 15 yrs is 49+14
        if(q_age >= 64) continue;
        
        int rr = q_age - 15;
        rr = rr>10 ? 10:rr;
        
        double pp = cbk->child_number_by_age[q_age-15]/(double)rr;
        int rand_num = binomial(rr, pp);
        int num = rand_num - int(q->chldr.size());
        
        if(num <= 0) continue;
        
        int *birth_age = new int[num];
        memset(birth_age, -1, sizeof(int)*num);
        
        int index = num-1, last_age = q_age-15+1;
        while(index >= 0){
            int age_dn = cbk->live_birth_order_interval[index][0];
            int age_up = min(cbk->live_birth_order_interval[index][1], last_age-1);
            
            if(age_dn > age_up) break;
            
            //next kid's age_dn = this age_dn, impossible to finish
            if(index > 0 && age_dn == age_up && age_dn == cbk->live_birth_order_interval[index-1][0]) break;
            
            double total = 0;
            for(int j = age_dn; j <= age_up; ++j) total += cbk->live_birth_order_by_age[index][j];
            
            double r = drandom(), prob = 0;
            int jj = age_dn;
            for(jj = age_dn; jj <= age_up; ++jj){
                prob += cbk->live_birth_order_by_age[index][jj]/total;
                if(r < prob) break;
            }
            
            if(jj < index) continue;
            if(index > 0 && jj <= cbk->live_birth_order_interval[index-1][0]) continue;
            
            birth_age[index] = jj;  last_age = jj;
            if(jj == index){
                for(int k = index-1; k >= 0; --k) birth_age[k] = k;
                break;
            }
            else --index;
        }
        
        if(birth_age[0] == -1){
            delete []birth_age;
            continue;    //not initialized
        }
        
        for(int i = 0; i < num; ++i){
            int chld_age = q_age - (birth_age[i]+15);
            if(chld_age >= 15) continue;
            
            int pos = binary_search(chld_vec, chld_age);
            if(pos == -1) continue;
            
            agent *cp = chld_vec[pos];
            if(p != NULL){
                cp->add_parent(p);
                p->add_child(cp);
            }
            
            q->add_child(cp);
            cp->add_parent(q);
            
            cur->child.push_back(cp);
            chld_vec.erase(chld_vec.begin()+pos);
        }
        delete []birth_age;
        if(cur->mother->chldr.size() == rand_num) family.erase(family.begin()+ii);
    }
    
    family.clear();
    family.shrink_to_fit();
    chld_vec.shrink_to_fit();
}

int mblok::binary_search(vector<agent*> &vec, int age){
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
    
    return pos;
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
