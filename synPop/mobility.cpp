//
//  blockMMod.cpp
//  synPop
//
//  Created by Sting Xu on 6/11/17.
//  Copyright © 2017 Sting Xu. All rights reserved.
//

#include "block.h"

void cblok::radt_model(char m){
    double *d = NULL;
    if(m == 'r') d = road_dst;
    else if(m == 'e') d = euclid_dst;
    
    double c_jobs = cannery_avg;
    
    int mid = mbloksIndexA["Atuu"];
    double total_pop = cpop;
    total_pop -= int(mbloks[mid]->mblok_males.size() + mbloks[mid]->mblok_fmals.size());
    
    for(map<int, mblok*>::iterator j = mbloks.begin(); j != mbloks.end(); ++j){
        mblok *mbk = j->second;
        int mpop = int(mbk->mblok_males.size() + mbk->mblok_fmals.size());
        
        if(mbk->mid != mid) mbk->jobs = labor_force*(1-c_jobs)*mpop/total_pop;
        else mbk->jobs = labor_force*c_jobs;
    }
    
    struct _comp_cnode_s{
        bool operator() (const mblok::c_node *p, const mblok::c_node *q){ return (p->p < q->p);}
    } _smaller;
    
    for(map<int, mblok*>::iterator j = mbloks.begin(); j != mbloks.end(); ++j){
        mblok *src = j->second;
        int src_id = src->mid;
        
        for(map<int, mblok*>::iterator k = mbloks.begin(); k != mbloks.end(); ++k){
            mblok *dst = k->second;
            int dst_id = dst->mid;
            
            if(dst_id == src_id) continue;
            
            int index = (min(src_id, dst_id)-1)*(meshblocks*2-min(src_id, dst_id))/2 + abs(dst_id-src_id) - 1;
            src->mblok_commuting.insert(pair<int, double>(dst_id, 0));
            src->mblok_comm.push_back(new mblok::c_node(dst_id, d[index]));
        }
        
        stable_sort(src->mblok_comm.begin(), src->mblok_comm.end(), _smaller);
        
        double mi = src->jobs, ti = src->labors*commuting_ratio, total = 0;
        for(int k = 0; k < src->mblok_comm.size(); ++k){
            double nj = mbloks[src->mblok_comm[k]->mid]->jobs, sij = 0;
            for(int i = 0; i < k; ++i) sij += mbloks[src->mblok_comm[i]->mid]->jobs;
            
            double tij = ti*mi*nj/(mi+nj)/(mi+nj+sij);
            
            src->mblok_commuting[src->mblok_comm[k]->mid] = tij;
            total += tij;
        }
        
        for(map<int, double>::iterator k = src->mblok_commuting.begin(); k != src->mblok_commuting.end(); ++k){
            k->second = k->second/total * commuting_ratio * src->labors;
        }
        
        src->mblok_commuting.insert(pair<int, double>(src->mid, (1-commuting_ratio)*src->labors));
        
        for(int k = 0; k < src->mblok_comm.size(); ++k) delete src->mblok_comm[k];
        src->mblok_comm.clear();    src->mblok_comm.shrink_to_fit();
    }
    
    struct _comp_cnode_l{
        bool operator() (const mblok::c_node *p, const mblok::c_node *q){ return (p->p > q->p);}
    } _larger;
    
    for(map<int, mblok*>::iterator j = mbloks.begin(); j != mbloks.end(); ++j){
        mblok *mbk = j->second;
        
        double workers = 0;
        for(map<int, mblok*>::iterator k = mbloks.begin(); k != mbloks.end(); ++k){
            workers += k->second->mblok_commuting[mbk->mid];
        }
        
        double ratio = mbk->jobs / workers;
        for(map<int, mblok*>::iterator k = mbloks.begin(); k != mbloks.end(); ++k){
            mblok *mbk_2 = k->second;
            
            double c = mbk_2->mblok_commuting[mbk->mid];
            
            double r = (mbk_2->labors-ratio*c) / (mbk_2->labors-c);
            for(map<int, double>::iterator it = mbk_2->mblok_commuting.begin(); it != mbk_2->mblok_commuting.end(); ++it){
                if(it->first == mbk->mid) it->second *= ratio;
                else it->second *= r;
            }
        }
    }
    
    for(map<int, mblok*>::iterator j = mbloks.begin(); j != mbloks.end(); ++j){
        mblok *mbk = j->second;
        
        double norm = 0;
        for(map<int, double>::iterator k = mbk->mblok_commuting.begin(); k != mbk->mblok_commuting.end(); ++k){
            mbk->mblok_comm.push_back(new mblok::c_node(k->first, k->second));
            norm += k->second;
        }
        
        for(int i = 0; i < mbk->mblok_comm.size(); ++i){
            mbk->mblok_comm[i]->p /= norm;
        }
        
        stable_sort(mbk->mblok_comm.begin(), mbk->mblok_comm.end(), _larger);
    }
    
    
    string file = outdir;   file = file + "commuting.csv";
    ofstream out(file.c_str());
    out << std::setprecision(2) << std::setiosflags(std::ios::fixed);
    
    for(map<int, mblok*>::iterator j = mbloks.begin(); j != mbloks.end(); ++j){
        mblok *mbk = j->second;
        for(int i = 0; i < mbk->mblok_comm.size(); ++i){
            if(mbk->mblok_comm[i]->p * mbk->jobs >= 30)
                out << mbk->lat << "," << mbk->log << "," << mbloks[mbk->mblok_comm[i]->mid]->lat << "," << mbloks[mbk->mblok_comm[i]->mid]->log << "," << mbk->mblok_comm[i]->p*mbk->jobs << endl;
        }
    }
    out.close();
}
