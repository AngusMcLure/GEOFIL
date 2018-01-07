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

void mblok::bld_hhold(pf f){
    
    //r_size(f, p);
}

void mblok::bld_pop(int mm, int ff, agrps *pp){
    int *male_vg = cbk->mblok_agrps[mid]->male;
    int *female_vg = cbk->mblok_agrps[mid]->female;
    
    bool no_data = true;
    for(int i = 0; i < vg_agrps; ++i){
        if(male_vg[i] > 0){
            no_data = false;
            break;
        }
    }
    
    //male & female pop by age group in village unavailable
    if(no_data){
        double males = 0, females = 0;
        for(int i = 0; i < vg_agrps; ++i){
            males += cbk->mvec[i].size();
            females += cbk->fvec[i].size();
        }
        
        int c_mm = mm, c_ff = ff;
        for(int i = 0; i < vg_agrps; ++i){
            male_vg[i] = int(cbk->mvec[i].size()*mm/males+0.5);         c_mm -= male_vg[i];
            female_vg[i] = int(cbk->fvec[i].size()*ff/females+0.5);     c_ff -= female_vg[i];
        }
        
        while(c_mm > 0){
            int index = rand() % vg_agrps;
            if(cbk->mvec[index].size() - male_vg[index] > 0){
                ++ male_vg[index];
                -- c_mm;
            }
        }
        
        while(c_mm < 0){
            -- male_vg[rand()%vg_agrps];
            ++ c_mm;
        }
        
        while(c_ff > 0){
            int index = rand() % vg_agrps;
            if(cbk->fvec[index].size() - female_vg[index] > 0){
                ++ female_vg[index];
                -- c_ff;
            }
        }
        
        while(c_ff < 0){
            -- female_vg[rand()%vg_agrps];
            ++ c_ff;
        }
    }
    
    //build pop based on male/female villagers by age group
    for(int i = 0; i < vg_agrps; ++i){
        while(male_vg[i]-- > 0){
            int index = rand() % cbk->mvec[i].size();
            agent *cur = cbk->mvec[i][index];
            
            cur->aid = cbk->next_aid++;
            add_agent(cur);
            cbk->add_agent(cur);
            cbk->mvec[i].erase(cbk->mvec[i].begin()+index);
            
            if(cbk->mvec[i].size() == 0 && male_vg[i] > 0){
                cout << "err: no enough males to build village pop" << endl;
                exit(1);
            }
        }
        
        while(female_vg[i]-- > 0){
            int index = rand() % cbk->fvec[i].size();
            agent *cur = cbk->fvec[i][index];
            
            cur->aid = cbk->next_aid++;
            add_agent(cur);
            cbk->add_agent(cur);
            cbk->fvec[i].erase(cbk->fvec[i].begin()+index);
            
            if(cbk->fvec[i].size() == 0 && female_vg[i] > 0){
                cout << "err: no enough females to build village pop" << endl;
                exit(1);
            }
        }
    }
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
